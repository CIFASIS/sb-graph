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
  Util::ERROR_UNLESS(!isEmpty(i), "LIB::MDI2: empty not allowed");
  intervals_.emplace_back(i);
}
MultiDimInter::MultiDimInter(unsigned int nmbr_copies
                             , Interval i) : intervals_() { 
  Util::ERROR_UNLESS(!isEmpty(i), "LIB::MDI3: empty not allowed");
  for (unsigned int j = 0; j < nmbr_copies; ++j)
    intervals_.emplace_back(i);
}
MultiDimInter::MultiDimInter(InterVector iv) : intervals_(iv) {}

member_imp(MultiDimInter, InterVector, intervals);

MultiDimInter::iterator MultiDimInter::begin() { return intervals_.begin(); }
MultiDimInter::iterator MultiDimInter::end() { return intervals_.end(); }

std::size_t MultiDimInter::size() const { return intervals_.size(); }

void MultiDimInter::emplaceBack(Interval i) 
{
  Util::ERROR_UNLESS(!isEmpty(i), "LIB::MDI::emplaceBack: empty not allowed");
  intervals_.emplace_back(i);
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
  return intervals() == other.intervals();
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

  MultiDimInter aux1 = *this, aux2 = other;

  if (isEmpty(aux1)) return true;

  if (isEmpty(aux2)) return false;

  for (auto const &[i1, i2] : boost::combine(aux1, aux2)) {
    if (i1 < i2) return true;

    else if (i2 < i1) return false;
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

unsigned int cardinal(MultiDimInter mdi)
{
  unsigned int res = 1;

  for (Interval i : mdi) res *= cardinal(i);

  return res;
}

bool isEmpty(MultiDimInter mdi) { return mdi.intervals().empty(); }

bool isMember(MD_NAT x, MultiDimInter mdi)
{
  Util::ERROR_UNLESS(x.size() == mdi.size()
               , "LIB::MDI::isMember: dimensions don't match"); 

  if (isEmpty(mdi)) return false;

  for (auto const &[xi, ii] : boost::combine(x, mdi))
    if (!isMember(xi, ii)) return false;

  return true;
}

Util::MD_NAT minElem(MultiDimInter mdi)
{
  Util::ERROR_UNLESS(!isEmpty(mdi), "LIB::MDI::minElem: shouldn't be empty");

  MD_NAT res;

  for (Interval i : mdi) 
    res.emplace_back(minElem(i));

  return res;
}

Util::MD_NAT maxElem(MultiDimInter mdi)
{
  Util::ERROR_UNLESS(!isEmpty(mdi), "LIB::MDI::maxElem: shouldn't be empty");

  MD_NAT res;

  for (Interval i : mdi)
    res.emplace_back(maxElem(i));

  return res;
}

MultiDimInter intersection(MultiDimInter mdi1, MultiDimInter mdi2)
{
  Util::ERROR_UNLESS(mdi1.size() == mdi2.size()
               , "LIB::MDI::intersection: dimensions don't match");

  if (isEmpty(mdi1) || isEmpty(mdi2))
    return MultiDimInter();

  MultiDimInter res;

  for (auto const &[i1, i2] : boost::combine(mdi1, mdi2)) {
    Interval ith = intersection(i1, i2);
    if (!isEmpty(ith))
      res.emplaceBack(ith);

    else
      return MultiDimInter();
  }

  return res;
}

// Extra operations ------------------------------------------------------------

MultiDimInter offset(Util::MD_NAT off, MultiDimInter mdi)
{
  Util::ERROR_UNLESS(off.size() == mdi.size()
               , "LIB::MDI::offset: dimensions don't match");

  MultiDimInter res;

  for (auto const &[o, i] : boost::combine(off, mdi))
    res.emplaceBack(offset(o, i));

  return res;
}

MultiDimInter least(MultiDimInter mdi1, MultiDimInter mdi2)
{
  return std::min(mdi1, mdi2);
}

bool isUnidim(MultiDimInter mdi) { return mdi.size() <= 1; }

std::size_t hash_value(const MultiDimInter &mdi)
{
  std::size_t seed = 0;

  MultiDimInter aux = mdi;  
  boost::hash_range(seed, aux.begin(), aux.end());

  return seed;
}

} // namespace LIB

} // namespace SBG
