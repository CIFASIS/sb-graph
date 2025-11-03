/** @file mrv.hpp

 @brief <b>SBG Minimum Reachable Vertex (MRV) Algorithm</b>

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

#ifndef SBG_MRV_HPP
#define SBG_MRV_HPP

#include "sbg/sbg.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Paths Discoverer Algorithm --------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Compile time configurable algorithm to calculate the MRV.
 *
 * @tparam MRVImp Concrete strategy to apply. Must provide an
 * `impl(const DSBG& dsbg)` method.
 */
template<class MRVImpl>
class MRVContext {
  public:
  /**
   * @brief For every vertex of `dsbg` calculates its minimum reachable vertex.
   * @return The resulting pw is such that if pw(x) = y then MRV(x) = y.
   */
  inline PWMap calculate(const DSBG& dsbg)
  {
    return static_cast<MRVImpl*>(this)->calculate(dsbg);
  }

  protected:
  MRVContext() = default;
};

} // namespace LIB

} // namespace SBG

#endif
