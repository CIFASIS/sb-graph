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

#include <util/defs.hpp>

namespace SBG {

namespace Util {

std::string to_str(INT x) { return (x == Inf) ? "Inf" : std::to_string(x); };

RATIONAL::RATIONAL() : value_() {}
RATIONAL::RATIONAL(boost::rational<INT> value) : value_(value) {}
RATIONAL::RATIONAL(INT n, INT d) : value_() {
  boost::rational<INT> v(n, d);
  set_value(v);
}

member_imp(RATIONAL, boost::rational<INT>, value);

RATIONAL RATIONAL::operator+(const RATIONAL &r) { return RATIONAL(value() + r.value()); }

RATIONAL RATIONAL::operator-(const RATIONAL &r) { return RATIONAL(value() - r.value()); }

RATIONAL RATIONAL::operator*(const RATIONAL &r) { return RATIONAL(value() * r.value()); }

bool RATIONAL::operator==(const RATIONAL &r) const { return value() == r.value(); }

bool RATIONAL::operator!=(const RATIONAL &r) const { return value() != r.value(); }

std::ostream &operator<<(std::ostream &out, const RATIONAL &r)
{
  boost::rational<INT> rv = r.value();

  out << rv.numerator();
  if (rv.denominator() != 1) out << "/" << rv.denominator();

  return out;
}

} // namespace Util

} // namespace SBG
