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

#include "sbg/multidim_inter.hpp"

#include <iostream>

namespace SBG {

namespace LIB {

namespace detail {

// Constructors/Destructors ----------------------------------------------------

MultiDimInter::MultiDimInter() : _intervals() {}

MultiDimInter::MultiDimInter(const IntTuple& x) : _intervals()
{
  for (const Int xi : x) {
    _intervals.emplace_back(Interval{xi, 1, xi});
  }
}

MultiDimInter::MultiDimInter(const Interval& i) : _intervals()
{
  _intervals.push_back(i);
}

MultiDimInter::MultiDimInter(const std::size_t k, const Interval& i)
  : _intervals()
{
  for (unsigned int j = 0; j < k; ++j) {
    _intervals.push_back(i);
  }
}

MultiDimInter::MultiDimInter(InterVector iv) : _intervals(std::move(iv)) {}

// Getters ---------------------------------------------------------------------

MultiDimInter::ConstIt MultiDimInter::begin() const
{
  return _intervals.begin();
}

MultiDimInter::ConstIt MultiDimInter::end() const
{
  return _intervals.end();
}

// Setters ---------------------------------------------------------------------

void MultiDimInter::pushBack(const Interval& i) 
{ 
  if (!i.isEmpty()) {
    _intervals.push_back(i);
  }
}

// Operators -------------------------------------------------------------------

Interval& MultiDimInter::operator[](std::size_t n)
{
  return _intervals[n];
}

const Interval& MultiDimInter::operator[](std::size_t n) const
{
  return _intervals[n];
}

bool MultiDimInter::operator==(const MultiDimInter& other) const
{
  return _intervals == other._intervals;
}

bool MultiDimInter::operator!=(const MultiDimInter& other) const
{
  return !(*this == other);
}

bool MultiDimInter::operator<(const MultiDimInter& other) const
{
  if (other.isEmpty()) {
    return false;
  }

  if (isEmpty()) {
    return true;
  }

  return minElem() < other.minElem();
}

bool MultiDimInter::operator>(const MultiDimInter& other) const
{
  return !(*this == other && *this < other);
}

std::ostream& operator<<(std::ostream& out, const MultiDimInter& mdi)
{
  std::size_t sz = mdi.arity();

  if (sz > 0) {
    for (std::size_t j = 0; j < sz - 1; ++j) {
      out << mdi[j] << "x";
    }
    out << mdi[sz-1];
  }

  return out;
}

// Set functions ---------------------------------------------------------------

unsigned int MultiDimInter::cardinal() const
{
  unsigned int result = 1;

  for (const Interval& i : _intervals) {
    result *= i.cardinal();
  }

  return result;
}

bool MultiDimInter::isEmpty() const { return _intervals.empty(); }

IntTuple MultiDimInter::minElem() const
{
  IntTuple result;

  for (const Interval& i : _intervals) {
    result.pushBack(i.begin());
  }

  return result;
}

IntTuple MultiDimInter::maxElem() const
{
  IntTuple result;

  for (const Interval& i : _intervals) {
    result.pushBack(i.end());
  }

  return result;
}

MultiDimInter MultiDimInter::intersection(const MultiDimInter& other) const
{
  if (isEmpty() || other.isEmpty()) {
    return MultiDimInter{};
  }

  MultiDimInter result;

  for (unsigned int j = 0; j < arity(); ++j) {
    Interval jth_intersection = operator[](j).intersection(other[j]);
    if (jth_intersection.isEmpty()) {
      return MultiDimInter{};
    } else {
      result.pushBack(jth_intersection);
    }
  }

  return result;
}

MultiDimInter MultiDimInter::cartesianProduct(const MultiDimInter& other) const
{
  MultiDimInter result = *this;

  for (const Interval& other_i : other._intervals) {
    result.pushBack(other_i); 
  }

  return result;
}

// Extra operations ------------------------------------------------------------

std::size_t MultiDimInter::arity() const { return _intervals.size(); }

MultiDimInter MultiDimInter::translate(const IntTuple& t) const
{
  MultiDimInter result;

  for (unsigned int j = 0; j < arity(); ++j) {
    result.pushBack(operator[](j).translate(t[j]));
  }

  return result;
}

MultiDimInter MultiDimInter::least(const MultiDimInter& other) const
{
  return std::min(*this, other);
}

Perimeter MultiDimInter::perimeter() const
{
  return Perimeter{minElem(), maxElem()};
}

MaybeMDI MultiDimInter::compact(const MultiDimInter& other) const
{
  MultiDimInter result;

  unsigned int j = 0;
  for (; j < arity(); ++j) {
    Interval jth_this = operator[](j);
    Interval jth_other = other[j];
    if (jth_this == jth_other) {
      result.pushBack(jth_this);
    } else {
      MaybeInterval jth_compact = jth_this.compact(jth_other);
      if (jth_compact) {
        result.pushBack(jth_compact.value());
        ++j;
        break;
      } else {
        return {};
      }
    }
  }

  for (; j < arity(); ++j) {
    if (operator[](j) != other[j]) {
      return {};
    } else {
      result.pushBack(operator[](j));
    }
  }

  return result;
}

// Non-member functions --------------------------------------------------------

rapidjson::Value toJSON(MultiDimInter mdi
  , rapidjson::Document::AllocatorType& alloc)
{
  rapidjson::Value interval_array{rapidjson::kArrayType};
  for (const Interval& i : mdi) {
    rapidjson::Value jth = toJSON(i, alloc);
    interval_array.PushBack(jth, alloc);
  }
  rapidjson::Value result{rapidjson::kObjectType};
  result.AddMember("bounds", interval_array, alloc);

  return result;
}

} // namespace detail

} // namespace LIB

} // namespace SBG
