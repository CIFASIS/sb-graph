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

PWMap LtEdgesMRV::recursivePaths(const PWMap& rmap
  , const PWMap& decreasing_smap)
{
  PWMap result = PWMAP_FACT.createPWMap();

  // Calculate edges in paths described by _smap
  const PWMap& mapB = _dsbg.mapB();
  const PWMap& mapD = _dsbg.mapD();
  Set ithP = decreasing_smap.composition(mapB).equalImage(mapD);

  // Check if there is a recursion
  PWMap Emap = _dsbg.Emap();
  Set ithSE = Emap.image(ithP);
  Set repeatedSE = _visitedSE.intersection(ithSE);
  if (!repeatedSE.isEmpty()) {
    Set P = _smap.composition(mapB).equalImage(mapD); 
    Set ith_start = _smap.domain().difference(_smap.image());
    Set ithE = mapB.preImage(ith_start).intersection(P);
    if (!ithE.isEmpty()) {
      Set E = SET_FACT.createSet(); 
      bool exit_condition = true;
      do {
        exit_condition = !repeatedSE.intersection(Emap.image(E)).isEmpty();
        ithE = mapB.preImage(ith_start).intersection(P);
        ith_start = mapD.image(ithE);
        E = std::move(E).disjointCup(std::move(ithE));
      } while (!exit_condition);
      // Take out edges that reach a MRV different from that of the recursion
      PWMap ithP_rmap = rmap.composition(mapD.restrict(ithP));
      Set ithP_mrvs = ithP_rmap.image();
      E = E.intersection(ithP_rmap.preImage(ithP_mrvs));

      Set E_plus = Emap.preImage(Emap.image(E));
      // In the presence of a cycle, if the minimum vertex belongs to the
      // recursion, it will be assigned a successor. This results in a cycling
      // smap, which is an error. For example, if there's a cycle
      // 1 -> 2 -> ... -> 10 -> 1, this function calculates smap(1) = 2,
      // when it should be smap(1) = 1. To avoid this, we erase outgoing edges
      // from vertices that already reach the desired MRV. 
      Set outgoing = mapB.preImage(rmap.preImage(ithP_mrvs));
      E_plus = E_plus.difference(outgoing);

      PWMap mapB_plus = _dsbg.mapB().restrict(E_plus);
      PWMap mapD_plus = _dsbg.mapD().restrict(E_plus);
      result = mapB_plus.minAdj(mapD_plus);
    }
    _visitedSE = _visitedSE.difference(Emap.image(P));
  } else {
    _visitedSE = std::move(_visitedSE).disjointCup(std::move(ithSE));
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
    do {
      old_rmap = rmap;

      // Calculate successor map using edges that lead to a minor representative
      E = decreasingRepresentative(rmap);
      PWMap decreasingB = mapB.restrict(E);
      PWMap decreasingD = mapD.restrict(E);
      PWMap decreasing_smap = decreasingB.minAdj(decreasingD); 
      _smap = decreasing_smap.combine(std::move(_smap));

      // Recursive paths
      _smap = recursivePaths(rmap, decreasing_smap).combine(std::move(_smap));

      // Calculate representatives map
      rmap = _smap.mapInf();
      rmap = rmap.min(old_rmap).combine(std::move(rmap));
    } while (!E.isEmpty());
  }

  return rmap;
}

} // namespace LIB

} // namespace SBG
