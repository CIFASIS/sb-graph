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

#include "sbg/unord_pw_mdinter.hpp"

namespace SBG {

namespace LIB {

// Type definitions ------------------------------------------------------------

std::ostream &operator<<(std::ostream &out, const MDInterUnordSet &ii) 
{
  MDInterUnordSet aux = ii;
  int sz = ii.size();

  out << "{";
  if (sz > 0) {
    auto it = aux.begin();
    for (int j = 0; j < sz - 1; ++j) { 
      out << *it << ", ";
      ++it;
    }
    out << *it;
  }
  out << "}";

  return out;
}

////////////////////////////////////////////////////////////////////////////////
// Non-optimized implementation ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

UnordPWMDInter::UnordPWMDInter() : pieces_() {}
UnordPWMDInter::UnordPWMDInter(SetPiece mdi) : pieces_()
{
  Util::ERROR_UNLESS(!isEmpty(mdi), "LIB::Unord1: empty not allowed");
  pieces_.emplace_hint(pieces_.end(), mdi);
}
UnordPWMDInter::UnordPWMDInter(MDInterUnordSet c) : pieces_(c) {}

member_imp(UnordPWMDInter, MDInterUnordSet, pieces);

std::size_t UnordPWMDInter::size() const { return pieces_.size(); }

void UnordPWMDInter::emplace(SetPiece mdi)
{
  Util::ERROR_UNLESS(!isEmpty(mdi), "LIB::Unord2: empty not allowed");
  pieces_.emplace(mdi);
}
void UnordPWMDInter::emplaceBack(SetPiece mdi)
{
  Util::ERROR_UNLESS(!isEmpty(mdi), "LIB::Unord3: empty not allowed");
  pieces_.emplace_hint(pieces_.end(), mdi);
}

UnordPWMDInter::iterator UnordPWMDInter::begin() { return pieces_.begin(); }
UnordPWMDInter::iterator UnordPWMDInter::end() { return pieces_.end(); }

SetPiece UnordPWMDInter::operator[](std::size_t n)
{
  Util::ERROR_UNLESS(n < pieces_.size() && pieces_.size() > 0
                     , "LIB::Unord::operator[]: invalid n");

  iterator it_res = pieces_.begin();

  for (unsigned int j = 0; j < n; j++)
    ++it_res;

  return *it_res;
}

const SetPiece &UnordPWMDInter::operator[](std::size_t n) const
{
  Util::ERROR_UNLESS(n < pieces_.size() && pieces_.size() > 0
                     , "LIB::Unord::operator[]: invalid n");

  iterator it_res = pieces_.begin();

  for (unsigned int j = 0; j < n; j++)
    ++it_res;

  return *it_res;
}

unsigned int UnordPWMDInter::dims() const
{
  Util::ERROR_UNLESS(pieces_.size() > 0, "LIB::Unord::dims: empty not allowed");
  return pieces_.begin()->size();
}

bool UnordPWMDInter::operator<(const UnordPWMDInter &other) const
{
  return minElem(*this) < minElem(other);
}

bool UnordPWMDInter::operator==(const UnordPWMDInter &other) const
{
  UnordPWMDInter pwi1 = *this, pwi2 = other;
  if (pwi1.pieces_ref() == pwi2.pieces_ref())
    return true;

  return isEmpty(difference(*this, other)) && isEmpty(difference(other, *this));
}

std::ostream &operator<<(std::ostream &out, const UnordPWMDInter &pwi)
{
  out << pwi.pieces();

  return out;
}

// Set operations --------------------------------------------------------------

unsigned int cardinal(UnordPWMDInter pwi) 
{
  unsigned int result = 0;

  for (SetPiece mdi : pwi)
    result += cardinal(mdi);

  return result;
}

bool isEmpty(UnordPWMDInter pwi) { return pwi.pieces().empty(); }

bool isMember(MD_NAT x, UnordPWMDInter pwi)
{
  if (isEmpty(pwi))
    return false;

  Util::ERROR_UNLESS(pwi[0].size() == x.size()
                     , "LIB::Unord::isMember: dimensions don't match");

  for (SetPiece mdi : pwi)
    if (!isMember(x, mdi)) 
      return false;

  return true;
}

Util::MD_NAT minElem(UnordPWMDInter pwi)
{
  Util::ERROR_UNLESS(!isEmpty(pwi)
                     , "LIB::Unord::minElem: shouldn't be empty");

  MD_NAT res = minElem(pwi[0]);
  for (SetPiece mdi : pwi)
    if (minElem(mdi) < res)
      res = minElem(mdi);

  return res;
}

Util::MD_NAT maxElem(UnordPWMDInter pwi)
{
  Util::ERROR_UNLESS(!isEmpty(pwi)
                     , "LIB::Unord::maxElem: shouldn't be empty");

  MD_NAT res = maxElem(pwi[0]);
  for (SetPiece mdi : pwi)
    if (res < maxElem(mdi))
      res = maxElem(mdi);

  return res;
}

UnordPWMDInter intersection(UnordPWMDInter pwi1, UnordPWMDInter pwi2)
{
  // Special cases to enhance performance
  if (isEmpty(pwi1) || isEmpty(pwi2))
    return UnordPWMDInter();

  Util::ERROR_UNLESS(pwi1.dims() == pwi2.dims()
                     , "LIB::Unord::intersection: dimensions don't match");

  if (pwi1.pieces() == pwi2.pieces())
    return pwi1;

  // General case
  MDInterUnordSet cap;
  for (SetPiece mi1 : pwi1) 
    for (SetPiece mi2 : pwi2) {
      SetPiece ith = intersection(mi1, mi2);

      if (!isEmpty(ith))
        cap.insert(ith); 
    }

  return UnordPWMDInter(cap);
}

UnordPWMDInter cup(UnordPWMDInter pwi1, UnordPWMDInter pwi2)
{
  UnordPWMDInter res;

  if (isEmpty(pwi1))
    return pwi2;
 
  if (isEmpty(pwi2))
    return pwi1;

  Util::ERROR_UNLESS(pwi1.dims() == pwi2.dims()
                     , "LIB::Unord::cup: dimensions don't match");

  if (pwi1.pieces() == pwi2.pieces())
    return pwi1;

  if (maxElem(pwi1) <= minElem(pwi2)) {
    for (SetPiece i1 : pwi1)
      res.emplaceBack(i1);

    for (SetPiece i2 : pwi2)
      res.emplaceBack(i2);
    
    return res;
  }

  if (maxElem(pwi2) <= minElem(pwi1)) {
    for (SetPiece i2 : pwi2)
      res.emplaceBack(i2);

    for (SetPiece i1 : pwi1)
      res.emplaceBack(i1);

    return res;
  }

  // General case
  UnordPWMDInter exclusive = difference(pwi1, pwi2);
  if (!isEmpty(exclusive)) 
    return concatenation(exclusive, pwi2);

  return pwi2;
}

UnordPWMDInter complementAtom(UnordPWMDInter pwi)
{
  UnordPWMDInter res;

  SetPiece mdi = pwi[0];

  Interval univ(0, 1, Util::Inf);
  SetPiece all(mdi.size(), univ);

  unsigned int dim = 0;
  for (Interval i : mdi) {
    MDInterUnordSet c;

    // Before interval
    if (i.begin() != 0) {
      Interval i_res(0, 1, i.begin() - 1);
      if (!isEmpty(i_res)) {
        all[dim] = i_res;
        c.emplace_hint(c.cend(), all);  
        all[dim] = univ;
      }
    }

    // "During" interval
    if (i.begin() < Util::Inf) {
      for (Util::NAT j = 1; j < i.step(); j++) {
        Interval i_res(i.begin() + j, i.step(), i.end());
        if (!isEmpty(i_res)) {
          all[dim] = i_res;
          c.emplace_hint(c.cend(), all);  
        }
      }
      all[dim] = univ;
    }

    // After interval
    if (maxElem(i) < Util::Inf)
      all[dim] = Interval(maxElem(i) + 1, 1, Util::Inf);
    else
      all[dim] = Interval(Util::Inf);

    c.emplace(all);
    all[dim] = i;

    // Initialize result
    if (dim == 0)
      res = UnordPWMDInter(c);
    else
      res = concatenation(res, UnordPWMDInter(c));

    ++dim;
  }

  return res;
}

UnordPWMDInter complement(UnordPWMDInter pwi)
{
  UnordPWMDInter res;

  if (!isEmpty(pwi)) {
    MDInterUnordSetIt first_it = pwi.begin();
    SetPiece first = *first_it;
    res = complementAtom(UnordPWMDInter(first));

    ++first_it;
    MDInterUnordSet second(first_it, pwi.end());
    for (SetPiece mdi : second) {
      UnordPWMDInter c = complementAtom(UnordPWMDInter(mdi));
      res = intersection(res, c);
    }
  }

  return res;
}

UnordPWMDInter difference(UnordPWMDInter pwi1, UnordPWMDInter pwi2)
{
  if (isEmpty(pwi1))
    return pwi1;

  if (isEmpty(pwi2))
    return pwi1;

  Util::ERROR_UNLESS(pwi1.dims() == pwi2.dims()
                     , "LIB::Unord::difference: dimensions don't match");

  UnordPWMDInter diff = intersection(pwi1, complement(pwi2));
  return diff;
}

// Extra operations ------------------------------------------------------------

UnordPWMDInter concatenation(UnordPWMDInter pwi1, UnordPWMDInter pwi2)
{
  UnordPWMDInter res;

  if (isEmpty(pwi1))
    return pwi2;

  if (isEmpty(pwi2))
    return pwi1;

  Util::ERROR_UNLESS(pwi1.dims() == pwi2.dims()
                     , "LIB::Unord::concatenation: dimensions don't match");

  for (SetPiece mdi : pwi1)
    res.emplaceBack(mdi);
  for (SetPiece mdi : pwi2)
    res.emplaceBack(mdi);

  return res;
}

UnordPWMDInter filterSet(bool (*f)(SetPiece), UnordPWMDInter pwi)
{
  UnordPWMDInter res;

  for (SetPiece mdi : pwi)
    if (f(mdi))
      res.emplaceBack(mdi);

  return res;
}

UnordPWMDInter offset(Util::MD_NAT off, UnordPWMDInter pwi)
{
  UnordPWMDInter res;

  if (isEmpty(pwi))
    return res;

  Util::ERROR_UNLESS(off.size() == pwi.dims()
                     , "LIB::Unord::offset: dimensions don't match");

  for (SetPiece mdi : pwi)
    res.emplace(offset(off, mdi));

  return res;
}

std::size_t hash_value(const UnordPWMDInter &pwi)
{
  UnordPWMDInter aux_pwi = pwi;
  return boost::hash_range(aux_pwi.begin(), aux_pwi.end());
}

} // namespace LIB

} // namespace SBG
