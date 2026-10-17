/** @file decreasing_edges_mrv.hpp

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

#ifndef SBGRAPH_ALGORITHMS_SCC_DECREASING_RMAP_MRV_HPP_
#define SBGRAPH_ALGORITHMS_SCC_DECREASING_RMAP_MRV_HPP_

#include <sbgraph/algorithms/scc/mrv.hpp>
#include <sbgraph/sbg/directed_sbg.hpp>
#include <sbgraph/sbg/pw_map.hpp>
#include <sbgraph/sbg/set.hpp>

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// MRV Algorithm ---------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Decreasing RMap implementation to calculate MRV.
 */
class LtRmapMRV : public MRVContext<LtRmapMRV> {
public:
  LtRmapMRV();
 
  /**
   * @brief Concrete implementation that starts with the identity pw for every
   * vertex. Then, it finds edges with a greater representative in its start
   * than its end. It uses those edges to calculate a decreasing Rmap.
   * It also handles repetitive paths (i.e. paths that have a length depending
   * on the size of the intervals that define the DSBG).
   */
  PWMap calculate(const DirectedSBG& dsbg);

private:
  /**
   * @brief Given the current state of _rmap, identifies edges (u, v) such that
   * _rmap(u) > _rmap(v) and _rmap(v) has the minimum representative among the
   * successors of u.
   */
  Set decreasingRepresentative();

  /*
   * @brief Calculates the MRV for repetitive paths.
   */
  PWMap repetition(const Set& E);

  DirectedSBG _dsbg;
  PWMap _old_rmap; ///< MRV map in the previous iteration.
  PWMap _rmap; ///< MRV map.
  Set _decreasing;
};

} // namespace LIB

} // namespace SBG

#endif // SBGRAPH_ALGORITHMS_SCC_DECREASING_RMAP_MRV_HPP_
