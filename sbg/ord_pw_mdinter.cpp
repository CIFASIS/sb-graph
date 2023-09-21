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

#include "sbg/ord_pw_mdinter.hpp"

namespace SBG {

namespace LIB {

// Type definitions ------------------------------------------------------------

LTMDInter::LTMDInter() {}

bool LTMDInter::operator()(const SetPiece &x, const SetPiece &y) const { return x < y; }

std::ostream &operator<<(std::ostream &out, const MDInterOrdSet &ii) 
{
  MDInterOrdSet aux = ii;
  int sz = aux.size();

  out << "{";
  if (sz > 0) {
    auto it = aux.begin();
    for (int i = 0; i < sz - 1; ++i) {
      if (!isEmpty(*it))
        out << *it << ", "; 
      ++it;
    }
    if (!isEmpty(*it))
      out << *it;
  }
  out << "}";

  return out;
}

// OrdPWMDInter ------------------------------------------------------------------

OrdPWMDInter::OrdPWMDInter() : pieces_() {}
OrdPWMDInter::OrdPWMDInter(Interval i) : pieces_() { pieces_ref().insert(SetPiece(i)); }
OrdPWMDInter::OrdPWMDInter(SetPiece mdi) : pieces_() { 
  if (isUnidim(mdi)) pieces_ref().insert(mdi);

  else Util::ERROR("LIB::OrdPWMDInter: piece should be uni-dimensional");
}
OrdPWMDInter::OrdPWMDInter(MDInterOrdSet ii) : pieces_() {
  if (!ii.empty()) {
    BOOST_FOREACH (SetPiece mdi, ii)
      if (!isUnidim(mdi)) Util::ERROR("LIB::OrdPWMDInter: piece should be uni-dimensional");

    set_pieces(ii);
  }

}

member_imp(OrdPWMDInter, MDInterOrdSet, pieces);

std::size_t OrdPWMDInter::size() { return pieces().size(); }

void OrdPWMDInter::emplace(SetPiece mdi) { pieces_ref().emplace(mdi); }
void OrdPWMDInter::emplace_hint(MDInterOrdSetIt it, SetPiece mdi) { pieces_ref().emplace_hint(it, mdi); }

OrdPWMDInter::iterator OrdPWMDInter::begin() { return pieces_ref().begin(); }
OrdPWMDInter::iterator OrdPWMDInter::end() { return pieces_ref().end(); }

SetPiece OrdPWMDInter::operator[](std::size_t n) { return *(pieces().nth(n)); }

bool OrdPWMDInter::operator==(const OrdPWMDInter &other) const
{
  MDInterOrdSet aux1 = this->pieces(), aux2 = other.pieces();
  if (optConds(aux1) && optConds(aux2)) {
    MDInterOrdSet ii1 = canonize(aux1), ii2 = canonize(aux2);
    return ii1 == ii2;
  }

  OrdPWMDInter pwi1 = *this, pwi2 = other;
  if (pwi1.pieces_ref() == pwi2.pieces_ref()) return true;

  return isEmpty(difference(*this, other)) && isEmpty(difference(other, *this));
}

bool OrdPWMDInter::operator<(const OrdPWMDInter &other) const { return minElem(*this) < minElem(other); }

std::ostream &operator<<(std::ostream &out, const OrdPWMDInter &pwi) 
{
  out << pwi.pieces();
 
  return out;
}

// Set functions ---------------------------------------------------------------

unsigned int cardinal(OrdPWMDInter pwi) 
{
  unsigned int result = 0;

  BOOST_FOREACH (SetPiece mdi, pwi)
    result += cardinal(mdi);

  return result;
}

bool isEmpty(OrdPWMDInter pwi) { return pwi.pieces().empty(); }

bool isMember(MD_NAT x, OrdPWMDInter pwi)
{
   BOOST_FOREACH (SetPiece mdi, pwi) 
     if (isMember(x, mdi)) return true;

  return false;
}

Util::MD_NAT minElem(OrdPWMDInter pwi) 
{
  if (isEmpty(pwi))
    return MD_NAT(0);

  return minElem(*(pwi.begin()));
}

Util::MD_NAT maxElem(OrdPWMDInter pwi) 
{
  if (isEmpty(pwi))
    return MD_NAT(0);

  MDInterOrdSetIt penult = pwi.end();
  --penult;
  return maxElem(*penult);
}

OrdPWMDInter intersection(OrdPWMDInter pwi1, OrdPWMDInter pwi2)
{
  // Special cases to enhance performance
  if (isEmpty(pwi1)) return OrdPWMDInter();

  if (isEmpty(pwi2)) return OrdPWMDInter();

  if (maxElem(pwi1) < minElem(pwi2)) return OrdPWMDInter();

  if (maxElem(pwi2) < minElem(pwi1)) return OrdPWMDInter();

  if (maxElem(pwi1) == minElem(pwi2)) return OrdPWMDInter(SetPiece(maxElem(pwi1)));

  if (maxElem(pwi2) == minElem(pwi1)) return OrdPWMDInter(SetPiece(maxElem(pwi2)));

  if (pwi1.pieces_ref() == pwi2.pieces_ref()) return pwi1;

  // General case
  MDInterOrdSet cap = boundedTraverse(pwi1, pwi2, &intersection);

  return OrdPWMDInter(cap);
}

OrdPWMDInter cup(OrdPWMDInter pwi1, OrdPWMDInter pwi2)
{
  OrdPWMDInter un;

  if (isEmpty(pwi1)) return pwi2;
 
  if (isEmpty(pwi2)) return pwi1;

  if (pwi1.pieces_ref() == pwi2.pieces_ref()) return pwi1;

  if (maxElem(pwi1) <= minElem(pwi2)) {
    BOOST_FOREACH (SetPiece mdi1, pwi1)
      un.emplace_hint(un.end(), mdi1);

    BOOST_FOREACH (SetPiece mdi2, pwi2)
      un.emplace_hint(un.end(), mdi2);
    
    return OrdPWMDInter(un);
  }

  if (maxElem(pwi2) <= minElem(pwi1)) {
    BOOST_FOREACH (SetPiece mdi2, pwi2)
      un.emplace_hint(un.end(), mdi2);

    BOOST_FOREACH (SetPiece mdi1, pwi1)
      un.emplace_hint(un.end(), mdi1);

    return OrdPWMDInter(un);
  }

  // General case

  // As the complement operation will add intervals to our set, we choose the
  // one with least quantity of them.  
  OrdPWMDInter lt_pieces, gt_pieces;
  int c_size1 = 0, c_size2 = 0;
  BOOST_FOREACH (SetPiece mdi1, pwi1) 
    c_size1 += mdi1.begin()->step();
  BOOST_FOREACH (SetPiece mdi2, pwi2)
    c_size2 += mdi2.begin()->step();

  if (c_size1 < c_size2) {
    lt_pieces = pwi1;
    gt_pieces = pwi2;
  }

  else {
    lt_pieces = pwi2;
    gt_pieces = pwi1;
  }

  OrdPWMDInter diff = difference(gt_pieces, lt_pieces);
  if (optConds(lt_pieces.pieces()) && optConds(gt_pieces.pieces()))
    return concatenation(lt_pieces, diff);

  else {
    BOOST_FOREACH (SetPiece mdi, lt_pieces)
      un.emplace(mdi);

    BOOST_FOREACH (SetPiece mdi, diff)
      un.emplace(mdi);
  }

  return un;
}

OrdPWMDInter complementAtom(OrdPWMDInter pwi)
{
  OrdPWMDInter c;

  if (pwi.size() == 1) {  
    Interval i = *(pwi.begin()->intervals().begin());

    // Before interval
    if (i.begin() != 0) {
      Interval i_res(0, 1, i.begin() - 1);
      if (!isEmpty(i_res))
        c.emplace_hint(c.end(), SetPiece(i_res));  
    }

    // "During" interval
    if (i.begin() < Util::Inf) {
      for (Util::NAT j = 1; j < i.step(); j++) {
        Interval i_res(i.begin() + j, i.step(), i.end());
        if (!isEmpty(i_res))
          c.emplace_hint(c.end(), SetPiece(i_res));  
       }
    }

    // After interval
    if (maxElem(i) < Util::Inf)
      c.emplace_hint(c.end(), SetPiece(Interval(maxElem(i) + 1, 1, Util::Inf)));

    else 
      c.emplace_hint(c.end(), SetPiece(Interval(Util::Inf)));
  }
  
  else Util::ERROR("LIB::OrdPWMDInter:complementAtom: argument should have only one element");

  return c;
}

OrdPWMDInter complement(OrdPWMDInter pwi)
{
  OrdPWMDInter res;

  if (isEmpty(pwi)) return OrdPWMDInter(Interval(0, 1, Util::Inf));

  MDInterOrdSetIt first_it = pwi.begin();
  SetPiece first = *first_it;
  res = complementAtom(OrdPWMDInter(first));

  ++first_it;
  MDInterOrdSet second(first_it, pwi.end());
  BOOST_FOREACH (SetPiece mdi, second) {
    OrdPWMDInter c = complementAtom(OrdPWMDInter(mdi));
    res = intersection(res, c);
  }

  return res;
}

OrdPWMDInter difference(OrdPWMDInter pwi1, OrdPWMDInter pwi2) { return intersection(pwi1, complement(pwi2)); }

// Extra operations ------------------------------------------------------------

bool isCompact(MDInterOrdSet ii)
{
  BOOST_FOREACH (SetPiece mdi, ii)
    if (mdi.begin()->step() != 1)
      return false;

  return true;
}

bool optConds(MDInterOrdSet ii) { return isCompact(ii); }

MDInterOrdSet canonize(MDInterOrdSet ii)
{
  MDInterOrdSet res;

  unsigned int sz = ii.size();
  if (sz == 1) res = ii;

  if (optConds(ii) && sz > 1) {
    MDInterOrdSetIt it = ii.begin(), next_it = it;
    ++next_it;
    SetPiece ith = *it;
    for (unsigned int j = 0; j < sz - 1; ++j) {
      MaybeInterval i = canonize(*ith.begin(), *next_it->begin()); 
      if (i) ith = SetPiece(*i);

      else {
        if (!isEmpty(ith)) res.emplace_hint(res.cend(), ith);

        if (j < (sz-2)) ith = SetPiece(*next_it);

        else ith = SetPiece(*next_it);
      }

      ++next_it;
      ++it;
    }
    if (!isEmpty(ith)) res.emplace_hint(res.cend(), ith);
  }

  else res = ii;

  return res;
}

OrdPWMDInter concatenation(OrdPWMDInter pwi1, OrdPWMDInter pwi2)
{
  OrdPWMDInter res;

  if (optConds(pwi1.pieces()) && optConds(pwi2.pieces())) res = traverse(pwi1, pwi2, &least);

  else {
    BOOST_FOREACH (SetPiece mdi1, pwi1) res.emplace(mdi1);

    BOOST_FOREACH (SetPiece mdi2, pwi2) res.emplace(mdi2);
  }

  return res;
}

MDInterOrdSet boundedTraverse(OrdPWMDInter pwi1, OrdPWMDInter pwi2, SetPiece (*func)(SetPiece, SetPiece))
{
  MDInterOrdSet result;

  if (isEmpty(pwi1) || isEmpty(pwi2)) return result;

  if (optConds(pwi1.pieces()) && optConds(pwi2.pieces())) {
    MDInterOrdSetIt it1 = pwi1.begin(), it2 = pwi2.begin();   
    MDInterOrdSetIt end1 = pwi1.end(), end2 = pwi2.end();   

    SetPiece mdi1, mdi2;
    for (int j = 0; it1 != end1 && it2 != end2; j++) {
      mdi1 = *it1;
      mdi2 = *it2;

      SetPiece funci = func(mdi1, mdi2);
      if (!isEmpty(funci)) result.emplace_hint(result.cend(), funci);

      if (maxElem(mdi1) < maxElem(mdi2)) ++it1;

      else ++it2;
    }
  }

  else {
    BOOST_FOREACH (SetPiece mdi1, pwi1)
      BOOST_FOREACH (SetPiece mdi2, pwi2) {
        SetPiece funci = func(mdi1, mdi2);
        if (!isEmpty(funci))
          result.emplace(funci);
      }
  }

  return result;
}

MDInterOrdSet traverse(OrdPWMDInter pwi1, OrdPWMDInter pwi2, SetPiece (*func)(SetPiece, SetPiece))
{
  MDInterOrdSet result;

  if (isEmpty(pwi1)) return pwi2.pieces();

  if (isEmpty(pwi2)) return pwi1.pieces();
  
  if (optConds(pwi1.pieces()) && optConds(pwi2.pieces())) {
    MDInterOrdSetIt it1 = pwi1.begin(), it2 = pwi2.begin();   
    MDInterOrdSetIt end1 = pwi1.end(), end2 = pwi2.end();   

    SetPiece mdi1, mdi2;
    for (; it1 != end1 && it2 != end2;) {
      mdi1 = *it1;
      mdi2 = *it2;

      SetPiece funci = func(mdi1, mdi2);
      if (!isEmpty(funci)) result.emplace_hint(result.cend(), funci);

      if (maxElem(mdi1) < maxElem(mdi2)) ++it1;

      else ++it2;
    }

    for (; it1 != end1; ++it1) {
      mdi1 = *it1;
      result.emplace_hint(result.cend(), mdi1);
    }

    for (; it2 != end2; ++it2) {
      mdi2 = *it2;
      result.emplace_hint(result.cend(), mdi2);
    }
  }

  else {
    BOOST_FOREACH (SetPiece mdi1, pwi1)
      BOOST_FOREACH (SetPiece mdi2, pwi2) {
        SetPiece funci = func(mdi1, mdi2);
        if (!isEmpty(funci))
          result.emplace(funci);
      }
  }

  return result;
}

std::size_t hash_value(const OrdPWMDInter &pwi)
{
  OrdPWMDInter aux_pwi = pwi;
  return boost::hash_range(aux_pwi.begin(), aux_pwi.end());
}

} // namespace LIB

} // namespace SBG
