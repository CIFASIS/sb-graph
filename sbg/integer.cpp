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

#include "sbg/integer.hpp"

#include <iostream>

namespace SBG {

namespace LIB {

// Constructors/Destructors ----------------------------------------------------

IntTuple::IntTuple() : _value() {}

IntTuple::IntTuple(const Int x) : _value() { _value.emplace_back(x); }

IntTuple::IntTuple(const std::size_t k, const Int x) : _value()
{ 
  for (unsigned int j = 0; j < k; ++j) {
    _value.push_back(x);
  }
}

IntTuple::IntTuple(IntTuple::Iterator b, IntTuple::Iterator e) : _value(b, e) {}

// Getters ---------------------------------------------------------------------

IntTuple::Iterator IntTuple::begin() { return _value.begin(); }

IntTuple::Iterator IntTuple::end() { return _value.end(); }

IntTuple::ConstIterator IntTuple::begin() const { return _value.begin(); }

IntTuple::ConstIterator IntTuple::end() const { return _value.end(); }

// Setters ---------------------------------------------------------------------

void IntTuple::pushBack(const Int x) { _value.push_back(x); }

// Operators -------------------------------------------------------------------

Int& IntTuple::operator[](std::size_t n) { return _value[n]; }

const Int& IntTuple::operator[](std::size_t n) const { return _value[n]; }

bool IntTuple::operator==(const IntTuple& other) const
{
  return _value == other._value;
}

bool IntTuple::operator!=(const IntTuple& other) const { return !(*this == other); }

bool IntTuple::operator<(const IntTuple& other) const
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


bool IntTuple::operator<=(const IntTuple& other) const
{
  return *this == other || *this < other;
}

IntTuple IntTuple::operator+(const IntTuple& other) const
{
  IntTuple result;

  for (auto j = 0; j < _value.size(); ++j) {
    result.pushBack(operator[](j) + other[j]);
  }

  return result;
}

// Member functions ------------------------------------------------------------

std::size_t IntTuple::arity() const { return _value.size(); }

std::ostream& operator<<(std::ostream& out, const IntTuple& md)
{
  IntTuple aux = md;
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
