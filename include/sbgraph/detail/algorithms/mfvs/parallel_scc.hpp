/** @file parallel_scc.hpp

 @brief <b>Concrete SBG Parallel SCC MFVS Algorithm implementation</b>

 In each step calculates a feedback vertex set with at least one element for
 each SCC.

 <hr>

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

#ifndef SBGRAPH_ALGORITHMS_MFVS_PARALLEL_SCC_HPP_
#define SBGRAPH_ALGORITHMS_MFVS_PARALLEL_SCC_HPP_

#include <sbgraph/sbg/directed_sbg.hpp>
#include <sbgraph/sbg/pw_map.hpp>
#include <sbgraph/sbg/set.hpp>

namespace SBG {

namespace LIB {

namespace detail {

///////////////////////////////////////////////////////////////////////////////
// Maximum Degree SCC MFVS Algorithm Implementation ---------------------------
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief In each step takes out a vertex from each remaining SCC, according to
 * the criteria defined by heuristic.
 */
class ParallelSCCMFVS {
public:
  ParallelSCCMFVS() = default;

  Set calculate(const DirectedSBG& input_dsbg);

private:
  /**
   * @brief Calculates the SCCs of the graph, and deletes edges between
   * different SCCs, and also deletes singleton SCCs.
   */
  void reduceGraph();

  Set smallestSV();

  /**
   * @brief Returns a collection of vertices that have maximum degree in their
   * respective SCCs.
   */
  Set maximumDegree(const Set& V);

  PWMap heuristic();

  DirectedSBG _dsbg;
  Set _Ediff;
  PWMap _rmap;
};

} // namespace detail

} // namespace LIB

} // namespace SBG

#endif // SBGRAPH_ALGORITHMS_MFVS_PARALLEL_SCC_HPP_
