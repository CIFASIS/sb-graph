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

#include "sbg/interval.hpp"
#include "util/debug.hpp"

#include <cmath>
#include <iostream>
#include <numeric>

namespace SBG {

namespace LIB {

namespace detail {

// Auxiliary functions ---------------------------------------------------------

bool isMember(const SBG::LIB::Int x, const SBG::LIB::detail::Interval& i)
{
  if (x < i.begin() || x > i.end()) {
    return false;
  }

  return ((x - i.begin()) % i.step()) == 0;
}

// Constructors/Destructors ----------------------------------------------------

Interval::Interval() : _begin(1), _step(1), _end(0) {}

Interval::Interval(const Int x) : _begin(x), _step(1), _end(x) {}

Interval::Interval(const Int begin, const Int step, const Int end) 
  : _begin(begin), _step(step), _end(end) 
{
  Util::ERROR_UNLESS(step >= 0
    , "Interval::Interval: step must be non-negative\n");

  if (end >= begin) {
    _end = end - ((end - begin) % step);
    _step = _begin == _end ? 1 : _step;
  } else {
    _begin = 1;
    _step = 1;
    _end = 0;
  }
}

// Getters ---------------------------------------------------------------------

const Int& Interval::begin() const
{
  return _begin;
}

const Int& Interval::step() const
{
  return _step;
}

const Int& Interval::end() const
{
  return _end;
}

// Operators -------------------------------------------------------------------

bool operator==(const Interval& lhs, const Interval& rhs) 
{
  return (lhs.begin() == rhs.begin()) && (lhs.step() == rhs.step())
    && (lhs.end() == rhs.end());
}

bool operator!=(const Interval& lhs, const Interval& rhs)
{
  if (lhs.begin() != rhs.begin()) {
    return true;
  }

  if (lhs.step() != rhs.step()) {
    return true;
  }

  if (lhs.end() != rhs.end()) {
    return true;
  }

  return false;
}

bool Interval::operator<(const Interval& other) const
{
  return _begin < other._begin;
}

std::ostream& operator<<(std::ostream& out, const Interval& i) 
{
  out << "[" << i.begin();
  if (i.step() != 1) {
    out << ":" << i.step();
  }
  out << ":" << i.end() << "]";
 
  return out;
}

// Set functions ---------------------------------------------------------------

unsigned int Interval::cardinal() const
{
  return std::abs(_end - _begin) / _step + 1;
}

bool Interval::isEmpty() const { return _end < _begin; }

Int Interval::minElem() const { return _begin; }

Int Interval::maxElem() const { return _end; }

Interval Interval::intersection(const Interval& other) const
{
  if (isEmpty() || other.isEmpty()) {
    return Interval{};
  }

  if ((_end < other._begin) || (other._end < _begin)) {
    return Interval{};
  }

  // Two non overlapping intervals with the same step.
  if (_step == other._step && !isMember(_begin, other)
    && !isMember(other._begin, *this)) {
    return Interval{};
  }

  Int max_begin = std::max(_begin, other._begin);
  Int new_step = std::lcm(_step, other._step);
  Int new_begin = max_begin;
  Int new_end = std::min(_end, other._end);
  bool found_member = false;
  for (Int x = max_begin; x < max_begin + new_step; ++x) {
    if (isMember(x, *this) && isMember(x, other)) {
      new_begin = x;
      found_member = true;
    }
  }

  if (found_member) {
    return Interval{new_begin, new_step, new_end};
  }

  return Interval{};
}

// Member functions ------------------------------------------------------------

Interval Interval::translate(const Int t) const
{
  return Interval{_begin + t, _step, _end + t};
}

Perimeter Interval::perimeter() const { return Perimeter(_begin, _end); }

MaybeInterval Interval::compact(const Interval& other) const
{
  if (_step == other._step) {
    if (_end + _step == other._begin) {
      return Interval{_begin, _step, other._end};
    } else if (other._end + _step == _begin) {
      return Interval{other._begin, _step, _end};
    } else if (!intersection(other).isEmpty()) {
      return Interval{std::min(_begin, other._begin),  _step
        , std::max(_end, other._end)};
    }
  }

  return {};
}

// Non-member functions --------------------------------------------------------

rapidjson::Value toJSON(Interval i, rapidjson::Document::AllocatorType& alloc)
{
  rapidjson::Value result{rapidjson::kArrayType};

  rapidjson::Value begin{static_cast<int64_t>(i.begin())};
  result.PushBack(begin, alloc);
  rapidjson::Value step{static_cast<int64_t>(i.step())};
  result.PushBack(step, alloc);
  rapidjson::Value end{static_cast<int64_t>(i.end())};
  result.PushBack(end, alloc);

  return result;
}

} // namespace detail

} // namespace LIB

} // namespace SBG
