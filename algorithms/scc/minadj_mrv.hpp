/** @file mrv.hpp

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

#ifndef SBG_MINADJ_MRV_HPP
#define SBG_MINADJ_MRV_HPP

#include "algorithms/scc/mrv.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Paths Discoverer Algorithm --------------------------------------------------
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
   * It also handles recursive paths (i.e. paths that have a length depending
   * on the size of the intervals that define the DSBG).
   */
  PWMap calculate(const DSBG& dsbg);

  private:
  /**
   * @brief Given the current state of rmap_, returns the set of edges (u, v)
   * such that rmap_(u) > rmap_(v), which are edges leading to a new
   * minimum MRV.
   */
  Set decreasingRepresentative(const PWMap& rmap) const;

  Set edgesInPaths(const PWMap& smap) const;

  /*
   * @brief Calculates the MRV for recursive paths.
   */
  PWMap recursivePaths(const Set& paths_edges, const Set& outgoing);

  DSBG dsbg_;
  PWMap smap_;
  Set visitedSE_;
};

} // namespace LIB

} // namespace SBG

#endif
