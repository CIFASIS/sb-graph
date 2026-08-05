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

#include "algorithms/mfvs/greedy_mfvs.hpp"
#include "algorithms/scc/scc.hpp"
#include "sbg/natural.hpp"
#include "sbg/pw_map.hpp"
#include "util/logger.hpp"

#include <numeric>

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Greedy MFVS Algorithm -------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

GreedyMFVS::GreedyMFVS() {}

/**
 * @brief It calculates the minimum vertex with maximum degree. 
 */
MD_NAT maxDegreeVertex(const DirectedSBG& dsbg)
{
  Set V = dsbg.V();
  PWMap mapB = dsbg.mapB();
  PWMap mapD = dsbg.mapD();

  Set adj_edges = mapB.preImage(V).cup(mapD.preImage(V));
  PWMap multB = mapB.restrict(adj_edges).imageMultiplicity();
  PWMap multD = mapD.restrict(adj_edges).imageMultiplicity();
  PWMap mmap = (multB + multD).restrict(V);

  if (mmap.isEmpty()) {
    return V.minElem();
  }

  Set max_mult_set{mmap.image().maxElem()};
  Set max_degree_vertices = mmap.preImage(max_mult_set);

  return max_degree_vertices.minElem();
}

Set GreedyMFVS::calculate(const DirectedSBG& input_dsbg) const
{
  if (input_dsbg.V().isEmpty()) {
    return Set{};
  }

  DirectedSBG dsbg = input_dsbg;

  Util::DEBUG_LOG << "initial mfvs dsbg:\n" << dsbg << "\n";

  PWMap rmap = SCC{}.calculate(dsbg).rmap();
  Set fvs_result;
  Set visitedSV;
  while (rmap.fixedPoints() != rmap.domain()) {
    // Get minimum vertex with maximum degree
    MD_NAT max_degree_vertex = maxDegreeVertex(dsbg);
    Set Vj{max_degree_vertex};
    fvs_result = std::move(fvs_result).disjointCup(Vj);

    // Handle repetition
    PWMap Vmap = dsbg.Vmap();
    Set repeatedSV = visitedSV.intersection(Vmap.image(Vj));
    if (!repeatedSV.isEmpty()) {
      Set V_plus = Vmap.preImage(Vmap.image(Vj));
      fvs_result = std::move(fvs_result).cup(std::move(V_plus));
    } else {
      visitedSV = std::move(repeatedSV).disjointCup(Vmap.image(Vj));
    }

    // Erase selected vertices
    dsbg.eraseVertices(fvs_result);

    // Resulting SCC from induced graph
    rmap = SCC{}.calculate(dsbg).rmap();

    Util::DEBUG_LOG << "Vj: " << Vj << "\n";
    Util::DEBUG_LOG << "new rmap: " << rmap << "\n\n";
  }

  fvs_result.compact();
  return fvs_result;
}

} // namespace detail

} // namespace LIB

} // namespace SBG
