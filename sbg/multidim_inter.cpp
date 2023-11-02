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
  BOOST_FOREACH (NAT xi, x)
    emplaceBack(Interval(xi, 1, xi));
}
MultiDimInter::MultiDimInter(Interval i) : intervals_() { emplaceBack(i); }
MultiDimInter::MultiDimInter(unsigned int nmbr_copies, Interval i) : intervals_() { 
  for (unsigned int j = 0; j < nmbr_copies; ++j) emplaceBack(i);
}
MultiDimInter::MultiDimInter(InterVector iv) : intervals_(iv) {}

member_imp(MultiDimInter, InterVector, intervals);

MultiDimInter::iterator MultiDimInter::begin() { return intervals_ref().begin(); }
MultiDimInter::iterator MultiDimInter::end() { return intervals_ref().end(); }

std::size_t MultiDimInter::size() const { return intervals().size(); }

void MultiDimInter::emplaceBack(Interval i) 
{
  if (!isEmpty(i)) intervals_ref().emplace_back(i);

  else {
    InterVector aux;
    set_intervals(aux);
  }
}

Interval &MultiDimInter::operator[](std::size_t n) { return intervals_ref()[n]; }

bool MultiDimInter::operator==(const MultiDimInter &other) const { return intervals() == other.intervals(); }

bool MultiDimInter::operator!=(const MultiDimInter &other) const { return !(*this == other); }

bool MultiDimInter::operator<(const MultiDimInter &other) const
{
  MultiDimInter aux1 = *this, aux2 = other;

  if (isEmpty(aux1)) return true;

  if (isEmpty(aux2)) return false;

  if (aux1.size() == aux2.size()) {
    parallel_foreach2(aux1.intervals_ref(), aux2.intervals_ref()) {
      Interval i1 = boost::get<0>(items), i2 = boost::get<1>(items);
      if (i1 < i2) return true;

      else if (i2 < i1) return false;
    }

    return false;
  }


  Util::ERROR("MultiDimInter operator<: comparing multi-dimensional intervals of different dimensions");
  return false;
}

std::ostream &operator<<(std::ostream &out, const MultiDimInter &mdi) 
{
  InterVector iv = mdi.intervals();
  int sz = iv.size();

  if (sz > 0) {
    auto it = iv.begin();
    for (int i = 0; i < sz - 1; ++i) {
      out << *it << " x ";
      ++it;
    }
    out << *it;
  }
 
  return out;
}

// Set functions ---------------------------------------------------------------

unsigned int cardinal(MultiDimInter mdi)
{
  unsigned int res = 1;

  BOOST_FOREACH (Interval i, mdi)
    res *= cardinal(i);

  return res;
}

bool isEmpty(MultiDimInter mdi) { return mdi.intervals().empty(); }

bool isMember(MD_NAT x, MultiDimInter mdi)
{
  if (isEmpty(mdi)) return false;

  if (x.size() == mdi.intervals().size()) {
    parallel_foreach2 (x, mdi.intervals_ref()) {
      NAT xi = boost::get<0>(items);
      Interval ii = boost::get<1>(items);

      if (!isMember(xi, ii)) return false;
    }

    return true;
  }

  Util::ERROR("LIB::MultiDimInter::isMember: dimensions don't match");
  return false;
}

Util::MD_NAT minElem(MultiDimInter mdi)
{
  MD_NAT res;

  BOOST_FOREACH (Interval i, mdi)
    res.emplace_back(minElem(i));

  return res;
}

Util::MD_NAT maxElem(MultiDimInter mdi)
{
  MD_NAT res;

  BOOST_FOREACH (Interval i, mdi)
    res.emplace_back(maxElem(i));

  return res;
}

MultiDimInter intersection(MultiDimInter mdi1, MultiDimInter mdi2)
{
  if (isEmpty(mdi1) || isEmpty(mdi2)) return MultiDimInter();

  if (mdi1.size() == mdi2.size()) {
    MultiDimInter res;

    parallel_foreach2 (mdi1.intervals_ref(), mdi2.intervals_ref()) {
      Interval i1 = boost::get<0>(items), i2 = boost::get<1>(items);
      Interval ith = intersection(i1, i2);
      if (!isEmpty(ith)) res.emplaceBack(intersection(i1, i2));

      else return MultiDimInter();
    }

    return res;
  }

  Util::ERROR("LIB::MultiDimInter::intersection: dimensions don't match");
  return MultiDimInter();
}

// Extra operations ------------------------------------------------------------

MultiDimInter offset(Util::MD_NAT off, MultiDimInter mdi)
{
  MultiDimInter res;

  parallel_foreach2 (off.value_ref(), mdi.intervals_ref()) {
    Util::NAT o = boost::get<0>(items);
    Interval i = boost::get<1>(items);
    res.emplaceBack(offset(o, i));
  }

  return res;
}

MultiDimInter least(MultiDimInter mdi1, MultiDimInter mdi2) { return std::min(mdi1, mdi2); }

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
