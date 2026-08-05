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

#include <iostream>

namespace SBG {

namespace LIB {

// Constructors/Destructors ----------------------------------------------------

MD_NAT::MD_NAT() : _value() {}

MD_NAT::MD_NAT(const NAT x) : _value() { _value.emplace_back(x); }

MD_NAT::MD_NAT(const std::size_t k, const NAT x) : _value()
{ 
  for (unsigned int j = 0; j < k; ++j) {
    _value.emplace_back(x);
  }
}

MD_NAT::MD_NAT(MD_NAT::Iterator b, MD_NAT::Iterator e) : _value(b, e) {}

// Getters ---------------------------------------------------------------------

MD_NAT::Iterator MD_NAT::begin() { return _value.begin(); }

MD_NAT::Iterator MD_NAT::end() { return _value.end(); }

MD_NAT::ConstIterator MD_NAT::begin() const { return _value.begin(); }

MD_NAT::ConstIterator MD_NAT::end() const { return _value.end(); }

// Setters ---------------------------------------------------------------------

void MD_NAT::pushBack(const NAT x) { _value.push_back(x); }

// Operators -------------------------------------------------------------------

NAT& MD_NAT::operator[](std::size_t n) { return _value[n]; }

const NAT& MD_NAT::operator[](std::size_t n) const { return _value[n]; }

bool MD_NAT::operator==(const MD_NAT& other) const
{
  return _value == other._value;
}

bool MD_NAT::operator!=(const MD_NAT& other) const { return !(*this == other); }

bool MD_NAT::operator<(const MD_NAT& other) const
{
  for (unsigned int j = 0; j < arity(); ++j) {
    if (operator[](j) < other[j]) {
      return true;
    } else if (operator[](j) > other[j]) {
      return false;
    }
  }

  return false;
}


bool MD_NAT::operator<=(const MD_NAT& other) const
{
  return *this == other || *this < other;
}

MD_NAT MD_NAT::operator+(const MD_NAT& other) const
{
  MD_NAT result;

  for (auto j = 0; j < _value.size(); ++j) {
    result.pushBack(operator[](j) + other[j]);
  }

  return result;
}

// Extra operations ------------------------------------------------------------

std::size_t MD_NAT::arity() const { return _value.size(); }

std::ostream& operator<<(std::ostream& out, const MD_NAT& md)
{
  MD_NAT aux = md;
  unsigned int sz = aux.arity();

  if (sz == 1) {
    out << aux[0];
  } else if (sz > 1) {
    out << "(";
    for (unsigned int j = 0; j < sz - 1; ++j)
      out << aux[j] << ", ";
    out << aux[sz-1];
    out << ")";
  }

  return out;
}

} // namespace LIB

} // namespace SBG
