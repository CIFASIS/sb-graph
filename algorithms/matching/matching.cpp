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

#include "algorithms/matching/matching.hpp"
#include "algorithms/matching/paths.hpp"
#include "util/logger.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Auxiliary structures --------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

MatchData::MatchData(SBG sbg, Set M, bool full_match)
  : sbg_(sbg), M_(M), full_match_(full_match) {}

member_imp(MatchData, SBG, sbg);
member_imp(MatchData, Set, M);
member_imp(MatchData, bool, full_match);

std::ostream &operator<<(std::ostream &out, const MatchData &data)
{
  out << data.M();
  if (data.full_match())
    out << " [FULLY MATCHED]";
  else
    out << " [UNMATCHED]";

  return out;
}

////////////////////////////////////////////////////////////////////////////////
// Matching Algorithm Delegate Constructors ------------------------------------
////////////////////////////////////////////////////////////////////////////////

MatchDelegate::MatchDelegate(const PWMapAF &fact) : fact_(std::move(fact)) {}

////////////////////////////////////////////////////////////////////////////////
// BFS Matching Algorithm ------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

BFSMatching::ExitCondition::ExitCondition(bool full_match, bool found_paths)
  : full_match_(full_match), found_paths_(found_paths) {}

bool BFSMatching::ExitCondition::full_match() { return full_match_; }

bool BFSMatching::ExitCondition::found_paths() { return found_paths_; }

DSBG createEmptyDSBG(const PWMapAF &fact)
{
  Set empty_set = fact.createSet();
  PWMap empty_pw = fact.createPWMap();
  return DSBG(fact, empty_set, empty_pw, empty_pw, empty_pw, empty_pw, empty_pw);
}

BFSMatching::BFSMatching(const PWMapAF &fact)
  : MatchDelegate(fact), M_(fact.createSet()), dsbg_(createEmptyDSBG(fact))
    , right_vertices_(fact.createSet()), direction_(Direction::kForward) {}

void BFSMatching::swapEdgesDirection(const Set &E)
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
  for (const Map &subset_edge : dsbg_.subEmap()) {
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

Set BFSMatching::edgesInPaths(const PWMap &smap, const Set &E) const
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

Set BFSMatching::directedStep(const Set &E)
{
  PWMap mapB = dsbg_.mapB().restrict(E).compact();
  PWMap mapD = dsbg_.mapD().restrict(E).compact();
  PWMap Emap = dsbg_.Emap().restrict(E).compact();
  PWMap subEmap = partitionSubsetEdges().restrict(E).compact();

  Set forward_vertices = dsbg_.V().difference(right_vertices_);
  Set matched_forward_vertices = mapB.image(M_);
  if (direction_ == Direction::kBackward) {
    forward_vertices = right_vertices_;
  }
  Set unmatched_forward_vertices
    = forward_vertices.difference(matched_forward_vertices);

  BFSPaths paths(fact_);
  DSBG restricted_dsbg(fact_, dsbg_.V(), dsbg_.Vmap(), mapB, mapD
    , Emap, subEmap);
  PWMap smap = paths.calculate(restricted_dsbg, unmatched_forward_vertices);

  Set paths_edges = edgesInPaths(smap, E);
  PWMap rmap = smap.mapInf();
  Set reach_unmatched = rmap.preImage(unmatched_forward_vertices);
  paths_edges = paths_edges.intersection(mapD.preImage(reach_unmatched));

  Util::DEBUG_LOG << "paths_edges: " << paths_edges << "\n";

  return paths_edges;
}

BFSMatching::ExitCondition BFSMatching::step()
{
  Set E = dsbg_.E();

  Set paths_edgesD = directedStep(E);

  swapEdgesDirection(E);
  direction_ = Direction::kBackward;
  Set paths_edgesB = directedStep(paths_edgesD);

  Set paths_edges = paths_edgesB.intersection(paths_edgesD);
  Util::DEBUG_LOG << "paths_edges: " << paths_edges << "\n";

  // Swap direction in edges in augmenting paths
  swapEdgesDirection(paths_edges);

  // Swap directions
  swapEdgesDirection(E);
  direction_ = Direction::kForward;
  M_ = dsbg_.mapD().preImage(right_vertices_);

  // Calculate exit conditions
  Set matchedU = dsbg_.mapD().image(M_);
  bool full_match = right_vertices_.difference(matchedU).isEmpty();
  bool found_paths = !M_.isEmpty();

  return ExitCondition(full_match, found_paths);
}

bool BFSMatching::ExitCondition::isSatisfied()
{
  return full_match_ || !found_paths_;
}

DSBG initDSBG(const SBG &sbg)
{
  return DSBG(sbg.fact(), sbg.V().compact(), sbg.Vmap().compact(), sbg.map2()
    , sbg.map1().compact(), sbg.Emap().compact(), sbg.subEmap().compact());
}

MatchData BFSMatching::calculate(const SBG &sbg)
{
  Util::DEBUG_LOG << "Matching sbg: \n" << sbg << "\n\n";

  auto begin = std::chrono::high_resolution_clock::now();
  dsbg_ = initDSBG(sbg);
  right_vertices_ = dsbg_.mapB().image();

  ExitCondition exit_cond(false, false);
  do {
    exit_cond = step();
  } while (!exit_cond.isSatisfied());
  auto end = std::chrono::high_resolution_clock::now();
  auto total = std::chrono::duration_cast<std::chrono::microseconds>(
    end - begin);
  Util::SBG_LOG << "Total matching exec time: " << total.count() << " [μs]\n";

  MatchData result(sbg, M_.compact(), exit_cond.full_match());
  Util::SBG_LOG << result << "\n\n";

  return result;
}

////////////////////////////////////////////////////////////////////////////////
// Matching Algorithm Implementation ------------------------------------------- 
////////////////////////////////////////////////////////////////////////////////

Matching::Matching(MatchDelegPtr deleg) : delegate_(std::move(deleg)) {}

MatchData Matching::calculate(const SBG &sbg)
{
  return delegate_->calculate(sbg);
}

} // namespace LIB

} // namespace SBG
