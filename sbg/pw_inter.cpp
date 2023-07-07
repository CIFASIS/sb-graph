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

#include <sbg/pw_inter.hpp>

namespace SBG {

// Type definitions ------------------------------------------------------------

std::ostream &operator<<(std::ostream &out, const InterSet &ii) 
{
  out << "{";
  if (ii.size() > 0) {
    BOOST_FOREACH (Interval i, ii)
      out << i << ", "; 
    out << "\b\b";
  }
  out << "}";

  return out;
}

// PWInterval ------------------------------------------------------------------

PWInterval::PWInterval() : pieces_() {}
PWInterval::PWInterval(Interval i) : pieces_() {
  pieces_ref().insert(i);
}
PWInterval::PWInterval(InterSet ii) : pieces_(ii) {}

member_imp(PWInterval, InterSet, pieces);

bool PWInterval::operator==(const PWInterval &other) const
{
  PWInterval pwi1 = canonize(*this), pwi2 = canonize(other);
  parallel_foreach2 (pwi1.pieces_ref(), pwi2.pieces_ref()) {
    Interval i1 = boost::get<0>(items), i2 = boost::get<1>(items);
    if (!(i1 == i2))
      return false;
  }

  return true;
}

std::ostream &operator<<(std::ostream &out, const PWInterval &pwi) 
{
  out << pwi.pieces();
 
  return out;
}

// Set functions ---------------------------------------------------------------

unsigned int cardinal(PWInterval pwi) 
{
  unsigned int result = 0;

  BOOST_FOREACH (Interval i, pwi.pieces())
    result += cardinal(i);

  return result;
}

bool isEmpty(PWInterval pwi) { return pwi.pieces().empty(); }

bool isMember(INT x, PWInterval pwi)
{
  BOOST_FOREACH (Interval i, pwi.pieces())
    if (isMember(x, i))
      return true;

  return false;
}

Util::INT minElem(PWInterval pwi) 
{
  if (isEmpty(pwi))
    return 0;

  return minElem(*(pwi.pieces_ref().begin()));
}

Util::INT maxElem(PWInterval pwi) 
{
  if (isEmpty(pwi))
    return 0;

  InterSetIt penult = pwi.pieces_ref().end();
  --penult;
  return maxElem(*penult);
}

PWInterval intersection(PWInterval pwi1, PWInterval pwi2)
{
  // Special cases to enhance performance
  if (isEmpty(pwi1)) return PWInterval();

  if (isEmpty(pwi2)) return PWInterval();

  if (maxElem(pwi1) < minElem(pwi2)) return PWInterval();

  if (maxElem(pwi2) < minElem(pwi1)) return PWInterval();

  if (maxElem(pwi1) == minElem(pwi2)) return PWInterval(Interval(maxElem(pwi1), 1, maxElem(pwi1)));

  if (maxElem(pwi2) == minElem(pwi1)) return PWInterval(Interval(maxElem(pwi2), 1, maxElem(pwi2)));

  // General case
  InterSet cap = linearTraverse(pwi1, pwi2, &intersection);

  return PWInterval(cap);
}

PWInterval complement(PWInterval pwi)
{
  InterSet c;

  if (isEmpty(pwi)) return PWInterval(Interval(0, 1, Util::Inf));

  Util::INT last_end = 0;
  BOOST_FOREACH (Interval i, pwi.pieces()){
    // Before interval
    if (i.begin() != 0)
      c.insert(Interval(last_end, 1, i.begin() - 1));  

    // "During" interval
    if (i.begin() < Util::Inf)
      for (Util::INT j = 1; j < i.step(); j++)
        c.insert(Interval(i.begin() + j, i.step(), i.end()));

    last_end = i.end() + 1;
  }

  // After intervals
  last_end = maxElem(pwi); 
  if (last_end < Util::Inf)
    c.insert(Interval(last_end + 1, 1, Util::Inf));

  else 
    c.insert(Interval(Util::Inf, 1, Util::Inf));

  return PWInterval(c);
}

PWInterval difference(PWInterval pwi1, PWInterval pwi2) { return intersection(pwi1, complement(pwi2)); }

// Extra operations ------------------------------------------------------------

InterSet linearTraverse(PWInterval pwi1, PWInterval pwi2, Interval (*func)(Interval, Interval))
{
  InterSet result;

  InterSetIt it1 = pwi1.pieces_ref().begin(), it2 = pwi2.pieces_ref().begin();   
  InterSetIt end1 = pwi1.pieces_ref().end(), end2 = pwi2.pieces_ref().end();   

  Interval i1, i2;
  for (; it1 != end1 && it2 != end2;) {
    i1 = *it1;
    i2 = *it2;

    Interval funci = func(i1, i2);
    if (!isEmpty(funci))
      result.emplace_hint(result.cend(), funci);

    if (maxElem(i1) < maxElem(i2))
      ++it1;

    else
      ++it2;
  }

  return result;
}

// TODO: {[1:2:99], [2:2:100]} will not be canonized, but in theory
// this should be possible.
PWInterval canonize(PWInterval pwi) 
{
  if (isEmpty(pwi)) return pwi;

  if (pwi.pieces().size() == 1) return pwi;

  InterSet canon;
  InterSetIt ith = pwi.pieces_ref().begin();
  canon.emplace_hint(canon.cbegin(), *ith);
  ++ith;
  InterSetIt cit = canon.begin();

  for (; ith != pwi.pieces_ref().end(); ++ith) {
    MaybeInterval i = canonize(*cit, *ith);
    if (i) {
      canon.erase(cit);
      cit = canon.emplace_hint(canon.cend(), *i);
    }

    else 
      cit = canon.emplace_hint(canon.cend(), *ith);
  }

  return PWInterval(canon);
}

} // namespace SBG
