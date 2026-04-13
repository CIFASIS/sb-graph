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

#include <iostream>

namespace SBG {

namespace LIB {

// Constructors/Destructors ----------------------------------------------------

RATIONAL::RATIONAL() : _value() {}

RATIONAL::RATIONAL(INT n) : _value(RATIONAL::RATIONALT{n, 1}) {}

RATIONAL::RATIONAL(const RATIONAL::RATIONALT& value) : _value(value) {}

RATIONAL::RATIONAL(INT n, INT d) : _value()
{
  boost::rational<long long int> v{n, d};
  _value = v;
}

// Getters ---------------------------------------------------------------------

const RATIONAL::RATIONALT& RATIONAL::value() const  { return _value; }

INT RATIONAL::numerator() const { return _value.numerator(); }

INT RATIONAL::denominator() const { return _value.denominator(); }

// Operators -------------------------------------------------------------------

bool RATIONAL::operator==(const RATIONAL& r) const
{
  return _value == r._value;
}

bool RATIONAL::operator!=(const RATIONAL& r) const
{
  return _value != r._value;
}

bool RATIONAL::operator<(const RATIONAL& r) const
{
  return _value < r._value;
}

bool RATIONAL::operator>(const RATIONAL& r) const
{
  return _value > r._value;
}

bool RATIONAL::operator>=(const RATIONAL& r) const
{
  return _value >= r._value;
}

bool RATIONAL::operator==(const INT& other) const
{
  RATIONAL aux = *this;
  return aux.numerator() == other && aux.denominator() == 1;
}

RATIONAL RATIONAL::operator-() const
{
  return RATIONAL{-_value};
}

RATIONAL RATIONAL::operator+(const RATIONAL& other) const
{
  return RATIONAL{_value + other._value};
}

RATIONAL RATIONAL::operator-(const RATIONAL& other) const
{
  return RATIONAL{_value - other._value};
}

RATIONAL RATIONAL::operator*(const RATIONAL& other) const
{
  return RATIONAL{_value*other._value};
}

RATIONAL RATIONAL::operator/(const RATIONAL& other) const
{
  return RATIONAL{_value/other._value};
}

// Extra operations ------------------------------------------------------------

NAT RATIONAL::toNat() const
{
  if (denominator() == 1 && 0 <= _value) {
    return numerator();
  }
  return 0;
}

INT RATIONAL::toInt() const
{
  if (denominator() == 1) {
    return numerator();
  }
  return 0;
}

INT RATIONAL::floor() const
{
  return boost::rational_cast<INT>(_value);
}

INT RATIONAL::ceiling() const
{
  INT trunc = boost::rational_cast<INT>(_value);
  return _value == trunc ? trunc : trunc + 1;
}

std::ostream& operator<<(std::ostream& out, const RATIONAL& r)
{
  RATIONAL::RATIONALT rv = r.value();
  INT num = rv.numerator(), den = rv.denominator();

  if (num == 0) {
    out << "0";
    return out;
  }
  out << num;
  if (den != 1) {
    out << "/" << den;
  }

  return out;
}

} // namespace LIB

} // namespace SBG
