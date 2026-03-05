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

#include "algorithms/scc/decreasing_edges_mrv.hpp"
#include "sbg/pwmap_fact.hpp"
#include "sbg/set_fact.hpp"
#include "util/logger.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Minimum Adjacent MRV Implementation -----------------------------------------
////////////////////////////////////////////////////////////////////////////////

LtEdgesMRV::LtEdgesMRV() : _dsbg(), _smap(PWMAP_FACT.createPWMap())
  , _visitedSE(SET_FACT.createSet()) {}

Set LtEdgesMRV::decreasingRepresentative(const PWMap& rmap) const
{
  PWMap mapB = _dsbg.mapB();
  PWMap mapD = _dsbg.mapD();

  if (mapB.isEmpty() || mapD.isEmpty()) {
    return SET_FACT.createSet();
  }

  PWMap rmapB = rmap.composition(mapB);
  PWMap rmapD = rmap.composition(mapD);

  Set result = rmapD.lessImage(rmapB);
  return result;
}

Set LtEdgesMRV::edgesInPaths(const PWMap& smap) const
{
  PWMap mapB = _dsbg.mapB();
  PWMap mapD = _dsbg.mapD();

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

PWMap LtEdgesMRV::recursivePaths(const Set& ith_paths_edges, const Set& outgoing)
{
  PWMap result = PWMAP_FACT.createPWMap();

  PWMap Emap = _dsbg.Emap();
  Set ithSE = Emap.image(ith_paths_edges);
  _visitedSE = std::move(_visitedSE).cup(ithSE);
  Set repeatedSE = _visitedSE.intersection(ithSE);
  if (!repeatedSE.isEmpty()) {
    PWMap mapB = _dsbg.mapB();
    PWMap mapD = _dsbg.mapD();

    Set ith_start = _smap.domain().difference(_smap.image());
    Set E = SET_FACT.createSet(); 
    Set ithE = mapB.preImage(ith_start).intersection(ith_paths_edges);
    if (!ithE.isEmpty()) {
      bool exit_condition = true;
      do {
        ithE = mapB.preImage(ith_start).intersection(ith_paths_edges);
        E = std::move(E).disjointCup(std::move(ithE));
        ith_start = mapD.image(ithE);
        exit_condition = !repeatedSE.intersection(Emap.image(E)).isEmpty();
      } while (!exit_condition);
    }

    Set smap_edges = edgesInPaths(_smap);
    Set adj = mapB.preImage(mapB.image(smap_edges));

    Set E_plus = Emap.preImage(Emap.image(E));
    E_plus = E_plus.difference(mapB.preImage(outgoing));
    E_plus = E_plus.difference(adj);
    PWMap mapB_plus = _dsbg.mapB().restrict(E_plus);
    PWMap mapD_plus = _dsbg.mapD().restrict(E_plus);
    result = mapB_plus.minAdj(mapD_plus);
  }

  return result;
}


PWMap LtEdgesMRV::calculate(const DirectedSBG& dsbg)
{
  Util::DEBUG_LOG << "LtEdgesMRV dsbg:\n" << dsbg << "\n\n";

  _dsbg = dsbg;
  PWMap mapB = _dsbg.mapB();
  PWMap mapD = _dsbg.mapD();
  _visitedSE = SET_FACT.createSet();

  _smap = PWMAP_FACT.createPWMap(dsbg.V());
  PWMap rmap = _smap;

  if (!_dsbg.V().isEmpty() && !_dsbg.E().isEmpty()) {
    PWMap old_rmap = PWMAP_FACT.createPWMap();
    Set E = SET_FACT.createSet();
    Set paths_edges = SET_FACT.createSet();
    do {
      old_rmap = rmap;

      // Calculate successor map using edges that lead to a minor representative
      E = decreasingRepresentative(rmap);
      PWMap decreasingB = mapB.restrict(E);
      PWMap decreasingD = mapD.restrict(E);
      PWMap decreasing_smap = decreasingB.minAdj(decreasingD); 
      _smap = decreasing_smap.combine(std::move(_smap));

      // Recursive paths
      Set ith_paths_edges = edgesInPaths(decreasing_smap);
      Set outgoing = rmap.image(mapD.image(ith_paths_edges));
      PWMap smap_plus = recursivePaths(ith_paths_edges, outgoing);
      _smap = std::move(smap_plus).combine(std::move(_smap));

      // Calculate representatives map
      rmap = _smap.mapInf();
      rmap = rmap.min(old_rmap).combine(std::move(rmap));
    } while (!E.isEmpty());
  }

  return rmap;
}

} // namespace LIB

} // namespace SBG
