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
#include "algorithms/scc/scc_fact.hpp"
#include "sbg/natural.hpp"
#include "sbg/pw_map.hpp"
#include "sbg/set_fact.hpp"
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
  PWMap multB = dsbg.mapB().imageMultiplicity();
  PWMap multD = dsbg.mapD().imageMultiplicity();
  PWMap mmap = multB + multD;

  MD_NAT max_mult{dsbg.V().arity(), 0};
  Set remaining = mmap.image();
  while (!remaining.isEmpty()) {
    MD_NAT jth_mult = remaining.minElem();
    NAT current_degree = std::accumulate(max_mult.begin(), max_mult.end(), 0);
    NAT jth_degree = std::accumulate(jth_mult.begin(), jth_mult.end(), 0);
    if (jth_degree > current_degree) {
      max_mult = jth_mult;
    }

    remaining = remaining.difference(SET_FACT.createSet(jth_mult));
  }

  Set max_mult_set = SET_FACT.createSet(max_mult);
  Set max_degree_vertices = mmap.preImage(max_mult_set);

  return max_degree_vertices.minElem();
}

Set GreedyMFVS::calculate(const DirectedSBG& input_dsbg) const
{
  DirectedSBG dsbg = input_dsbg;

  Util::DEBUG_LOG << "initial mfvs dsbg:\n" << dsbg << "\n";

  PWMap rmap = SCC_FACT.createSCCAlgorithm().calculate(dsbg).rmap();
  Set fvs_result = SET_FACT.createSet();
  Set visitedSV = SET_FACT.createSet();
  while (rmap.fixedPoints() != rmap.domain()) {
    // Get minimum vertex with maximum degree
    MD_NAT max_degree_vertex = maxDegreeVertex(dsbg);
    Set Vj = SET_FACT.createSet(max_degree_vertex);
    fvs_result = std::move(fvs_result).disjointCup(Vj);

    // Handle repetition
    PWMap Vmap = dsbg.Vmap();
    Set repeatedSV = visitedSV.intersection(Vmap.image(Vj));
    if (!repeatedSV.isEmpty()) {
      Set V_plus = Vmap.preImage(Vmap.image(Vj));
      fvs_result = std::move(fvs_result).cup(std::move(V_plus));

      visitedSV = repeatedSV.difference(Vmap.image(Vj));
    } else {
      visitedSV = std::move(repeatedSV).disjointCup(Vmap.image(Vj));
    }

    // Erase selected vertices
    dsbg.eraseVertices(fvs_result);

    // Resulting SCC from induced graph
    rmap = SCC_FACT.createSCCAlgorithm().calculate(dsbg).rmap();

    Util::DEBUG_LOG << "Vj: " << Vj << "\n";
    Util::DEBUG_LOG << "new rmap: " << rmap << "\n\n";
  }

  return fvs_result;
}

} // namespace detail

} // namespace LIB

} // namespace SBG
