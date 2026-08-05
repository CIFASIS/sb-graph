/** @file defs.hpp

 @brief <b>Common definitions for all SBG modules</b>
 
 This file provides helpful definitions that will be used by all SBG modules.

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

#ifndef SBGRAPH_UTIL_DEFS_HPP_
#define SBGRAPH_UTIL_DEFS_HPP_

namespace SBG {

namespace Util {

////////////////////////////////////////////////////////////////////////////////
// Helpful macros --------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Define a variable member of a class and the getters and setters.
 *
 * The macro takes two parameters:
 *  + The variable type @c X.
 *  + The name of the variable @c Y.
 *
 * It will define the following:
 *  + A member variable called: @c Y_
 *  + A constant ref getter method: @c Y()
 *
 */
#define member_class(X, Y) \
  private:                 \
  X Y##_;                  \
  public:                  \
  const X &Y() const;

#define member_imp(C, X, Y)          \
  const X &C::Y() const { return Y##_; }

/**
 * @brief Execution time of selected functions will be printed if it's true.
 */
constexpr bool time_profiler_enabled = true;

/**
 * @brief Calling this before the program returns.
 * The execution time of the measured functions will be printed
 * iff `time_profiler_enabled` is true.
 */
void time_profiler_results();

/**
 * @brief Provides in-place lambdas to visit std::variant types.
 */

template<class... Ts>
class Overload : Ts... {
public:
  using Ts::operator()...;
  Overload(Ts... ts) : Ts(ts)... {};
};

template<class... Ts>
Overload(Ts...) -> Overload<Ts...>;

} // namespace Util

} // namespace SBG

#endif // SBGRAPH_UTIL_DEFS_HPP_
