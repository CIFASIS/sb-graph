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
MD_NAT::const_iterator MD_NAT::begin() const { return value_.begin(); }
MD_NAT::const_iterator MD_NAT::end() const { return value_.end(); }

std::size_t MD_NAT::size() const { return value_.size(); }

void MD_NAT::emplace(MD_NAT::iterator it, NAT x) { value_.emplace(it, x); }
void MD_NAT::emplaceBack(NAT x) { value_.emplace_back(x); }
void MD_NAT::push_back(NAT x) { emplaceBack(x); }

NAT &MD_NAT::operator[](std::size_t n) { return value_[n]; }
const NAT &MD_NAT::operator[](std::size_t n) const { return value_[n]; }

bool MD_NAT::operator==(const MD_NAT &other) const
{
  ERROR_UNLESS(
      size() == other.size()
      , "Util::MD_NAT::operator==: dimensions don't match"
  );

  return value_ == other.value_;
}

bool MD_NAT::operator<(const MD_NAT &other) const
{
  ERROR_UNLESS(
      size() == other.size()
      , "Util::MD_NAT::operator<: dimensions don't match"
  );

  for (unsigned int j = 0; j < size(); ++j)
    if (operator[](j) < other[j])
      return true;

  return false;
}

bool MD_NAT::operator<=(const MD_NAT &other) const
{
  return *this == other || *this < other;
}

MD_NAT MD_NAT::operator+=(const MD_NAT &other) const
{
  MD_NAT res = *this;
  for (unsigned int j = 0; j < size(); ++j)
    res[j] += other[j];

  return res;
}

MD_NAT MD_NAT::operator+(const MD_NAT &other) const
{
  return *this += other;
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
  boost::rational<INT> value_res = value_;

  value_res += other.value_;

  return RATIONAL(value_res);
}

RATIONAL RATIONAL::operator+(const RATIONAL &other) const
{
  return *this += other;
}

RATIONAL RATIONAL::operator-=(const RATIONAL &other) const
{
  boost::rational<INT> value_res = value_;

  value_res -= other.value_;

  return RATIONAL(value_res);
}

RATIONAL RATIONAL::operator-(const RATIONAL &other) const
{
  return *this -= other;
}

RATIONAL RATIONAL::operator*=(const RATIONAL &other) const
{
  boost::rational<INT> value_res = value_;

  value_res *= other.value_;

  return RATIONAL(value_res);
}

RATIONAL RATIONAL::operator*(const RATIONAL &other) const
{
  return *this *= other;
}

RATIONAL RATIONAL::operator/=(const RATIONAL &other) const
{
  boost::rational<INT> value_res = value_;

  value_res *= other.value_;

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

  ERROR("toNat: RATIONAL is not NAT");
  return 0;
}

INT RATIONAL::toInt() const
{
  if (denominator() == 1)
    return numerator();

  ERROR("toInt: RATIONAL is not INT");
  return 0;
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

std::size_t hash_value(const RATIONAL &r)
{
  std::size_t seed = 0;
  boost::hash_combine(seed, r.numerator());
  boost::hash_combine(seed, r.denominator());

  return seed;
}

} // namespace Util

} // namespace SBG
