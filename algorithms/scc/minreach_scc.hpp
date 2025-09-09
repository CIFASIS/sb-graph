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

#ifndef SBG_MINREACH_SCC_HPP
#define SBG_MINREACH_SCC_HPP

#include "algorithms/scc/scc.hpp"

namespace SBG {

namespace LIB {

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
class MinReachSCC : public SCCStrategy {
  public:
  MinReachSCC(const PWMapAF& fact);

  SCCData calculate(const DSBG& dsbg) override;

  private:
  /**
   * @brief Initializes data members determined by the input SBG.
   */
  void init(const DSBG& dsbg);

  /**
   * @brief Performs a step of the algorithm in a certain direction, detecting
   * and erasing edges that belong to different SCC.
   */
  PWMap sccStep();

  /**
   * @brief Modifies the `dsbg_` member, restricting the domain of edges maps
   * to `E`, and swaps mapB and mapD for elements also in `E`.
   */
  void swapEdgesDirection(const Set& E);

  /**
   * @brief Calculates the MRV for every vertex in DSBG.
   */
  PWMap sccMinReach(const DSBG& dsbg) const;

  DSBG dsbg_; ///< Input DSBG
  Set E_;     ///< Edges with both endings in the same SCC
};

} // namespace LIB

} // namespace SBG

#endif
