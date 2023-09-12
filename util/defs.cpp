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

#include "util/defs.hpp"

namespace SBG {

namespace Util {

// Naturals --------------------------------------------------------------------

std::string toStr(NAT x) { return std::to_string(x); }

MD_NAT::MD_NAT() : value_() {}
MD_NAT::MD_NAT(NAT x) : value_() { value_ref().emplace_back(x); }
MD_NAT::MD_NAT(MD_NAT::iterator b, MD_NAT::iterator e) : value_(b, e) {}

member_imp(MD_NAT, VNAT, value);

MD_NAT::iterator MD_NAT::begin() { return value_ref().begin(); }
MD_NAT::iterator MD_NAT::end() { return value_ref().end(); }

std::size_t MD_NAT::size() { return value().size(); }

void MD_NAT::emplace(MD_NAT::iterator it, NAT x) { value_ref().emplace(it, x); }
void MD_NAT::emplace_back(NAT x) { value_ref().emplace_back(x); }

NAT &MD_NAT::operator[](std::size_t n) { return value_ref()[n]; }

bool MD_NAT::operator==(const MD_NAT &md) const { return value() == md.value(); }

bool MD_NAT::operator<(const MD_NAT &x2) const
{
  MD_NAT aux1 = *this, aux2 = x2;

  parallel_foreach2 (aux1.value_ref(), aux2.value_ref()) {
    NAT e1 = boost::get<0>(items), e2 = boost::get<1>(items);
    if (e1 < e2) return true;
  }

  return false;
}

bool MD_NAT::operator<=(const MD_NAT &x2) const { return *this == x2 || *this < x2; }

std::ostream &operator<<(std::ostream &out, const MD_NAT &md)
{
  MD_NAT aux = md;
  unsigned int sz = aux.size();

  out << "(";
  if (sz > 0) {
    unsigned int j = 0;
    for (; j < sz - 1; j++)
      out << aux[j] << ", ";
    out << aux[j];
  }
  out << ")";

  return out;
}

// Rationals -------------------------------------------------------------------

RATIONAL::RATIONAL() : value_() {}
RATIONAL::RATIONAL(NAT n) : value_(boost::rational<INT>(n, 1)) {}
RATIONAL::RATIONAL(boost::rational<INT> value) : value_(value) {}
RATIONAL::RATIONAL(INT n, INT d) : value_() {
  boost::rational<long long int> v(n, d);
  set_value(v);
}

member_imp(RATIONAL, boost::rational<INT>, value);

INT RATIONAL::numerator() { return value().numerator(); }

INT RATIONAL::denominator() { return value().denominator(); }

NAT toNat(RATIONAL r)
{
  if (r.denominator() == 1 && 0 <= r.value()) return r.numerator();

  ERROR("toNat: RATIONAL is not NAT");
  return 0;
}

INT toInt(RATIONAL r)
{
  if (r.denominator() == 1) return r.numerator();

  ERROR("toInt: RATIONAL is not INT");
  return 0;
}

RATIONAL RATIONAL::operator+=(const RATIONAL &r)
{
  value_ref() += r.value();
  return *this;
}

RATIONAL RATIONAL::operator-=(const RATIONAL &r) 
{
  value_ref() -= r.value();
  return *this;
}

RATIONAL RATIONAL::operator*=(const RATIONAL &r) 
{
  value_ref() *= r.value();
  return *this;
}

RATIONAL RATIONAL::operator/=(const RATIONAL &r)
{
  value_ref() /= r.value();
  return *this;
}

RATIONAL operator+(const RATIONAL &r1, const RATIONAL &r2)
{ 
  RATIONAL res(r1);
  res += r2;
  return res;
}

RATIONAL operator-(const RATIONAL &r1, const RATIONAL &r2)
{
  RATIONAL res(r1);
  res -= r2;
  return res;
}

RATIONAL operator*(const RATIONAL &r1, const RATIONAL &r2)
{
  RATIONAL res(r1);
  res *= r2;
  return res;
}

RATIONAL operator/(const RATIONAL &r1, const RATIONAL &r2)
{
  RATIONAL res(r1);
  res /= r2;
  return res;
}

bool RATIONAL::operator==(const INT &r) const
{
  RATIONAL aux = *this;
  return aux.numerator() == r && aux.denominator() == 1;
}

bool RATIONAL::operator==(const RATIONAL &r) const { return value() == r.value(); }

bool RATIONAL::operator<(const RATIONAL &r) const { return value() < r.value(); }

bool RATIONAL::operator>(const RATIONAL &r) const { return value() > r.value(); }

bool RATIONAL::operator!=(const RATIONAL &r) const { return value() != r.value(); }

RATIONAL operator-(const RATIONAL &r) 
{
  auto aux = r; 
  return RATIONAL(-aux.numerator(), aux.denominator());
}

std::ostream &operator<<(std::ostream &out, const RATIONAL &r)
{
  boost::rational<INT> rv = r.value();

  out << rv.numerator();
  if (rv.denominator() != 1) out << "/" << rv.denominator();

  return out;
}

} // namespace Util

} // namespace SBG
