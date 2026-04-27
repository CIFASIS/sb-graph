/** @file minreach_scc.hpp

 @brief <b>SBG Tearing Algorithm implementation</b>

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

#ifndef SBG_MINREACH_TEARING_HPP
#define SBG_MINREACH_TEARING_HPP

#include "algorithms/tearing/tearing.hpp"
#include "algorithms/scc/scc.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Tearing Algorithm Implementation (concrete strategy) ----------
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Minimum Reachable SCC implementation. In each step it identifies edges
 * with different MRV (Minimum Reachable Vertex) in their endings, that are
 * edges connecting different SCC. Then, it swaps the direction of edges, and
 * perform the same calculations. This is repeated all edges between different
 * SCC are deleted. When this process ends each SCC is identified by its
 * minimum vertex.
 */

typedef std::unique_ptr<SCC> SCCPtr;

class TearingV1 : public TearingStrategy {
  public:
  TearingV1(SCCPtr sccAlgorithm);

  TearingData calculate(const DSBG& dsbg) override;

  protected:
  /**
   * @brief Initializes data members determined by the input SBG.
   */
  void init(const DSBG& dsbg);

  SCCPtr sccAlgorithm_;
  DSBG dsbg_; ///< Input DSBG
  DSBG finalDSBG; ///< Output DSBG
  PWMap tearIOMap_; ///< Output tearing vertex map
  PWMap finalMapB; ///< Output DSBG mapB
  PWMap finalMapD; ///< Output DSBG mapD
};

} // namespace LIB

} // namespace SBG

#endif
