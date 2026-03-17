/*******************************************************************************

 This file is part of Set--Based Graph Library.

 SBG Library is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 SBG Library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with SBG Library.  If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#include "algorithms/matching/bfs_matching.hpp"
#include "sbg/pwmap_fact.hpp"
#include "sbg/set_fact.hpp"
#include "util/logger.hpp"
#include "util/time_profiler.hpp"

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// BFS Matching Algorithm ------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

BFSMatching::ExitCondition::ExitCondition(bool full_match, bool found_paths)
  : full_match_(full_match), found_paths_(found_paths) {}

bool BFSMatching::ExitCondition::full_match() { return full_match_; }

bool BFSMatching::ExitCondition::found_paths() { return found_paths_; }

bool BFSMatching::ExitCondition::isSatisfied()
{
  return full_match_ || !found_paths_;
}

// Algorithm -------------------------------------------------------------------

BFSMatching::BFSMatching() : _M(SET_FACT.createSet()), _dsbg()
  , _direction(Direction::kForward) {}

void BFSMatching::swapEdgesDirection(const Set& E)
{
  PWMap mapB = _dsbg.mapB();
  PWMap mapD = _dsbg.mapD();

  PWMap temp_mapB = mapB.restrict(E);
  mapB = mapD.restrict(E).combine(std::move(mapB));
  mapD = temp_mapB.restrict(E).combine(std::move(mapD));

  _dsbg = DirectedSBG{_dsbg.V(), _dsbg.Vmap(), mapB, mapD, _dsbg.Emap()};
}

PWMap BFSMatching::partitionSubsetEdges() const
{
  PWMap result = PWMAP_FACT.createPWMap();
  Set free_edges = _dsbg.E().difference(_M);
  std::size_t arity = free_edges.arity();
  NAT j = 1;
  PWMap Emap = _dsbg.Emap();
  Set set_edges = Emap.image();
  while (!set_edges.isEmpty()) {
    Set min_elem_set = SET_FACT.createSet(set_edges.minElem());
    Set domain_edges = Emap.preImage(min_elem_set);

    Expression matched_expr{MD_NAT{arity, j}};
    Map matched_map{_M.intersection(domain_edges), matched_expr};
    ++j;
    Expression free_expr{MD_NAT{arity, j}};
    Map free_map{free_edges.intersection(domain_edges), free_expr};
    ++j;

    result.emplaceBack(matched_map);
    result.emplaceBack(free_map);

    set_edges = set_edges.difference(min_elem_set);
  }

  return result;
}

Set BFSMatching::edgesInPaths(const PWMap& smap, const Set& E) const
{
  PWMap mapB = _dsbg.mapB().restrict(E);
  PWMap mapD = _dsbg.mapD().restrict(E);

  // Vertices that are successors of other vertices in a path
  Set not_fixed = smap.domain().difference(smap.fixedPoints());
  Set succs = smap.restrict(not_fixed).image();
  // Edges whose endings are successors 
  Set ending_edges = mapD.preImage(succs);
  // Map from a 'successor' edge to its start
  PWMap auxB = mapB.restrict(ending_edges);
  // Map from edge to the successor of its start
  PWMap map_succs = smap.composition(auxB);
 
  return map_succs.equalImage(mapD);
}

Set BFSMatching::directedStep(const Set& E, const Set& right_vertices)
{
  PWMap mapB = _dsbg.mapB().restrict(E);
  PWMap mapD = _dsbg.mapD().restrict(E);
  PWMap Emap = partitionSubsetEdges().restrict(E);

  // Calculate unmatched vertices in the side determined by the current
  // direction of edges
  Set forward_vertices = _dsbg.V().difference(right_vertices);
  Set matched_forward_vertices = mapB.image(_M);
  if (_direction == Direction::kBackward) {
    forward_vertices = right_vertices;
  }
  Set unmatched_forward_vertices
    = forward_vertices.difference(matched_forward_vertices);

  // Detect paths leading to unmatched_forward_vertices
  DirectedSBG restricted_dsbg{_dsbg.V(), _dsbg.Vmap(), mapB, mapD, Emap};
  BFSPaths paths;
  PWMap smap = paths.calculate(restricted_dsbg, unmatched_forward_vertices);

  // Keep edges
  Set paths_edges = edgesInPaths(smap, E);
  PWMap rmap = smap.mapInf();
  Set reach_unmatched = rmap.preImage(unmatched_forward_vertices);
  paths_edges = paths_edges.intersection(mapD.preImage(reach_unmatched));

  Util::DEBUG_LOG << "paths_edges in " << _direction << " direction: "
    << paths_edges << "\n";

  return paths_edges;
}

BFSMatching::ExitCondition BFSMatching::step(const Set& right_vertices)
{
  Set E = _dsbg.E();

  // Forward direction
  Set paths_edgesD = directedStep(E, right_vertices);

  // Backward direction
  swapEdgesDirection(E);
  _direction = Direction::kBackward;
  Set paths_edgesB = directedStep(paths_edgesD, right_vertices);

  // Calculate augmenting paths and swap edges in these paths
  Set augmenting_edges = paths_edgesB.intersection(paths_edgesD);
  Util::DEBUG_LOG << "augmenting paths: " << augmenting_edges << "\n";
  swapEdgesDirection(augmenting_edges);

  // Swap directions in all graph
  swapEdgesDirection(E);
  _direction = Direction::kForward;

  // Calculate new matched edges
  _M = _dsbg.mapD().preImage(right_vertices);

  // Calculate exit conditions
  Set matchedU = _dsbg.mapD().image(_M);
  bool full_match = right_vertices.difference(matchedU).isEmpty();
  bool found_paths = !augmenting_edges.isEmpty();

  return ExitCondition{full_match, found_paths};
}

void BFSMatching::init(const BipartiteSBG& bsbg)
{
  PWMap map1 = bsbg.map1();
  map1.compact();
  PWMap map2 = bsbg.map2();
  map2.compact();

  Set Y = bsbg.Y();
  PWMap map1_toY = map1.restrict(map1.preImage(Y));
  PWMap map2_toY = map2.restrict(map2.preImage(Y));
  PWMap mapB = std::move(map1_toY).concatenation(std::move(map2_toY));

  Set X = bsbg.X();
  PWMap map1_toX = map1.restrict(map1.preImage(X));
  PWMap map2_toX = map2.restrict(map2.preImage(X));
  PWMap mapD = std::move(map1_toX).concatenation(std::move(map2_toX));

  Set V = bsbg.V();
  V.compact();
  PWMap Vmap = bsbg.Vmap();
  Vmap.compact();
  PWMap Emap = bsbg.Emap();
  Emap.compact();
  _dsbg = DirectedSBG{V, Vmap, mapB, mapD, Emap};
}

MatchData BFSMatching::calculate(const BipartiteSBG& bsbg)
{
  Util::Internal::TimeProfiler profiler{"Total matching exec time: "};

  init(bsbg);
  Set right_vertices = bsbg.Y();

  ExitCondition exit_cond{false, false};
  do {
    exit_cond = step(right_vertices);
  } while (!exit_cond.isSatisfied());

  _M.compact();
  MatchData result{bsbg, _M, exit_cond.full_match()};
  Util::SBG_LOG << result << "\n\n";

  return result;
}

} // namespace detail

} // namespace LIB

} // namespace SBG
