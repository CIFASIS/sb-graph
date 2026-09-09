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

Rational::Rational() : _value() {}

Rational::Rational(Int n) : _value(Rational::RationalT{n, 1}) {}

Rational::Rational(const Rational::RationalT& value) : _value(value) {}

Rational::Rational(Int n, Int d) : _value()
{
  boost::rational<long long int> v{n, d};
  _value = v;
}

// Getters ---------------------------------------------------------------------

const Rational::RationalT& Rational::value() const  { return _value; }

Int Rational::numerator() const { return _value.numerator(); }

Int Rational::denominator() const { return _value.denominator(); }

// Operators -------------------------------------------------------------------

bool Rational::operator==(const Rational& r) const
{
  return _value == r._value;
}

bool Rational::operator!=(const Rational& r) const
{
  return _value != r._value;
}

bool Rational::operator<(const Rational& r) const
{
  return _value < r._value;
}

bool Rational::operator>(const Rational& r) const
{
  return _value > r._value;
}

bool Rational::operator>=(const Rational& r) const
{
  return _value >= r._value;
}

bool Rational::operator==(const Int& other) const
{
  return numerator() == other && denominator() == 1;
}

Rational Rational::operator-() const
{
  return Rational{-_value};
}

Rational Rational::operator+(const Rational& other) const
{
  return Rational{_value + other._value};
}

Rational Rational::operator-(const Rational& other) const
{
  return Rational{_value - other._value};
}

Rational Rational::operator*(const Rational& other) const
{
  return Rational{_value*other._value};
}

Rational Rational::operator/(const Rational& other) const
{
  return Rational{_value/other._value};
}

// Member functions ------------------------------------------------------------

Int Rational::toInt() const
{
  if (denominator() == 1) {
    return numerator();
  }
  return 0;
}

Int Rational::floor() const
{
  return boost::rational_cast<Int>(_value);
}

Int Rational::ceiling() const
{
  Int trunc = boost::rational_cast<Int>(_value);
  return _value == trunc ? trunc : trunc + 1;
}

std::ostream& operator<<(std::ostream& out, const Rational& r)
{
  Rational::RationalT rv = r.value();
  Int num = rv.numerator(), den = rv.denominator();

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
