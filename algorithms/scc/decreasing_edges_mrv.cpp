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

#include <iostream>

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Minimum Adjacent MRV Implementation -----------------------------------------
////////////////////////////////////////////////////////////////////////////////

LtEdgesMRV::LtEdgesMRV() : _dsbg(), _smap(PWMAP_FACT.createPWMap())
  , _visitedSE(SET_FACT.createSet()), _n(0) {}

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

PWMap LtEdgesMRV::repetitivePaths(const PWMap& rmap
  , const PWMap& decreasing_smap)
{
  PWMap result = PWMAP_FACT.createPWMap();

  // Calculate edges in paths described by decreasing_smap
  const PWMap& mapB = _dsbg.mapB();
  const PWMap& mapD = _dsbg.mapD();
  Set Pj = decreasing_smap.composition(mapB).equalImage(mapD);

  // Check if there is a repetition
  PWMap Emap = _dsbg.Emap();
  Set repeatedSE = _visitedSE.intersection(Emap.image(Pj));
  if (!repeatedSE.isEmpty()) {
    Set Vi = decreasing_smap.domain().difference(decreasing_smap.image());
    Set V = Vi;
    for (unsigned int j = 0; j < _n; ++j) {
      Vi = _smap.image(Vi);
      V = V.disjointCup(Vi);
    }
    PWMap smap_rep = _smap.restrict(V);
    Set E_repetition = smap_rep.composition(mapB).equalImage(mapD);
    
    Set E_plus = Emap.preImage(Emap.image(E_repetition));
    // In the presence of a cycle, if the minimum vertex belongs to the
    // repetition, it will be assigned a successor. This results in a cycling
    // smap, which is an error. For example, if there's a cycle
    // 1 -> 2 -> ... -> 10 -> 1, this function calculates smap(1) = 2,
    // when it should be smap(1) = 1. To avoid this, we erase outgoing edges
    // from the MRVs of the repetition.
    Set outgoing = mapB.preImage(rmap.image(V));
    E_plus = E_plus.difference(outgoing);

    PWMap mapB_plus = _dsbg.mapB().restrict(E_plus);
    PWMap mapD_plus = _dsbg.mapD().restrict(E_plus);
    result = mapB_plus.minAdj(mapD_plus);

    _visitedSE = _visitedSE.difference(Emap.image(E_repetition));
    _n = 0;
  } else {
    _visitedSE = std::move(_visitedSE).disjointCup(std::move(Emap.image(Pj)));
    ++_n;
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
    _n = 0;
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

      // Repetitive paths
      _smap = repetitivePaths(rmap, decreasing_smap).combine(std::move(_smap));

      // Calculate representatives map
      rmap = _smap.mapInf();
      rmap = rmap.min(old_rmap).combine(std::move(rmap));
      rmap.compact();
    } while (!E.isEmpty());
  }

  return rmap;
}

} // namespace LIB

} // namespace SBG
