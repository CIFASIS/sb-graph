/** @file minadj_mrv.hpp

 @brief <b>Concrete SBG MRV Algorithm</b>

 @see scc.hpp and scc.cpp files to comprehend its purpose.

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

#ifndef SBGRAPH_ALGORITHMS_SCC_MINADJ_MRV_HPP_
#define SBGRAPH_ALGORITHMS_SCC_MINADJ_MRV_HPP_

#include "algorithms/scc/mrv.hpp"
#include "sbg/directed_sbg.hpp"
#include "sbg/pw_map.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// MRV Algorithm ---------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Minimum Adjacent implementation to calculate MRV.
 */
class MinAdjMRV : public MRVContext<MinAdjMRV> {
public:
  MinAdjMRV();
 
  /**
   * @brief Concrete implementation that starts with the identity pw for every
   * vertex. Then, for every vertex it compares the current MRV with the MRVs
   * of their adjacent reachable vertices.
   * It also handles repetitive paths (i.e. paths that have a length depending
   * on the size of the intervals that define the DSBG).
   */
  PWMap calculate(const DirectedSBG& dsbg);
};

} // namespace LIB

} // namespace SBG

#endif // SBGRAPH_ALGORITHMS_SCC_MINADJ_MRV_HPP_
