/** @file paths.hpp

 @brief <b>SBG Path Discoverer Algorithm</b>

 @see matching.hpp and matching.cpp files to comprehend its purpose.

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

#ifndef SBGRAPH_ALGORITHMS_MATCHING_PATHS_HPP_
#define SBGRAPH_ALGORITHMS_MATCHING_PATHS_HPP_

#include "sbg/directed_sbg.hpp"
#include "sbg/pw_map.hpp"
#include "sbg/set.hpp"

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Paths Discoverer Algorithm --------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Compile time configurable algorithm for discovering paths.
 *
 * @tparam PathsImp Concrete strategy to apply. Must provide an
 * `impl(const DSBG& dsbg, const Set& endings)` method.
 */
template<class PathsImpl>
class PathsContext {
public:
  /**
   * @brief For every vertex of `dsbg` calculates a path starting from itself
   * up to a vertex in `endings`.
   * @return The resulting pw is such that if pw(x) = y, then y is the successor
   * of x in the path.
   */
  inline PWMap calculate(const DirectedSBG& dsbg, const Set& endings)
  {
    return static_cast<PathsImpl*>(this)->calculate(dsbg, endings);
  }

protected:
  PathsContext() = default;
};

} // namespace detail

} // namespace LIB

} // namespace SBG

#endif // SBGRAPH_ALGORITHMS_MATCHING_PATHS_HPP_
