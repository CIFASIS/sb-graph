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

#include <iostream>
#include <optional>
#include <variant>

#include <boost/functional/hash.hpp>
#include <boost/rational.hpp>

#include "util/debug.hpp"

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

// Intervals definitions -------------------------------------------------------

typedef long long unsigned int NAT;
std::string toStr(NAT x);

typedef std::vector<NAT> VNAT;
struct MD_NAT {
  member_class(VNAT, value);

  MD_NAT();
  MD_NAT(NAT x);
  MD_NAT(unsigned int nmbr_copies, NAT x);
  MD_NAT(VNAT::iterator b, VNAT::iterator e);

  typedef VNAT::iterator iterator;
  typedef VNAT::const_iterator const_iterator;
  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;
  void emplace(iterator it, NAT x);
  void emplaceBack(NAT x);
  void push_back(NAT x);
  NAT &operator[](std::size_t n);
  const NAT &operator[](std::size_t n) const;

  bool operator==(const MD_NAT &other) const;
  bool operator!=(const MD_NAT &other) const;
  bool operator<(const MD_NAT &other) const;
  bool operator<=(const MD_NAT &other) const;

  MD_NAT operator+=(const MD_NAT &other) const;
  MD_NAT operator+(const MD_NAT &other) const;

  std::size_t arity() const;
};
std::ostream &operator<<(std::ostream &out, const MD_NAT &md);

std::size_t hash_value(const MD_NAT &n);

const NAT Inf = std::numeric_limits<NAT>::max();

/** @typedef INT 
 *
 * @brief Representation of individual elements. 
 */
typedef long long int INT;
const INT INT_Inf = std::numeric_limits<INT>::max();

/** @typedef RATIONAL 
 *
 * @brief Used for coefficients in linear expressions. 
 */
struct RATIONAL{
  member_class(boost::rational<INT>, value);

  RATIONAL();
  RATIONAL(NAT n);
  RATIONAL(boost::rational<INT> value);
  RATIONAL(INT n, INT d);

  bool operator==(const RATIONAL &other) const;
  bool operator!=(const RATIONAL &other) const;
  bool operator<(const RATIONAL &other) const;
  bool operator>(const RATIONAL &other) const;
  bool operator==(const INT &other) const;

  RATIONAL operator-() const;
  RATIONAL operator+=(const RATIONAL &other) const;
  RATIONAL operator+(const RATIONAL &other) const;
  RATIONAL operator-=(const RATIONAL &other) const;
  RATIONAL operator-(const RATIONAL &other) const;
  RATIONAL operator*=(const RATIONAL &other) const;
  RATIONAL operator*(const RATIONAL &other) const;
  RATIONAL operator/=(const RATIONAL &other) const;
  RATIONAL operator/(const RATIONAL &other) const;

  INT numerator() const;
  INT denominator() const;
  NAT toNat() const;
  INT toInt() const;
};
std::ostream &operator<<(std::ostream &out, const RATIONAL &r);

std::size_t hash_value(const RATIONAL &r);

// Parser definitions ----------------------------------------------------------

typedef std::string VariableName;

// std::variant definitions ----------------------------------------------------

/** @struct Overload
 *
 * @brief To provide in-place lambdas for visitation for the different
 * operations. These is needed because different structures share the same
 * functions (for example, isEmpty can be applied to intervals, sets, etc.).
 */

template<class... Ts> struct Overload : Ts... { using Ts::operator()...; };
template<class... Ts> Overload(Ts...) -> Overload<Ts...>;

} // namespace Util

} // namespace SBG

#endif
