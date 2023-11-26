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

namespace SBG {

namespace LIB {

Interval::Interval() : begin_(1), step_(1), end_(0) {}
Interval::Interval(NAT x) : begin_(x), step_(1), end_(x) {}
Interval::Interval(NAT begin, NAT step, NAT end) 
  : begin_(begin), step_(step), end_(end) 
{
  if (end >= begin) {
    int rem = fmod(end - begin, step);
    end_ = end - rem;

    if (cardinal() == 1)
      step_ = 1;
  }

  else {
    begin_ = 1;
    step_ = 1;
    end_ = 0;
  }
}

member_imp(Interval, NAT, begin);
member_imp(Interval, NAT, step);
member_imp(Interval, NAT, end);

// Operators -------------------------------------------------------------------

bool Interval::operator==(const Interval &other) const
{
  return (begin_ == other.begin_)
          && (step_ == other.step_)
          && (end_ == other.end_);
}

bool Interval::operator!=(const Interval &other) const
{
  return !(*this == other);
}

bool Interval::operator<(const Interval &other) const
{
  if (begin_ < other.begin_)
    return true;

  if (begin_ == other.begin_ && end_ < other.end_)
    return true;

  if (begin_ == other.begin_
      && end_ == other.end_
      && step_ < other.step_)
    return true;

  return false;
}

std::ostream &operator<<(std::ostream &out, const Interval &i) 
{
  out << "[" << Util::toStr(i.begin());
  if (i.step_ != 1)
    out << ":" << Util::toStr(i.step());
  out << ":" << Util::toStr(i.end()) << "]";
 
  return out;
}

// Set functions ---------------------------------------------------------------

unsigned int Interval::cardinal() const
{
  return (end_ - begin_) / step_ + 1;
}

bool Interval::isEmpty() const { return end_ < begin_; }

bool Interval::isMember(NAT x) const
{
  if (x < begin_ || x > end_)
    return false;

  int rem = fmod(x - begin_, step_);

  return rem == 0;
}

Util::NAT Interval::minElem() const { return begin_; }

Util::NAT Interval::maxElem() const { return end_; }

Interval Interval::intersection(const Interval &other)const
{
  if (isEmpty() || other.isEmpty())
    return Interval();

  if ((end_ < other.begin_) || (other.end_ < begin_))
    return Interval();

  // Two non overlapping intervals with the same step
  if (step_ == other.step_
      && !other.isMember(begin_)
      && !isMember(other.begin_))
    return Interval();

  NAT max_begin = std::max(begin_, other.begin_);
  NAT new_step = std::lcm(step_, other.step_)
  , new_begin = max_begin
  , new_end = std::min(end_, other.end_);
  bool found_member = false;
  for (NAT x = max_begin; x < max_begin + new_step; ++x)
    if (isMember(x) && other.isMember(x)) {
      new_begin = x;
      found_member = true;
    }

  if (found_member)
    return Interval(new_begin, new_step, new_end);

  return Interval(1, 1, 0);
}

// Extra operations ------------------------------------------------------------

Interval Interval::offset(Util::NAT off) const
{
  Util::NAT new_b = begin_ + off, new_e = end_ + off;

  return Interval(new_b, step_, new_e);
}

Interval Interval::least(const Interval &other) const
{
  return std::min(*this, other);
}

std::size_t hash_value(const Interval &i)
{
  std::size_t seed = 0;
  boost::hash_combine(seed, i.begin());
  boost::hash_combine(seed, i.step());
  boost::hash_combine(seed, i.end());

  return seed;
}

} // namespace LIB

} // namespace SBG
