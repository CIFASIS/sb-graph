/** @file minreach_scc.hpp

 @brief <b>SBG Minimum Adjacent SCC Algorithm implementation</b>

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

#ifndef SBGRAPH_ALGORITHMS_SCC_MINREACH_SCC_HPP_
#define SBGRAPH_ALGORITHMS_SCC_MINREACH_SCC_HPP_

#include "algorithms/scc/scc_data.hpp"
#include "sbg/directed_sbg.hpp"
#include "sbg/pw_map.hpp"
#include "sbg/set.hpp"

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Minimum Reachable SCC Algorithm Implementation (concrete strategy) ----------
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Minimum Reachable SCC implementation. In each step it identifies edges
 * with different MRV (Minimum Reachable Vertex) in their endings, that are
 * edges connecting different SCC. Then, it swaps the direction of edges, and
 * perform the same calculations. This is repeated all edges between different
 * SCC are deleted. When this process ends each SCC is identified by its
 * minimum vertex.
 */
class MinReachSCC {
public:
  MinReachSCC();

  SCCData calculate(const DirectedSBG& dsbg);

protected:
  /**
   * @brief Initializes data members determined by the input SBG.
   */
  void init(const DirectedSBG& dsbg);

  /**
   * @brief Performs a step of the algorithm in a certain direction, detecting
   * and erasing edges that belong to different SCC.
   */
  virtual PWMap sccStep() = 0;

  /**
   * @brief Modifies the `_dsbg` member, restricting the domain of edges maps
   * to `E`, and swaps mapB and mapD for elements also in `E`.
   */
  void swapEdgesDirection(const Set& E);

  /**
   * @brief Calculates the MRV for every vertex in DSBG.
   */
  PWMap sccMinReach(const DirectedSBG& dsbg) const;

  DirectedSBG _dsbg; ///< Input DSBG
  Set _E; ///< Edges with both endings in the same SCC
};

////////////////////////////////////////////////////////////////////////////////
// First version with mininimum adjacent MRV ----------------------------------- 
////////////////////////////////////////////////////////////////////////////////

class MinReachSCCV1 : public MinReachSCC {
public:
  MinReachSCCV1();

protected:
  PWMap sccStep();
};

////////////////////////////////////////////////////////////////////////////////
// Second version with decreasing edges MRV ------------------------------------ 
////////////////////////////////////////////////////////////////////////////////

class MinReachSCCV2 : public MinReachSCC {
public:
  MinReachSCCV2();

protected:
  PWMap sccStep();
};

} // namespace detail

} // namespace LIB

} // namespace SBG

#endif // SBGRAPH_ALGORITHMS_SCC_MINREACH_SCC_HPP_
