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
MD_NAT::MD_NAT(NAT x) : value_() { value_.emplace_back(x); }
MD_NAT::MD_NAT(unsigned int nmbr_copies, NAT x) : value_() { 
  for (unsigned int j = 0; j < nmbr_copies; j++)
    value_.emplace_back(x); 
}
MD_NAT::MD_NAT(MD_NAT::iterator b, MD_NAT::iterator e) : value_(b, e) {}

member_imp(MD_NAT, VNAT, value);

MD_NAT::iterator MD_NAT::begin() { return value_.begin(); }
MD_NAT::iterator MD_NAT::end() { return value_.end(); }

std::size_t MD_NAT::size() { return value().size(); }

void MD_NAT::emplace(MD_NAT::iterator it, NAT x) { value_.emplace(it, x); }
void MD_NAT::emplace_back(NAT x) { value_.emplace_back(x); }
void MD_NAT::push_back(NAT x) { emplace_back(x); }

NAT &MD_NAT::operator[](std::size_t n) { return value_[n]; }

bool MD_NAT::operator==(const MD_NAT &md) const
{ 
  return value() == md.value();
}

bool MD_NAT::operator<(const MD_NAT &x2) const
{
  MD_NAT aux1 = *this, aux2 = x2;

  for (auto const &[e1, e2] : boost::combine(aux1, aux2)) 
    if (e1 < e2)
      return true;

  return false;
}

bool MD_NAT::operator<=(const MD_NAT &x2) const
{
  return *this == x2 || *this < x2;
}

std::ostream &operator<<(std::ostream &out, const MD_NAT &md)
{
  MD_NAT aux = md;
  unsigned int sz = aux.size();

  if (sz == 1) { 
    out << aux[0];
    return out;
  }

  if (sz > 0) {
    out << "(";
    for (unsigned int j = 0; j < sz - 1; j++)
      out << aux[j] << ", ";
    out << aux[sz-1];
    out << ")";
  }

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

INT RATIONAL::numerator() const { return value().numerator(); }

INT RATIONAL::denominator() const { return value().denominator(); }

NAT toNat(RATIONAL r)
{
  if (r.denominator() == 1 && 0 <= r.value())
    return r.numerator();

  std::stringstream ss;
  ss << r;
  ERROR("toNat: RATIONAL %s is not NAT", ss.str().c_str());
  return 0;
}

INT toInt(RATIONAL r)
{
  if (r.denominator() == 1)
    return r.numerator();

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

bool RATIONAL::operator==(const RATIONAL &r) const
{
  return value() == r.value();
}

bool RATIONAL::operator<(const RATIONAL &r) const
{
  return value() < r.value();
}

bool RATIONAL::operator>(const RATIONAL &r) const
{
  return value() > r.value();
}

bool RATIONAL::operator!=(const RATIONAL &r) const
{
  return value() != r.value();
}

RATIONAL operator-(const RATIONAL &r) 
{
  auto aux = r; 
  return RATIONAL(-aux.numerator(), aux.denominator());
}

std::ostream &operator<<(std::ostream &out, const RATIONAL &r)
{
  boost::rational<INT> rv = r.value();
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

bool isZero(RATIONAL r) { return r.numerator() == 0; }

bool isOne(RATIONAL r) { return r.numerator() == r.denominator(); }

std::size_t hash_value(const RATIONAL &r)
{
  std::size_t seed = 0;
  boost::hash_combine(seed, r.numerator());
  boost::hash_combine(seed, r.denominator());

  return seed;
}

} // namespace Util

} // namespace SBG
