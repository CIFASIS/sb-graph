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

#include <sbgraph/detail/algorithms/scc/decreasing_rmap_mrv.hpp>
#include "util/logger.hpp"
#include <iostream>

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Minimum Adjacent MRV Implementation -----------------------------------------
////////////////////////////////////////////////////////////////////////////////

LtRmapMRV::LtRmapMRV() {}

Set LtRmapMRV::decreasingRepresentative()
{
  PWMap mapB = _dsbg.mapB();
  PWMap mapD = _dsbg.mapD();

  if (mapB.isEmpty() || mapD.isEmpty()) {
    return Set{};
  }

  PWMap rmapB = _rmap.composition(mapB);
  PWMap rmapD = _rmap.composition(mapD);
  Set decreasing_edges = rmapD.lessImage(rmapB);
  _decreasing = _decreasing.cup(decreasing_edges);

  rmapB = rmapB.restrict(decreasing_edges);
  rmapD = rmapD.restrict(decreasing_edges);
  PWMap decreasing_rmap = rmapB.minAdj(rmapD);
  _rmap = decreasing_rmap.combine(std::move(_rmap));

  return decreasing_edges;
}

PWMap LtRmapMRV::repetition(const Set& E) 
{
  // Extend edges of the repetition.
  PWMap Emap = _dsbg.Emap();
  Set E_plus = Emap.preImage(Emap.image(_decreasing));

  // Delete cycle generating edges.
  PWMap mapD = _dsbg.mapD();
  Set decreasing_reps = _rmap.image(mapD.image(E));
  PWMap mapB = _dsbg.mapB();
  E_plus = E_plus.difference(mapB.preImage(decreasing_reps));

  // Calculate a guess path using edges in the repetition.
  PWMap mapB_plus = mapB.restrict(E_plus);
  PWMap mapD_plus = mapD.restrict(E_plus);
  PWMap smap_plus = mapB_plus.minAdj(mapD_plus);

  // Avoid using repetitive paths for vertices that found a minimum through
  // the main iteration of the algorithm.
  Set decreasing_repd = _rmap.preImage(decreasing_reps);
  smap_plus = smap_plus.restrict(
    smap_plus.domain().difference(decreasing_repd)
  );

  Set edges = smap_plus.composition(mapB).equalImage(mapD);
  _decreasing = _decreasing.difference(edges);

  return smap_plus;
}


PWMap LtRmapMRV::calculate(const DirectedSBG& dsbg)
{
  Util::DEBUG_LOG << "LtRmapMRV dsbg:\n" << dsbg << "\n\n";

  _dsbg = dsbg;
  if (dsbg.V().isEmpty() || dsbg.E().isEmpty()) {
    return PWMap{dsbg.V()};
  }

  PWMap mapB = _dsbg.mapB();
  PWMap mapD = _dsbg.mapD();
  PWMap Emap = _dsbg.Emap();

  _rmap = PWMap{dsbg.V()};

  Set E;
  Set visitedSE;
  do {
    _old_rmap = _rmap;

    // Edges that lead to a new MRV.
    E = decreasingRepresentative();

    PWMap smap_plus;
    Set repeatedSE = visitedSE.intersection(Emap.image(E));
    if (!repeatedSE.isEmpty()) { // Handle repetition.
      smap_plus = repetition(E);
    } else {
      visitedSE = std::move(visitedSE).disjointCup(Emap.image(E));
    }

    if (!smap_plus.isEmpty()) {
      PWMap smap = smap_plus.combine(_rmap);
      PWMap guess_rmap = smap.mapInf();
      _rmap = guess_rmap.min(_old_rmap).combine(std::move(_rmap));
    } else { // Calculate new MRV with the proposed paths.
      PWMap reps_rmap = _rmap.restrict(_rmap.image()).mapInf();
      _rmap = reps_rmap.combine(std::move(_rmap));
    }
    _rmap.compact();
    Util::DEBUG_LOG << "LtRmapMRV rmap:\n" << _rmap << "\n\n";
  } while (!E.isEmpty());

  return _rmap.mapInf();
}

} // namespace LIB

} // namespace SBG
