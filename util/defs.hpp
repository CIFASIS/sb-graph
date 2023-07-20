/** @file defs.hpp

 @brief <b>Useful macros</b>
 
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

#include <optional>

#include <boost/range/combine.hpp>
#include <boost/rational.hpp>

namespace SBG {

namespace Util {

// Helpful macros --------------------------------------------------------------

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
  X Y##_;                  \
  X Y() const;             \
  void set_##Y(X x);       \
  X &Y##_ref();

#define member_imp(C, X, Y)          \
  X C::Y() const { return Y##_; }    \
  void C::set_##Y(X x) { Y##_ = x; } \
  X &C::Y##_ref() { return Y##_; }

#define member_imp_temp(T, C, X, Y)    \
  T X C::Y() const { return Y##_; }    \
  T void C::set_##Y(X x) { Y##_ = x; } \
  T X &C::Y##_ref() { return Y##_; }

#define eq_class(X) bool operator==(const X &other) const;
#define neq_class(X) bool operator!=(const X &other) const;
#define lt_class(X) bool operator<(const X &other) const;
#define gt_class(X) bool operator>(const X &other) const;

#define ApplyThis(X) boost::apply_visitor(*this, X)
#define Apply(X, Y) boost::apply_visitor(X, Y)
#define Apply2(X, Y, Z) boost::apply_visitor(X, Y, Z)

// Helpful functions -----------------------------------------------------------

/**
 * @brief Functions that access the i-th element of given collection simultaneously.
 */

#define parallel_foreach2(X, Y) for (auto&& items : boost::combine(X, Y))
#define parallel_foreach3(X, Y, Z) for (auto&& items : boost::combine(X, Y, Z))
#define parallel_foreach4(W, X, Y, Z) for (auto&& items : boost::combine(W, X, Y, Z))

// Intervals definitions -------------------------------------------------------

/** @typedef INT 
 *
 * @brief Representation of individual elements. 
 */
typedef unsigned long long int INT;
std::string to_str(INT);

/** @typedef RATIONAL 
 *
 * @brief Used for coefficients in linear expressions. 
 */
struct RATIONAL{
  member_class(boost::rational<INT>, value);

  RATIONAL();
  RATIONAL(boost::rational<INT> value);
  RATIONAL(INT n, INT d);

  eq_class(RATIONAL);
  neq_class(RATIONAL);

  RATIONAL operator+(const RATIONAL &r);
  RATIONAL operator-(const RATIONAL &r);
  RATIONAL operator*(const RATIONAL &r);
};
std::ostream &operator<<(std::ostream &out, const RATIONAL &r);

const INT Inf = std::numeric_limits<INT>::max();

// Parser definitions ----------------------------------------------------------

typedef std::string VariableName;

} // namespace Util

} // namespace SBG

#endif
