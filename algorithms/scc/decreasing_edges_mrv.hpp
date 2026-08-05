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

#ifndef SBGRAPH_ALGORITHMS_SCC_DECREASING_EDGES_MRV_HPP_
#define SBGRAPH_ALGORITHMS_SCC_DECREASING_EDGES_MRV_HPP_

#include "algorithms/scc/mrv.hpp"
#include "sbg/directed_sbg.hpp"
#include "sbg/pw_map.hpp"
#include "sbg/set.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// MRV Algorithm ---------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Decreasing Edges implementation to calculate MRV.
 */
class LtEdgesMRV : public MRVContext<LtEdgesMRV> {
public:
  LtEdgesMRV();
 
  /**
   * @brief Concrete implementation that starts with the identity pw for every
   * vertex. Then, it finds edges with a greater representative in its start
   * than its end. With those edges it constructs a successor map, which is
   * composed with itself up to convergence.
   * It also handles repetitive paths (i.e. paths that have a length depending
   * on the size of the intervals that define the DSBG).
   */
  PWMap calculate(const DirectedSBG& dsbg);

private:
  /**
   * @brief Given the current state of rmap_, returns the set of edges (u, v)
   * such that rmap_(u) > rmap_(v), which are edges leading to a new
   * minimum MRV.
   */
  Set decreasingRepresentative(const PWMap& rmap) const;

  /*
   * @brief Calculates the MRV for repetitive paths.
   */
  PWMap repetitivePaths(const PWMap& rmap, const PWMap& decreasing_smap);

  DirectedSBG _dsbg;
  PWMap _smap;
  Set _visitedSE;
  unsigned int _n;
};

} // namespace LIB

} // namespace SBG

#endif // SBGRAPH_ALGORITHMS_SCC_DECREASING_EDGES_MRV_HPP_
