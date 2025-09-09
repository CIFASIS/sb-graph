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

#include <chrono>

#include "algorithms/matching/bfs_matching.hpp"
#include "algorithms/matching/bfs_paths.hpp"
#include "util/logger.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// BFS Matching Algorithm ------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

BFSMatching::ExitCondition::ExitCondition(bool full_match, bool found_paths)
  : full_match_(full_match), found_paths_(found_paths) {}

bool BFSMatching::ExitCondition::full_match() { return full_match_; }

bool BFSMatching::ExitCondition::found_paths() { return found_paths_; }

BFSMatching::BFSMatching(const PWMapAF& fact)
  : MatchStrategy(fact), M_(fact.createSet()), dsbg_(DSBG(fact))
    , direction_(Direction::kForward) {}

void BFSMatching::swapEdgesDirection(const Set& E)
{
  PWMap mapB = dsbg_.mapB();
  PWMap mapD = dsbg_.mapD();

  PWMap temp_mapB = mapB.restrict(E);
  mapB = mapD.restrict(E).combine(mapB);
  mapD = temp_mapB.restrict(E).combine(mapD);

  dsbg_ = DSBG(fact_, dsbg_.V().compact(), dsbg_.Vmap().compact()
    , mapB.compact(), mapD.compact(), dsbg_.Emap().compact()
    , dsbg_.subEmap().compact());

  return;
}

PWMap BFSMatching::partitionSubsetEdges() const
{
  PWMap result = fact_.createPWMap();
  Set free_edges = dsbg_.E().difference(M_);
  unsigned int dims = free_edges.arity();
  unsigned int j = 1;
  for (const Map& subset_edge : dsbg_.subEmap()) {
    Set dom = subset_edge.dom();
    Exp matched_exp(MD_NAT(dims, j));
    Map matched_map = fact_.createMap(M_.intersection(dom), matched_exp);
    ++j;
    Exp free_exp(MD_NAT(dims, j));
    Map free_map = fact_.createMap(free_edges.intersection(dom), free_exp);
    ++j;

    result.emplaceBack(matched_map);
    result.emplaceBack(free_map);
  }

  return result;
}

Set BFSMatching::edgesInPaths(const PWMap& smap, const Set& E) const
{
  PWMap mapB = dsbg_.mapB().restrict(E);
  PWMap mapD = dsbg_.mapD().restrict(E);

  // Vertices that are successors of other vertices in a path
  Set not_fixed = smap.dom().difference(smap.fixedPoints());
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
  PWMap mapB = dsbg_.mapB().restrict(E).compact();
  PWMap mapD = dsbg_.mapD().restrict(E).compact();
  PWMap Emap = dsbg_.Emap().restrict(E).compact();
  PWMap subEmap = partitionSubsetEdges().restrict(E).compact();

  // Calculate unmatched vertices in the side determined by the current
  // direction of edges
  Set forward_vertices = dsbg_.V().difference(right_vertices);
  Set matched_forward_vertices = mapB.image(M_);
  if (direction_ == Direction::kBackward) {
    forward_vertices = right_vertices;
  }
  Set unmatched_forward_vertices
    = forward_vertices.difference(matched_forward_vertices);

  // Detect paths leading to unmatched_forward_vertices
  Paths paths(fact_);
  DSBG restricted_dsbg(fact_, dsbg_.V(), dsbg_.Vmap(), mapB, mapD
    , Emap, subEmap);
  PWMap smap = paths.calculate(restricted_dsbg, unmatched_forward_vertices);

  // Keep edges
  Set paths_edges = edgesInPaths(smap, E);
  PWMap rmap = smap.mapInf();
  Set reach_unmatched = rmap.preImage(unmatched_forward_vertices);
  paths_edges = paths_edges.intersection(mapD.preImage(reach_unmatched));

  Util::DEBUG_LOG << "paths_edges in " << direction_ << " direction: "
    << paths_edges << "\n";

  return paths_edges;
}

BFSMatching::ExitCondition BFSMatching::step(const Set& right_vertices)
{
  Set E = dsbg_.E();

  // Forward direction
  Set paths_edgesD = directedStep(E, right_vertices);

  // Backward direction
  swapEdgesDirection(E);
  direction_ = Direction::kBackward;
  Set paths_edgesB = directedStep(paths_edgesD, right_vertices);

  // Calculate augmenting paths and swap edges in these paths
  Set augmenting_edges = paths_edgesB.intersection(paths_edgesD);
  Util::DEBUG_LOG << "augmenting paths: " << augmenting_edges << "\n";
  swapEdgesDirection(augmenting_edges);

  // Swap directions in all graph
  swapEdgesDirection(E);
  direction_ = Direction::kForward;

  // Calculate new matched edges
  M_ = dsbg_.mapD().preImage(right_vertices);

  // Calculate exit conditions
  Set matchedU = dsbg_.mapD().image(M_);
  bool full_match = right_vertices.difference(matchedU).isEmpty();
  bool found_paths = !M_.isEmpty();

  return ExitCondition(full_match, found_paths);
}

bool BFSMatching::ExitCondition::isSatisfied()
{
  return full_match_ || !found_paths_;
}

void BFSMatching::init(const SBG& sbg)
{
  dsbg_ = DSBG(sbg.fact(), sbg.V().compact(), sbg.Vmap().compact(), sbg.map2()
    , sbg.map1().compact(), sbg.Emap().compact(), sbg.subEmap().compact());

  return;
}

MatchData BFSMatching::calculate(const SBG& sbg)
{
  Util::DEBUG_LOG << "Matching sbg: \n" << sbg << "\n\n";

  auto begin = std::chrono::high_resolution_clock::now();
  init(sbg);
  Set right_vertices = dsbg_.mapB().image();

  ExitCondition exit_cond(false, false);
  do {
    exit_cond = step(right_vertices);
  } while (!exit_cond.isSatisfied());
  auto end = std::chrono::high_resolution_clock::now();
  auto total = std::chrono::duration_cast<std::chrono::microseconds>(
    end - begin);
  Util::SBG_LOG << "Total matching exec time: " << total.count() << " [μs]\n";

  MatchData result(sbg, M_.compact(), exit_cond.full_match());
  Util::SBG_LOG << result << "\n\n";

  return result;
}

} // namespace LIB

} // namespace SBG
