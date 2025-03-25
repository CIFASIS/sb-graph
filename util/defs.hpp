/** @file common.hpp

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

#ifndef SBG_DEFS_HPP
#define SBG_DEFS_HPP

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
 *  + An constant getter method: @c Y()
 *  + A setter method: @c set_Y()
 *  + A getter ref method @c Y_ref()
 *
 */
#define member_class(X, Y) \
  private:                 \
  X Y##_;                  \
  public:                  \
  X Y() const;             \
  X &Y##_ref();

#define member_imp(C, X, Y)          \
  X C::Y() const { return Y##_; }    \
  void C::set_##Y(X x) { Y##_ = x; } \
  X &C::Y##_ref() { return Y##_; }

#define member_move_imp(C, X, Y)     \
  X C::Y() const { return Y##_; }    \
  void C::set_##Y(X x) { Y##_ = std::move(x); } \

} // namespace Util

} // namespace SBG

#endif
