/** @file rational.hpp

 @brief <b>Rationals implementation</b>

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

#ifndef SBG_RAT_HPP
#define SBG_RAT_HPP

#include <boost/rational.hpp>

#include "sbg/natural.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Rationals implementation ----------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

/** @typedef INT 
 *
 * @brief Integers implementation, used in rationals definition. 
 */
typedef long long int INT;
const INT INT_Inf = std::numeric_limits<INT>::max();

typedef boost::rational<INT> RatType;

/**
 * @brief Used as coefficients and slopes in linear expressions.
 */
struct RATIONAL {
  member_class(RatType, value);

  /**
   * @brief Zero constructor.
   */
  RATIONAL();

  /**
   * @brief Construct rational r = n/1.
   */
  RATIONAL(NAT n);

  /**
   * @brief Copy constructor.
   */
  RATIONAL(const RatType &value);

  /**
   * @brief Construct rational r = n/d.
   */
  RATIONAL(INT n, INT d);

  bool operator==(const RATIONAL &other) const;
  bool operator!=(const RATIONAL &other) const;
  bool operator<(const RATIONAL &other) const;
  bool operator>(const RATIONAL &other) const;
  bool operator>=(const RATIONAL& other) const;
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
  INT floor() const;
  INT ceiling() const;
};
std::ostream &operator<<(std::ostream &out, const RATIONAL &r);

} // namespace LIB

}  // namespace SBG

#endif
