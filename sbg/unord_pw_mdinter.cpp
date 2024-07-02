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
UnordPWMDInter::UnordPWMDInter(Util::MD_NAT x) : pieces_() {
  pieces_.insert(SetPiece(x));
}
UnordPWMDInter::UnordPWMDInter(Interval i) : pieces_() {
  if (!i.isEmpty())
   pieces_.insert(SetPiece(i));
}
UnordPWMDInter::UnordPWMDInter(SetPiece mdi) : pieces_(){
  if (!mdi.isEmpty())
    pieces_.emplace_hint(pieces_.end(), mdi);
}
UnordPWMDInter::UnordPWMDInter(MDInterUnordSet c) : pieces_(c) {}

member_imp(UnordPWMDInter, MDInterUnordSet, pieces);

UnordPWMDInter::iterator UnordPWMDInter::begin() { return pieces_.begin(); }
UnordPWMDInter::iterator UnordPWMDInter::end() { return pieces_.end(); }
UnordPWMDInter::const_iterator UnordPWMDInter::begin() const
{
  return pieces_.begin();
}
UnordPWMDInter::const_iterator UnordPWMDInter::end() const
{
  return pieces_.end();
}

std::size_t UnordPWMDInter::size() const { return pieces_.size(); }

void UnordPWMDInter::emplace(SetPiece mdi)
{
  if (!mdi.isEmpty())
    pieces_.emplace(mdi);
}
void UnordPWMDInter::emplaceBack(SetPiece mdi)
{
  if (!mdi.isEmpty())
    pieces_.emplace_hint(pieces_.end(), mdi);
}

bool UnordPWMDInter::operator==(const UnordPWMDInter &other) const
{
  if (pieces_ == other.pieces_)
    return true;

  return this->difference(other).isEmpty()
         && other.difference(*this).isEmpty();
}

bool UnordPWMDInter::operator!=(const UnordPWMDInter &other) const
{
  return !(*this == other);
}

bool UnordPWMDInter::operator<(const UnordPWMDInter &other) const
{
  return this->minElem() < other.minElem();
}

std::ostream &operator<<(std::ostream &out, const UnordPWMDInter &pwi)
{
  out << pwi.pieces();

  return out;
}

// Set operations --------------------------------------------------------------

unsigned int UnordPWMDInter::cardinal() const
{
  unsigned int result = 0;

  for (const SetPiece &mdi : pieces_)
    result += mdi.cardinal();

  return result;
}

bool UnordPWMDInter::isEmpty() const { return pieces_.empty(); }

Util::MD_NAT UnordPWMDInter::minElem() const
{
  Util::ERROR_UNLESS(!isEmpty()
      , "LIB::Unord::minElem: shouldn't be empty");

  MD_NAT res = begin()->minElem();
  for (const SetPiece &mdi : pieces_) {
    Util::MD_NAT ith = mdi.minElem();
    if (ith < res)
      res = ith;
  }

  return res;
}

Util::MD_NAT UnordPWMDInter::maxElem() const
{
  Util::ERROR_UNLESS(!isEmpty()
      , "LIB::Unord::maxElem: shouldn't be empty");

  MD_NAT res = begin()->maxElem();
  for (const SetPiece &mdi : pieces_) {
    Util::MD_NAT ith = mdi.maxElem();
    if (res < ith)
      res = ith;
  }

  return res;
}

UnordPWMDInter UnordPWMDInter::intersection(const UnordPWMDInter &other) const
{
  // Special cases to enhance performance
  if (isEmpty() || other.isEmpty())
    return UnordPWMDInter();

  Util::ERROR_UNLESS(begin()->size() == other.begin()->size()
      , "LIB::Unord::intersection: dimensions don't match");

  if (pieces_ == other.pieces_)
    return *this;

  // General case
  MDInterUnordSet cap;
  for (const SetPiece &mdi1 : pieces_)
    for (const SetPiece &mdi2 : other.pieces_) {
      SetPiece ith = mdi1.intersection(mdi2);

      if (!ith.isEmpty())
        cap.insert(ith);
    }

  return UnordPWMDInter(cap);
}

UnordPWMDInter UnordPWMDInter::cup(const UnordPWMDInter &other) const
{
  UnordPWMDInter res;

  if (isEmpty())
    return other;

  if (other.isEmpty())
    return *this;

  Util::ERROR_UNLESS(begin()->size() == other.begin()->size()
      , "LIB::Unord::cup: dimensions don't match");

  if (pieces_ == other.pieces_)
    return *this;

  if (maxElem() < other.minElem()) {
    for (const SetPiece &mdi1 : pieces_)
      res.emplaceBack(mdi1);

    for (const SetPiece &mdi2 : other.pieces_)
      res.emplaceBack(mdi2);

    return res;
  }

  if (other.maxElem() < minElem()) {
    for (const SetPiece &mdi2 : other.pieces_)
      res.emplaceBack(mdi2);

    for (const SetPiece &mdi1 : pieces_)
      res.emplaceBack(mdi1);

    return res;
  }

  // General case
  UnordPWMDInter exclusive = difference(other);
  if (!exclusive.isEmpty())
    return exclusive.concatenation(other);

  return other;
}

UnordPWMDInter UnordPWMDInter::complementAtom() const
{
  UnordPWMDInter res;

  SetPiece mdi = *begin();

  Interval univ(0, 1, Util::Inf);
  SetPiece all(mdi.size(), univ);

  unsigned int dim = 0;
  for (const Interval &i : mdi) {
    MDInterUnordSet c;

    // Before interval
    if (i.begin() != 0) {
      Interval i_res(0, 1, i.begin() - 1);
      if (!i_res.isEmpty()) {
        all[dim] = i_res;
        c.emplace_hint(c.cend(), all);
        all[dim] = univ;
      }
    }

    // "During" interval
    if (i.begin() < Util::Inf) {
      for (Util::NAT j = 1; j < i.step(); ++j) {
        Interval i_res(i.begin() + j, i.step(), i.end());
        if (!i_res.isEmpty()) {
          all[dim] = i_res;
          c.emplace_hint(c.cend(), all);
        }
      }
      all[dim] = univ;
    }

    // After interval
    if (i.maxElem() < Util::Inf)
      all[dim] = Interval(i.maxElem() + 1, 1, Util::Inf);
    else
      all[dim] = Interval(Util::Inf);

    c.emplace(all);
    all[dim] = i;

    // Initialize result
    if (dim == 0)
      res = UnordPWMDInter(c);
    else
      res = res.concatenation(UnordPWMDInter(c));

    ++dim;
  }

  return res;
}

UnordPWMDInter UnordPWMDInter::complement() const
{
  UnordPWMDInter res;

  if (!isEmpty()) {
    auto first_it = pieces_.begin();
    SetPiece first = *first_it;
    res = UnordPWMDInter(first).complementAtom();

    ++first_it;
    MDInterUnordSet second(first_it, end());
    for (const SetPiece &mdi : second) {
      UnordPWMDInter c = UnordPWMDInter(mdi).complementAtom();
      res = res.intersection(c);
    }
  }

  return res;
}

UnordPWMDInter UnordPWMDInter::difference(const UnordPWMDInter &other) const
{
  if (isEmpty())
    return *this;

  if (other.isEmpty())
    return *this;

  Util::ERROR_UNLESS(begin()->size() == other.begin()->size()
      , "LIB::Unord::difference: dimensions don't match");

  return intersection(other.complement());
}

// Extra operations ------------------------------------------------------------

UnordPWMDInter UnordPWMDInter::concatenation(const UnordPWMDInter &other) const
{
  UnordPWMDInter res;

  if (isEmpty())
    return other;

  if (other.isEmpty())
    return *this;

  Util::ERROR_UNLESS(begin()->size() == other.begin()->size()
      , "LIB::Unord::concatenation: dimensions don't match");

  for (const SetPiece &mdi : pieces_)
    res.emplaceBack(mdi);
  for (const SetPiece &mdi : other.pieces_)
    res.emplaceBack(mdi);

  return res;
}

UnordPWMDInter UnordPWMDInter::filterSet(bool (*f)(const SetPiece &mdi)) const
{
  UnordPWMDInter res;

  for (const SetPiece &mdi : pieces_)
    if (f(mdi))
      res.emplaceBack(mdi);

  return res;
}

UnordPWMDInter UnordPWMDInter::offset(const Util::MD_NAT &off) const
{
  UnordPWMDInter res;

  if (isEmpty())
    return res;

  Util::ERROR_UNLESS(off.size() == begin()->size()
      , "LIB::Unord::offset: dimensions don't match");

  for (const SetPiece &mdi : pieces_)
    res.emplace(mdi.offset(off));

  return res;
}

UnordPWMDInter UnordPWMDInter::compact() const
{
  UnordPWMDInter res;

  if (isEmpty())
    return res;

  UnordPWMDInter compacted;
  for (auto it = begin(); it != end(); ++it) {
    auto next_it = it;
    ++next_it;
    UnordPWMDInter ith_compacted = compacted.intersection(UnordPWMDInter(*it));
    if (ith_compacted.isEmpty()) {
      SetPiece new_ith = *it;
      for (; next_it != end(); ++next_it) {
        UnordPWMDInter next_compacted 
          = compacted.intersection(UnordPWMDInter(*next_it));
        if (next_compacted.isEmpty()) {
          auto ith = new_ith.compact(*next_it);
          if (ith) {
            new_ith = ith.value();
            compacted.emplaceBack(*next_it);
          }
        }
      }

      res.emplaceBack(new_ith);
    }
  }

  return res;
}

std::size_t hash_value(const UnordPWMDInter &pwi)
{
  return boost::hash_range(pwi.begin(), pwi.end());
}

} // namespace LIB

} // namespace SBG
