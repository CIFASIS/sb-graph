/*******************************************************************************

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

#include "sbg/rational.hpp"

namespace SBG {

namespace LIB {

RATIONAL::RATIONAL() : value_() {}
RATIONAL::RATIONAL(NAT n) : value_(RatType(n, 1)) {}
RATIONAL::RATIONAL(const RatType &value) : value_(value) {}
RATIONAL::RATIONAL(INT n, INT d) : value_() {
  boost::rational<long long int> v(n, d);
  set_value(v);
}

member_imp(RATIONAL, RatType, value);

bool RATIONAL::operator==(const RATIONAL &r) const
{
  return value_ == r.value_;
}

bool RATIONAL::operator!=(const RATIONAL &r) const
{
  return value_ != r.value_;
}

bool RATIONAL::operator<(const RATIONAL &r) const
{
  return value_ < r.value_;
}

bool RATIONAL::operator>(const RATIONAL &r) const
{
  return value_ > r.value_;
}

bool RATIONAL::operator==(const INT &other) const
{
  RATIONAL aux = *this;
  return aux.numerator() == other && aux.denominator() == 1;
}

RATIONAL RATIONAL::operator-() const
{
  return RATIONAL(-numerator(), denominator());
}

RATIONAL RATIONAL::operator+=(const RATIONAL &other) const
{
  RatType value_res = value_;

  value_res += other.value_;

  return RATIONAL(value_res);
}

RATIONAL RATIONAL::operator+(const RATIONAL &other) const
{
  return *this += other;
}

RATIONAL RATIONAL::operator-=(const RATIONAL &other) const
{
  RatType value_res = value_;

  value_res -= other.value_;

  return RATIONAL(value_res);
}

RATIONAL RATIONAL::operator-(const RATIONAL &other) const
{
  return *this -= other;
}

RATIONAL RATIONAL::operator*=(const RATIONAL &other) const
{
  RatType value_res = value_;

  value_res *= other.value_;

  return RATIONAL(value_res);
}

RATIONAL RATIONAL::operator*(const RATIONAL &other) const
{
  return *this *= other;
}

RATIONAL RATIONAL::operator/=(const RATIONAL &other) const
{
  RatType value_res = value_;

  value_res /= other.value_;

  return RATIONAL(value_res);
}

RATIONAL RATIONAL::operator/(const RATIONAL &other) const
{
  return *this /= other;
}


INT RATIONAL::numerator() const { return value_.numerator(); }

INT RATIONAL::denominator() const { return value_.denominator(); }

NAT RATIONAL::toNat() const
{
  if (denominator() == 1 && 0 <= value_)
    return numerator();

  return 0;
}

INT RATIONAL::toInt() const
{
  if (denominator() == 1)
    return numerator();

  return 0;
}

std::ostream &operator<<(std::ostream &out, const RATIONAL &r)
{
  RatType rv = r.value();
  INT num = rv.numerator(), den = rv.denominator();

  if (num == 0) {
    out << "0";
    return out;
  }

  out << num;
  if (den != 1)
    out << "/" << den;

  return out;
}

} // namespace LIB

} // namespace SBG
