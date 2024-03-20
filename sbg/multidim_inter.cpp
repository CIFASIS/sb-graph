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

#include <iostream>
#include "sbg/multidim_inter.hpp"

namespace SBG {

namespace LIB {

MultiDimInter::MultiDimInter() : intervals_() {}
MultiDimInter::MultiDimInter(MD_NAT x) : intervals_() {
  Util::ERROR_UNLESS(x.size() > 0, "LIB::MDI1: empty not allowed");
  for (NAT xi : x)
    intervals_.emplace_back(Interval(xi, 1, xi));
}
MultiDimInter::MultiDimInter(Interval i) : intervals_()
{
  Util::ERROR_UNLESS(!i.isEmpty(), "LIB::MDI2: empty not allowed");
  intervals_.emplace_back(i);
}
MultiDimInter::MultiDimInter(unsigned int nmbr_copies
                             , Interval i) : intervals_() {
  Util::ERROR_UNLESS(!i.isEmpty(), "LIB::MDI3: empty not allowed");
  for (unsigned int j = 0; j < nmbr_copies; ++j)
    intervals_.emplace_back(i);
}
MultiDimInter::MultiDimInter(InterVector iv) : intervals_(iv) {}

member_imp(MultiDimInter, InterVector, intervals);

MultiDimInter::iterator MultiDimInter::begin() { return intervals_.begin(); }
MultiDimInter::iterator MultiDimInter::end() { return intervals_.end(); }
MultiDimInter::const_iterator MultiDimInter::begin() const
{
  return intervals_.begin();
}
MultiDimInter::const_iterator MultiDimInter::end() const
{
  return intervals_.end();
}

std::size_t MultiDimInter::size() const { return intervals_.size(); }

void MultiDimInter::emplaceBack(Interval i) 
{
  if (i.isEmpty())
    intervals_ = InterVector();
  else
    intervals_.emplace_back(i);
  return;
}

Interval &MultiDimInter::operator[](std::size_t n)
{
  Util::ERROR_UNLESS(n < intervals_.size() && intervals_.size() > 0
                     , "LIB::MDI::operator[]: invalid n");
  return intervals_[n];
}

const Interval &MultiDimInter::operator[](std::size_t n) const
{
  Util::ERROR_UNLESS(n < intervals_.size() && intervals_.size() > 0
                     , "LIB::MDI::operator[]: invalid n");
  return intervals_[n];
}

bool MultiDimInter::operator==(const MultiDimInter &other) const
{
  Util::ERROR_UNLESS(size() == other.size()
      , "LIB::MDI::operator==: dimensions don't match");
  return intervals_ == other.intervals_;
}

bool MultiDimInter::operator!=(const MultiDimInter &other) const
{
  Util::ERROR_UNLESS(size() == other.size()
      , "LIB::MDI::operator!=: dimensions don't match");
  return !(*this == other);
}

bool MultiDimInter::operator<(const MultiDimInter &other) const
{
  Util::ERROR_UNLESS(size() == other.size()
      , "LIB::MDI::operator<: dimensions don't match");

  if (isEmpty())
    return true;

  if (other.isEmpty())
    return false;

  for (unsigned int j = 0; j < size(); ++j) {
    if (operator[](j) < other[j])
      return true;
    else if (other[j] < operator[](j))
      return false;
  }

  return false;
}

std::ostream &operator<<(std::ostream &out, const MultiDimInter &mdi)
{
  std::size_t sz = mdi.size();

  if (sz > 0) {
    for (std::size_t j = 0; j < sz - 1; ++j)
      out << mdi[j] << "x";
    out << mdi[sz-1];
  }

  return out;
}

// Set functions ---------------------------------------------------------------

unsigned int MultiDimInter::cardinal() const
{
  unsigned int res = 1;

  for (const Interval &i : intervals_)
    res *= i.cardinal();

  return res;
}

bool MultiDimInter::isEmpty() const { return intervals_.empty(); }

bool MultiDimInter::isMember(const MD_NAT &x) const
{
  Util::ERROR_UNLESS(x.size() == size()
      , "LIB::MDI::isMember: dimensions don't match");

  if (isEmpty())
    return false;

  for (unsigned int j = 0; j < size(); ++j)
    if (!operator[](j).isMember(x[j]))
      return false;

  return true;
}
Util::MD_NAT MultiDimInter::minElem() const
{
  Util::ERROR_UNLESS(!isEmpty(), "LIB::MDI::minElem: shouldn't be empty");

  MD_NAT res;

  for (const Interval &i : intervals_)
    res.emplaceBack(i.minElem());

  return res;
}

Util::MD_NAT MultiDimInter::maxElem() const
{
  Util::ERROR_UNLESS(!isEmpty(), "LIB::MDI::maxElem: shouldn't be empty");

  MD_NAT res;

  for (const Interval &i : intervals_)
    res.emplaceBack(i.maxElem());

  return res;
}

MultiDimInter MultiDimInter::intersection(const MultiDimInter &other) const
{
  Util::ERROR_UNLESS(size() == other.size()
    , "LIB::MDI::intersection: dimensions don't match");

  if (isEmpty() || other.isEmpty())
    return MultiDimInter();

  MultiDimInter res;

  for (unsigned int j = 0; j < size(); ++j) {
    Interval ith = operator[](j).intersection(other[j]);
    if (!ith.isEmpty())
      res.emplaceBack(ith);

    else
      return MultiDimInter();
  }

  return res;
}

// Extra operations ------------------------------------------------------------

MultiDimInter MultiDimInter::offset(const MD_NAT &off) const
{
  Util::ERROR_UNLESS(off.size() == size()
      , "LIB::MDI::offset: dimensions don't match");

  MultiDimInter res;

  for (unsigned int j = 0; j < size(); ++j)
    res.emplaceBack(operator[](j).offset(off[j]));

  return res;
}

MultiDimInter MultiDimInter::least(const MultiDimInter &other) const
{
  return std::min(*this, other);
}

bool MultiDimInter::isUnidim() const { return size() <= 1; }

Util::MD_NAT MultiDimInter::midElem() const
{
  Util::MD_NAT res;

  for (unsigned int j = 0; j < size(); ++j)
    res.emplaceBack(operator[](j).midElem());

  return res;
}

MultiDimInter::MaybeMDI MultiDimInter::compact(const MultiDimInter &other) const
{
  MultiDimInter res;

  unsigned int j = 0;
  for (; j < size(); ++j) {
    auto ith = operator[](j).compact(other[j]);
    if (ith) {
      res.emplaceBack(ith.value());
      ++j;
      break;
    }

    else if (operator[](j) != other[j])
      return {};

    else
      res.emplaceBack(operator[](j));
  }

  for (; j < size(); ++j) {
    if (operator[](j) != other[j])
      return {};

    else
      res.emplaceBack(operator[](j));
  }

  return res;
}

std::size_t hash_value(const MultiDimInter &mdi)
{
  return boost::hash_range(mdi.begin(), mdi.end());
}

} // namespace LIB

} // namespace SBG
