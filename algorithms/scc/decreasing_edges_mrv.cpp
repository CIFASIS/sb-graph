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

#include <sbgraph/algorithms/scc/decreasing_edges_mrv.hpp>
#include <sbgraph/util/logger.hpp>

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Minimum Adjacent MRV Implementation -----------------------------------------
////////////////////////////////////////////////////////////////////////////////

LtEdgesMRV::LtEdgesMRV() : _n(0) {}

Set LtEdgesMRV::decreasingRepresentative(const PWMap& rmap) const
{
  PWMap mapB = _dsbg.mapB();
  PWMap mapD = _dsbg.mapD();

  if (mapB.isEmpty() || mapD.isEmpty()) {
    return Set{};
  }

  PWMap rmapB = rmap.composition(mapB);
  PWMap rmapD = rmap.composition(mapD);

  Set result = rmapD.lessImage(rmapB);
  return result;
}

PWMap LtEdgesMRV::detectRepetition(const PWMap& decreasing_smap) const
{
  // Get repetitive paths.
  Set Vj = decreasing_smap.domain().difference(decreasing_smap.image());
  Set repetitive_vertices = Vj; ///< Vertices traversed by the repetition.
  for (unsigned int j = 0; j < _n; ++j) {
    Vj = _smap.image(Vj);
    repetitive_vertices = repetitive_vertices.disjointCup(repetitive_vertices);
  }
  
  return decreasing_smap.combine(_smap).restrict(repetitive_vertices);
}

PWMap LtEdgesMRV::repetition(const PWMap& decreasing_smap) const
{
  // Calculate edges of the repetition.
  PWMap smap_repetition = detectRepetition(decreasing_smap);
  PWMap mapB = _dsbg.mapB();
  PWMap mapD = _dsbg.mapD();
  Set E_repetition = smap_repetition.composition(mapB).equalImage(mapD);

  // Extend edges of the repetition.
  PWMap Emap = _dsbg.Emap();
  Set E_plus = Emap.preImage(Emap.image(E_repetition));

  // Delete cycle generating edges.
  Set decreasing_reps = _rmap.image(decreasing_smap.image());
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

  return smap_plus;
}


PWMap LtEdgesMRV::calculate(const DirectedSBG& dsbg)
{
  Util::DEBUG_LOG << "LtEdgesMRV dsbg:\n" << dsbg << "\n\n";

  _dsbg = dsbg;
  if (dsbg.V().isEmpty() || dsbg.E().isEmpty()) {
    return PWMap{dsbg.V()};
  }

  PWMap mapB = _dsbg.mapB();
  PWMap mapD = _dsbg.mapD();
  PWMap Emap = _dsbg.Emap();

  _smap = PWMap{dsbg.V()};
  _rmap = PWMap{dsbg.V()};

  _n = 0;
  Set E;
  Set visitedSE;
  do {
    _old_rmap = _rmap;

    // Calculate successor map using edges that lead to a minor representative.
    E = decreasingRepresentative(_rmap);
    PWMap decreasingB = mapB.restrict(E);
    PWMap decreasingD = mapD.restrict(E);
    PWMap decreasing_smap = decreasingB.minAdj(decreasingD);

    // Handle repetition.
    PWMap smap_plus;
    Set Pj = decreasing_smap.composition(mapB).equalImage(mapD);
    Set repeatedSE = visitedSE.intersection(Emap.image(Pj));
    if (!repeatedSE.isEmpty()) {
      smap_plus = repetition(decreasing_smap);
      //visitedSE = visitedSE.difference(Emap.image(E_repetition));
      _n = 0;
    } else {
      visitedSE = std::move(visitedSE).disjointCup(Emap.image(Pj));
      ++_n;
    }
    _smap = decreasing_smap.combine(smap_plus).combine(std::move(_smap));

    // Calculate new MRV with the proposed paths.
    _rmap = _smap.mapInf();
    _rmap = _rmap.min(_old_rmap).combine(std::move(_rmap));
    _rmap.compact();
  } while (!E.isEmpty());

  return _rmap;
}

} // namespace LIB

} // namespace SBG
