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

#ifndef SBGRAPH_SBG_Rational_HPP_
#define SBGRAPH_SBG_Rational_HPP_

#include "sbg/integer.hpp"

#include <boost/rational.hpp>

#include <iosfwd>
#include <limits>

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Rationals implementation ----------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Used as coefficients and slopes in linear expressions.
 */
class Rational {
public:
  using RationalT = boost::rational<Int>;

  /**
   * @brief Zero constructor.
   */
  Rational();

  /**
   * @brief Construct rational r = n/1.
   */
  Rational(Int n);

  /**
   * @brief Copy constructor.
   */
  Rational(const RationalT& value);

  /**
   * @brief Construct rational r = n/d.
   */
  Rational(Int n, Int d);

  Rational(const Rational& r) = default;
  Rational(Rational&& r) = default;

  const RationalT& value() const;
  Int numerator() const;
  Int denominator() const;

  Rational& operator=(const Rational& other) = default;
  Rational& operator=(Rational&& other) = default;
  bool operator==(const Rational& other) const;
  bool operator!=(const Rational& other) const;
  bool operator<(const Rational& other) const;
  bool operator>(const Rational& other) const;
  bool operator>=(const Rational& other) const;
  bool operator==(const Int& other) const;

  Rational operator-() const;
  Rational operator+(const Rational& other) const;
  Rational operator-(const Rational& other) const;
  Rational operator*(const Rational& other) const;
  Rational operator/(const Rational& other) const;

  Int toInt() const;
  Int floor() const;
  Int ceiling() const;

private:
  RationalT _value;
};
std::ostream& operator<<(std::ostream& out, const Rational& r);

} // namespace LIB

}  // namespace SBG

#endif // SBGRAPH_SBG_Rational_HPP_
