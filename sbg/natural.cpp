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

#include "sbg/natural.hpp"

namespace SBG {

namespace LIB {

member_imp(MD_NAT, VNAT, value);

MD_NAT::MD_NAT() : value_() {}
MD_NAT::MD_NAT(NAT x) : value_() { value_.push_back(x); }
MD_NAT::MD_NAT(unsigned int nmbr_copies, NAT x) : value_() { 
  for (unsigned int j = 0; j < nmbr_copies; j++)
    value_.push_back(x); 
}
MD_NAT::MD_NAT(MD_NAT::iterator b, MD_NAT::iterator e) : value_(b, e) {}

MD_NAT::iterator MD_NAT::begin() { return value_.begin(); }
MD_NAT::iterator MD_NAT::end() { return value_.end(); }
MD_NAT::const_iterator MD_NAT::begin() const { return value_.begin(); }
MD_NAT::const_iterator MD_NAT::end() const { return value_.end(); }

void MD_NAT::emplaceBack(NAT x) { value_.push_back(x); }

NAT &MD_NAT::operator[](std::size_t n) { return value_[n]; }
const NAT &MD_NAT::operator[](std::size_t n) const { return value_[n]; }

bool MD_NAT::operator==(const MD_NAT &other) const
{
  return value_ == other.value_;
}

bool MD_NAT::operator!=(const MD_NAT &other) const { return !(*this == other); }

bool MD_NAT::operator<(const MD_NAT &other) const
{
  for (unsigned int j = 0; j < arity(); ++j) {
    if (operator[](j) < other[j])
      return true;

    if (operator[](j) > other[j])
      return false;
  }

  return false;
}

bool MD_NAT::menorThan(const MD_NAT &other) const
{
  for (unsigned int j = 0; j < arity(); ++j)
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
  for (unsigned int j = 0; j < arity(); ++j)
    res[j] += other[j];

  return res;
}

MD_NAT MD_NAT::operator+(const MD_NAT &other) const
{
  return *this += other;
}

std::size_t MD_NAT::arity() const { return value_.size(); }

std::ostream &operator<<(std::ostream &out, const MD_NAT &md)
{
  MD_NAT aux = md;
  unsigned int sz = aux.arity();

  if (sz == 1)  
    out << aux[0];

  if (sz > 1) {
    out << "(";
    for (unsigned int j = 0; j < sz - 1; j++)
      out << aux[j] << ", ";
    out << aux[sz-1];
    out << ")";
  }

  return out;
}

} // namespace LIB

} // namespace SBG
