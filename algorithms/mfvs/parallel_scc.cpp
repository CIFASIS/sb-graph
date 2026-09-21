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

#include <sbgraph/algorithms/mfvs/parallel_scc.hpp>
#include <sbgraph/algorithms/scc/scc.hpp>
#include <sbgraph/algorithms/scc/scc_data.hpp>
#include <sbgraph/sbg/integer.hpp>
#include <sbgraph/util/logger.hpp>

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Maximum Degree MFVS Algorithm -----------------------------------------------
////////////////////////////////////////////////////////////////////////////////

void ParallelSCCMFVS::reduceGraph()
{
  // Resulting SCC from induced graph.
  SCCData scc_data = SCC{}.calculate(_dsbg);
  _Ediff = scc_data.Ediff();
  _rmap = scc_data.rmap();

  // Erase edges connecting different SCC.
  _dsbg.eraseEdges(_Ediff);

  // Erase vertices that belong to a singleton SCC.
  PWMap mmap = _rmap.imageMultiplicity();
  Set one{SBG::LIB::IntTuple{_dsbg.V().arity(), 1}};
  _dsbg.eraseVertices(mmap.preImage(one));
}

Set ParallelSCCMFVS::smallestSV()
{
  // Size of set-vertices.
  PWMap Vmap = _dsbg.Vmap();
  PWMap dmap = _dsbg.Vmap().imageMultiplicity();

  // Map from vertices to the size of their respective set-vertex.
  PWMap vertices_to_size = dmap.composition(Vmap);

  // Map from representatives to minimum size of set-vertex for each SCC.
  PWMap reps_to_min_size = _rmap.minAdj(vertices_to_size);

  // Map from vertices to minimum size of set-vertex of their respective SCC.
  PWMap vertices_to_min_size = reps_to_min_size.composition(_rmap);

  // Vertices in smallest set-vertices of each SCC.
  Set min_size_vs = vertices_to_size.equalImage(vertices_to_min_size);

  return min_size_vs;
}

Set ParallelSCCMFVS::maximumDegree(const Set& V)
{
  // Ingoing/outgoing degree for all vertices of _dsbg.
  PWMap mapB = _dsbg.mapB();
  PWMap mapB_V = mapB.restrict(mapB.preImage(V));
  PWMap dmap = mapB_V.imageMultiplicity();

  // Map from representatives to maximum degree of each SCC.
  PWMap reps_to_max_degree = _rmap.maxAdj(dmap);

  // Map from vertices, to maximum degree of their SCC.
  PWMap vertices_to_max_degree = reps_to_max_degree.composition(_rmap);

  // Vertices of maximum degree in each SCC.
  Set max_deg_vs = dmap.equalImage(vertices_to_max_degree);

  return max_deg_vs;
}

PWMap ParallelSCCMFVS::heuristic()
{
  Set to_delete = maximumDegree(smallestSV());
  return PWMap{_dsbg.V()}.restrict(to_delete);
}

Set ParallelSCCMFVS::calculate(const DirectedSBG& input_dsbg)
{
  if (input_dsbg.V().isEmpty()) {
    return Set{};
  }

  _dsbg = input_dsbg;
  reduceGraph();

  Util::DEBUG_LOG << "initial max deg scc mfvs dsbg:\n" << _dsbg << "\n";

  Set visitedSV;
  Set fvs_result;
  while (_rmap.fixedPoints() != _rmap.domain()) {
    // Get minimum vertex with maximum degree for each SCC.
    Set Vj = _rmap.minAdj(heuristic()).image();
    fvs_result = std::move(fvs_result).disjointCup(Vj);

    // Handle repetition.
    PWMap Vmap = _dsbg.Vmap();
    Set Vj_SV = Vmap.image(Vj);
    Set repeatedSV = visitedSV.intersection(Vj_SV);
    if (!repeatedSV.isEmpty()) {
      fvs_result = fvs_result.cup(Vmap.preImage(Vj_SV)); 
    } else {
      visitedSV = visitedSV.cup(Vj_SV);
    }

    // Erase calculated vertices.
    _dsbg.eraseVertices(fvs_result);

    // Calculate SCCs and simplify graph for next iteration.
    reduceGraph();

    Util::DEBUG_LOG << "Vj: " << Vj << "\n";
    Util::DEBUG_LOG << "new rmap: " << _rmap << "\n\n";
  }

  fvs_result.compact();
  return fvs_result;
}

} // namespace detail

} // namespace LIB

} // namespace SBG
