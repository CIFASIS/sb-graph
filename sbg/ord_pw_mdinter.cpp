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

std::ostream &operator<<(std::ostream &out, const MDInterOrdSet &ii) 
{
  std::size_t sz = ii.size();

  out << "{";
  if (sz > 0) {
    unsigned int j = 0;
    for (const SetPiece &mdi : ii) { 
      if (j < sz - 1)
        out << mdi << ", "; 
      else
        out << mdi;

      ++j;
    }
  }
  out << "}";

  return out;
}

// OrdPWMDInter ------------------------------------------------------------------

OrdPWMDInter::OrdPWMDInter() : pieces_() {}
OrdPWMDInter::OrdPWMDInter(Util::MD_NAT x) : pieces_() {
  pieces_.insert(SetPiece(x));
}
OrdPWMDInter::OrdPWMDInter(Interval i) : pieces_() {
  if (!i.isEmpty())
    pieces_.insert(SetPiece(i));
}
OrdPWMDInter::OrdPWMDInter(SetPiece mdi) : pieces_() { 
  if (!mdi.isEmpty())
    pieces_.insert(mdi);
}
OrdPWMDInter::OrdPWMDInter(MDInterOrdSet container) : pieces_() {
  for (const SetPiece &mdi : container) {
    if (!mdi.isEmpty())
      pieces_.insert(mdi);
  }
}

member_imp(OrdPWMDInter, MDInterOrdSet, pieces);

OrdPWMDInter::iterator OrdPWMDInter::begin() { return pieces_.begin(); }
OrdPWMDInter::iterator OrdPWMDInter::end() { return pieces_.end(); }
OrdPWMDInter::const_iterator OrdPWMDInter::begin() const
{
  return pieces_.begin();
}
OrdPWMDInter::const_iterator OrdPWMDInter::end() const { return pieces_.end(); }

std::size_t OrdPWMDInter::size() const { return pieces_.size(); }

void OrdPWMDInter::emplace(SetPiece mdi)
{
  if (!mdi.isEmpty())
    pieces_.emplace(mdi);
}
void OrdPWMDInter::emplaceBack(SetPiece mdi)
{
  if (!mdi.isEmpty())
    pieces_.emplace_hint(pieces_.cend(), mdi);
}

bool OrdPWMDInter::operator==(const OrdPWMDInter &other) const
{
  return compact().pieces_ == other.compact().pieces_;
}

bool OrdPWMDInter::operator!=(const OrdPWMDInter &other) const
{
  return !(*this == other);
}

bool OrdPWMDInter::operator<(const OrdPWMDInter &other) const
{
  if (other.isEmpty())
    return false;

  if (isEmpty())
    return true;

  return minElem() < other.minElem();
}

std::ostream &operator<<(std::ostream &out, const OrdPWMDInter &pwi)
{
  out << pwi.pieces();

  return out;
}

// Set functions ---------------------------------------------------------------

unsigned int OrdPWMDInter::cardinal() const
{
  unsigned int result = 0;

  for (const SetPiece &mdi : pieces_)
    result += mdi.cardinal();

  return result;
}

bool OrdPWMDInter::isEmpty() const { return pieces_.empty(); }

Util::MD_NAT OrdPWMDInter::minElem() const
{
  return begin()->minElem();
}

Util::MD_NAT OrdPWMDInter::maxElem() const
{
  auto it = end();
  --it;
  return it->maxElem();
}

OrdPWMDInter OrdPWMDInter::intersection(const OrdPWMDInter &other) const
{
  // Special cases to enhance performance
  if (isEmpty() || other.isEmpty())
    return OrdPWMDInter();

  if (maxElem() < other.minElem())
    return OrdPWMDInter();

  if (other.maxElem() < minElem())
    return OrdPWMDInter();

  if (maxElem() == other.minElem()) {
    return OrdPWMDInter(SetPiece(maxElem()));
  }

  if (other.maxElem() == minElem())
    return OrdPWMDInter(SetPiece(other.maxElem()));

  if (pieces_ == other.pieces_)
    return *this;

  // General case
  MDInterOrdSet cap = boundedTraverse(&SetPiece::intersection, other);

  return OrdPWMDInter(cap);
}

OrdPWMDInter OrdPWMDInter::cup(const OrdPWMDInter &other) const
{
  OrdPWMDInter un;

  if (isEmpty())
    return other;
 
  if (other.isEmpty())
    return *this;

  if (pieces_ == other.pieces_)
    return pieces_;

  if (maxElem() < other.minElem()) {
    for (const SetPiece &mdi1 : pieces_)
      un.emplaceBack(mdi1);

    for (const SetPiece &mdi2 : other.pieces_)
      un.emplaceBack(mdi2);
    
    return OrdPWMDInter(un);
  }

  if (other.maxElem() < minElem()) {
    for (const SetPiece &mdi2 : other.pieces_)
      un.emplaceBack(mdi2);

    for (const SetPiece &mdi1 : pieces_)
      un.emplaceBack(mdi1);

    return OrdPWMDInter(un);
  }

  // General case

  // As the complement operation will add intervals to our set, we choose the
  // one with least quantity of them.  
  OrdPWMDInter lt_pieces, gt_pieces;
  int c_size1 = 0, c_size2 = 0;
  for (const SetPiece &mdi1 : pieces_)
    c_size1 += mdi1.begin()->step();
  for (const SetPiece &mdi2 : other.pieces_)
    c_size2 += mdi2.begin()->step();

  if (c_size1 < c_size2) {
    lt_pieces = *this;
    gt_pieces = other;
  }

  else {
    lt_pieces = other;
    gt_pieces = *this;
  }

  OrdPWMDInter diff = gt_pieces.difference(lt_pieces);

  return lt_pieces.concatenation(diff);
}

OrdPWMDInter OrdPWMDInter::complementAtom() const
{
  OrdPWMDInter res;

  Interval i = begin()->operator[](0);

  // Before interval
  if (i.begin() != 0) {
    Interval i_res(0, 1, i.begin() - 1);
    if (!i_res.isEmpty())
      res.emplaceBack(SetPiece(i_res));
  }

  // "During" interval
  if (i.begin() < Util::Inf) {
    for (Util::NAT j = 1; j < i.step(); ++j) {
      Interval i_res(i.begin() + j, i.step(), i.end());
      if (!i_res.isEmpty())
        res.emplaceBack(SetPiece(i_res));
     }
  }

  // After interval
  if (i.end() < Util::Inf)
    res.emplaceBack(SetPiece(Interval(i.end() + 1, 1, Util::Inf)));
  else 
    res.emplaceBack(SetPiece(Interval(Util::Inf)));

  return res;
}

OrdPWMDInter OrdPWMDInter::complement() const
{
  OrdPWMDInter res;

  if (isEmpty())
    return OrdPWMDInter(Interval(0, 1, Util::Inf));

  auto first_it = begin();
  SetPiece first = *first_it;
  res = OrdPWMDInter(first).complementAtom();

  ++first_it;
  MDInterOrdSet second(first_it, end());
  for (const SetPiece &mdi : second) {
    OrdPWMDInter c = OrdPWMDInter(mdi).complementAtom();
    res = res.intersection(c);
  }

  return res;
}

OrdPWMDInter OrdPWMDInter::difference(const OrdPWMDInter &other) const
{
  return this->intersection(other.complement());
}

// Extra operations ------------------------------------------------------------

std::size_t OrdPWMDInter::arity() const
{
  if (isEmpty())
    return 0;

  return begin()->arity();
}

OrdPWMDInter OrdPWMDInter::concatenation(const OrdPWMDInter &other) const
{
  return traverse(&SetPiece::least, other);
}

OrdPWMDInter OrdPWMDInter::filterSet(bool (*f)(const SetPiece &mdi)) const
{
  OrdPWMDInter res;

  for (const SetPiece &mdi : pieces_)
    if (f(mdi))
      res.emplaceBack(mdi);

  return res;
}

OrdPWMDInter OrdPWMDInter::offset(Util::MD_NAT off) const
{
  OrdPWMDInter res;

  for (const SetPiece &mdi : pieces_)
    res.emplaceBack(mdi.offset(off));

  return res;
}

OrdPWMDInter OrdPWMDInter::compact() const
{
  OrdPWMDInter res;

  if (isEmpty())
    return res;

  OrdPWMDInter compacted;
  for (auto it = begin(); it != end(); ++it) {
    auto next_it = it;
    ++next_it;
    OrdPWMDInter ith_compacted = compacted.intersection(OrdPWMDInter(*it));
    if (ith_compacted.isEmpty()) {
      SetPiece new_ith = *it;
      for (; next_it != end(); ++next_it) {
        OrdPWMDInter next_compacted 
          = compacted.intersection(OrdPWMDInter(*next_it));
        if (next_compacted.isEmpty()) {
          auto ith = new_ith.compact(*next_it);
          if (ith) {
            new_ith = std::move(ith.value());
            compacted.emplaceBack(*next_it);
          }
        }
      }

      res.emplaceBack(new_ith);
    }
  }

  return res;
}

MDInterOrdSet OrdPWMDInter::boundedTraverse(
    SetPiece (SetPiece::*f)(const SetPiece &) const, const OrdPWMDInter &other
) const
{
  MDInterOrdSet res;

  if (isEmpty() || other.isEmpty())
    return res;

  auto it1 = begin(), it2 = other.begin();
  auto end1 = end(), end2 = other.end();

  SetPiece mdi1, mdi2;
  for (int j = 0; it1 != end1 && it2 != end2; ++j) {
    mdi1 = *it1;
    mdi2 = *it2;

    SetPiece funci = (mdi1.*f)(mdi2);
    if (!funci.isEmpty())
      res.emplace_hint(res.cend(), funci);

    if (mdi1.maxElem() < mdi2.maxElem())
      ++it1;
    else 
      ++it2;
  }

  return res;
}

MDInterOrdSet OrdPWMDInter::traverse(
    SetPiece (SetPiece::*f)(const SetPiece &) const, const OrdPWMDInter &other
) const
{
  MDInterOrdSet res;

  if (isEmpty())
    return other.pieces_;

  if (other.isEmpty())
    return pieces_;
  
  auto it1 = begin(), it2 = other.begin();
  auto end1 = end(), end2 = other.end();

  SetPiece mdi1, mdi2;
  for (; it1 != end1 && it2 != end2;) {
    mdi1 = *it1;
    mdi2 = *it2;

    SetPiece funci = (mdi1.*f)(mdi2);
    if (!funci.isEmpty())
      res.emplace_hint(res.cend(), funci);

    if (mdi1.maxElem() < mdi2.maxElem())
      ++it1;
    else
      ++it2;
  }

  for (; it1 != end1; ++it1) {
    mdi1 = *it1;
    res.emplace_hint(res.cend(), mdi1);
  }

  for (; it2 != end2; ++it2) {
    mdi2 = *it2;
    res.emplace_hint(res.cend(), mdi2);
  }

  return res;
}

} // namespace LIB

} // namespace SBG
