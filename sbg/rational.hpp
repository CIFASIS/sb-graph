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

#ifndef SBGRAPH_SBG_RATIONAL_HPP_
#define SBGRAPH_SBG_RATIONAL_HPP_

#include "sbg/natural.hpp"

#include <boost/rational.hpp>

#include <iosfwd>
#include <limits>

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Rationals implementation ----------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

/** @typedef INT 
 *
 * @brief Integers implementation, used in rationals definition. 
 */
using INT = long long int;
constexpr INT INT_Inf = std::numeric_limits<INT>::max();

/**
 * @brief Used as coefficients and slopes in linear expressions.
 */
class RATIONAL {
public:
  using RATIONALT = boost::rational<INT>;

  /**
   * @brief Zero constructor.
   */
  RATIONAL();

  /**
   * @brief Construct rational r = n/1.
   */
  RATIONAL(INT n);

  /**
   * @brief Copy constructor.
   */
  RATIONAL(const RATIONALT& value);

  /**
   * @brief Construct rational r = n/d.
   */
  RATIONAL(INT n, INT d);

  RATIONAL(const RATIONAL& r) = default;
  RATIONAL(RATIONAL&& r) = default;

  const RATIONALT& value() const;
  INT numerator() const;
  INT denominator() const;

  RATIONAL& operator=(const RATIONAL& other) = default;
  RATIONAL& operator=(RATIONAL&& other) = default;
  bool operator==(const RATIONAL& other) const;
  bool operator!=(const RATIONAL& other) const;
  bool operator<(const RATIONAL& other) const;
  bool operator>(const RATIONAL& other) const;
  bool operator>=(const RATIONAL& other) const;
  bool operator==(const INT& other) const;

  RATIONAL operator-() const;
  RATIONAL operator+(const RATIONAL& other) const;
  RATIONAL operator-(const RATIONAL& other) const;
  RATIONAL operator*(const RATIONAL& other) const;
  RATIONAL operator/(const RATIONAL& other) const;

  NAT toNat() const;
  INT toInt() const;
  INT floor() const;
  INT ceiling() const;

private:
  RATIONALT _value;
};
std::ostream& operator<<(std::ostream& out, const RATIONAL& r);

} // namespace LIB

}  // namespace SBG

#endif // SBGRAPH_SBG_RATIONAL_HPP_
