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

namespace LIB {

// Type definitions ------------------------------------------------------------

bool LTInter::operator()(const SetPiece &x, const SetPiece &y) const { return x < y; }

std::ostream &operator<<(std::ostream &out, const InterSet &ii) 
{
  InterSet aux = ii;
  int sz = aux.size();

  out << "{";
  if (sz > 0) {
    auto it = aux.begin();
    for (int i = 0; i < sz - 1; ++i) {
      out << *it << ", "; 
      ++it;
    }
    out << *it;
  }
  out << "}";

  return out;
}

// PWInterval ------------------------------------------------------------------

PWInterval::PWInterval() : pieces_() {}
PWInterval::PWInterval(SetPiece i) : pieces_() { pieces_ref().insert(i); }
PWInterval::PWInterval(InterSet ii) : pieces_(ii) {}

member_imp(PWInterval, InterSet, pieces);

bool PWInterval::operator==(const PWInterval &other) const
{
  PWInterval pwi1 = *this, pwi2 = other;
  if (pwi1.pieces_ref() == pwi2.pieces_ref()) return true;

  return isEmpty(difference(*this, other)) && isEmpty(difference(other, *this));
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

  BOOST_FOREACH (SetPiece i, pwi.pieces())
    result += cardinal(i);

  return result;
}

bool isEmpty(PWInterval pwi) { return pwi.pieces().empty(); }

bool isMember(NAT x, PWInterval pwi)
{
  BOOST_FOREACH (SetPiece i, pwi.pieces())
    if (isMember(x, i))
      return true;

  return false;
}

Util::NAT minElem(PWInterval pwi) 
{
  if (isEmpty(pwi))
    return 0;

  return minElem(*(pwi.pieces_ref().begin()));
}

Util::NAT maxElem(PWInterval pwi) 
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

  if (maxElem(pwi1) == minElem(pwi2)) return PWInterval(SetPiece(maxElem(pwi1), 1, maxElem(pwi1)));

  if (maxElem(pwi2) == minElem(pwi1)) return PWInterval(SetPiece(maxElem(pwi2), 1, maxElem(pwi2)));

  if (pwi1.pieces_ref() == pwi2.pieces_ref()) return pwi1;

  // General case
  InterSet cap = boundedTraverse(pwi1, pwi2, &intersection);

  return PWInterval(cap);
}

// TODO: choose better lt/gt pieces
PWInterval cup(PWInterval pwi1, PWInterval pwi2)
{
  InterSet un;

  if (isEmpty(pwi1)) return pwi2;
 
  if (isEmpty(pwi2)) return pwi1;

  if (pwi1.pieces_ref() == pwi2.pieces_ref()) return pwi1;

  if (maxElem(pwi1) <= minElem(pwi2)) {
    BOOST_FOREACH (SetPiece i1, pwi1.pieces_ref())
      un.emplace_hint(un.cend(), i1);

    BOOST_FOREACH (SetPiece i2, pwi2.pieces_ref())
      un.emplace_hint(un.cend(), i2);
  
    return PWInterval(un);
  }

  if (maxElem(pwi2) <= minElem(pwi1)) {
    BOOST_FOREACH (SetPiece i2, pwi2.pieces_ref())
      un.emplace_hint(un.cend(), i2);

    BOOST_FOREACH (SetPiece i1, pwi1.pieces_ref())
      un.emplace_hint(un.cend(), i1);

    return PWInterval(un);
  }

  // General case

  // As the complement operation will add intervals to our set, we choose the
  // one with least quantity of them.  
  PWInterval lt_pieces, gt_pieces;
  int c_size1 = 0, c_size2 = 0;
  BOOST_FOREACH (Interval i1, pwi1.pieces())
    c_size1 += i1.step();
  BOOST_FOREACH (Interval i2, pwi2.pieces())
    c_size2 += i2.step();

  if (c_size1 < c_size2) {
    lt_pieces = pwi1;
    gt_pieces = pwi2;
  }

  else {
    lt_pieces = pwi2;
    gt_pieces = pwi1;
  }

  PWInterval diff = difference(gt_pieces, lt_pieces);
  if (isCompact(lt_pieces) && isCompact(gt_pieces))
    un = traverse(lt_pieces, diff, &least);

  else {
    BOOST_FOREACH (SetPiece i, lt_pieces.pieces_ref())
      un.emplace(i);

    BOOST_FOREACH (SetPiece i, gt_pieces.pieces_ref())
      un.emplace(i);
  }

  return PWInterval(un);
}

PWInterval complement(Interval i)
{
  InterSet c;

  // Before interval
  if (i.begin() != 0) {
    SetPiece i_res(0, 1, i.begin() - 1);
    if (!isEmpty(i_res))
      c.emplace_hint(c.cend(), i_res);  
  }

  // "During" interval
  if (i.begin() < Util::Inf) {
    for (Util::NAT j = 1; j < i.step(); j++) {
      SetPiece i_res(i.begin() + j, i.step(), i.end());
      if (!isEmpty(i_res))
        c.emplace_hint(c.cend(), i_res);  
     }
  }

  // After interval
  if (maxElem(i) < Util::Inf)
    c.insert(SetPiece(maxElem(i) + 1, 1, Util::Inf));

  else 
    c.insert(SetPiece(Util::Inf));


  return c;
}

PWInterval complement(PWInterval pwi)
{
  PWInterval res;

  if (isEmpty(pwi)) return PWInterval(SetPiece(0, 1, Util::Inf));

  InterSetIt first_it = pwi.pieces_ref().begin();
  SetPiece first = *first_it;
  res = complement(first);

  ++first_it;
  InterSet second(first_it, pwi.pieces_ref().end());
  BOOST_FOREACH (Interval i, second) {
    PWInterval c = complement(i);
    res = intersection(res, c);
  }

  return res;
}

PWInterval difference(PWInterval pwi1, PWInterval pwi2) { return intersection(pwi1, complement(pwi2)); }

// Extra operations ------------------------------------------------------------

bool isCompact(PWInterval pwi)
{
  BOOST_FOREACH (SetPiece i, pwi.pieces_ref())
    if (i.step() != 1)
      return false;

  return true;
}

InterSet boundedTraverse(PWInterval pwi1, PWInterval pwi2, SetPiece (*func)(SetPiece, SetPiece))
{
  InterSet result;

  if (isEmpty(pwi1) || isEmpty(pwi2)) return result;

  if (isCompact(pwi1) && isCompact(pwi2)) {
    InterSetIt it1 = pwi1.pieces_ref().begin(), it2 = pwi2.pieces_ref().begin();   
    InterSetIt end1 = pwi1.pieces_ref().end(), end2 = pwi2.pieces_ref().end();   

    SetPiece i1, i2;
    for (int j = 0; it1 != end1 && it2 != end2; j++) {
      i1 = *it1;
      i2 = *it2;

      SetPiece funci = func(i1, i2);
      if (!isEmpty(funci))
        result.emplace_hint(result.cend(), funci);

      if (maxElem(i1) < maxElem(i2)) ++it1;

      else ++it2;
    }
  }

  else {
    BOOST_FOREACH (SetPiece i1, pwi1.pieces_ref())
      BOOST_FOREACH (SetPiece i2, pwi2.pieces_ref()) {
        SetPiece funci = func(i1, i2);
        if (!isEmpty(funci))
          result.emplace(funci);
      }
  }

  return result;
}

InterSet traverse(PWInterval pwi1, PWInterval pwi2, SetPiece (*func)(SetPiece, SetPiece))
{
  InterSet result;

  if (isEmpty(pwi1) || isEmpty(pwi2)) return result;

  InterSetIt it1 = pwi1.pieces_ref().begin(), it2 = pwi2.pieces_ref().begin();   
  InterSetIt end1 = pwi1.pieces_ref().end(), end2 = pwi2.pieces_ref().end();   

  SetPiece i1, i2;
  for (; it1 != end1 && it2 != end2;) {
    i1 = *it1;
    i2 = *it2;

    SetPiece funci = func(i1, i2);
    if (!isEmpty(funci))
      result.emplace_hint(result.cend(), funci);

    if (maxElem(i1) < maxElem(i2))
      ++it1;

    else
      ++it2;
  }

  for (; it1 != end1; ++it1) {
    i1 = *it1;
    result.emplace_hint(result.cend(), i1);
  }

  for (; it2 != end2; ++it2) {
    i2 = *it2;
    result.emplace_hint(result.cend(), i2);
  }

  return result;
}

} // namespace LIB

} // namespace SBG
