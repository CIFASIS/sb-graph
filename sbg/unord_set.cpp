/*******************************************************************************

 This file is part of Set--Based Graph Library.

 SBG Library is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 SBG Library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the stratied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with SBG Library.  If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#include <set>

#include "sbg/unord_set.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Unordered Set Implementation ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

member_imp(UnorderedSet, UnorderedSet::MDIUnordCollection, pieces);

UnorderedSet::~UnorderedSet() {}
UnorderedSet::UnorderedSet() : pieces_() {}
UnorderedSet::UnorderedSet(const MD_NAT& x) : pieces_() {
  pieces_.emplace_back(SetPiece(x));
}
UnorderedSet::UnorderedSet(const Interval& i) : pieces_() {
  if (!i.isEmpty())
    pieces_.emplace_back(SetPiece(i));
}
UnorderedSet::UnorderedSet(const SetPiece& mdi) : pieces_() {
  if (!mdi.isEmpty())
    pieces_.emplace_back(mdi);
}
UnorderedSet::UnorderedSet(const UnorderedSet::MDIUnordCollection& pieces)
  : pieces_(std::move(pieces)) {}

SetStratPtr UnorderedSet::clone() const
{
  return std::make_unique<UnorderedSet>(*this);
}

member_imp(UnorderedSet::Iterator
  , UnorderedSet::MDIUnordCollection::const_iterator, it);

UnorderedSet::Iterator::Iterator(
  UnorderedSet::MDIUnordCollection::const_iterator it) : it_(it) {}

void UnorderedSet::Iterator::operator++()
{
  ++it_;
  return;
}

bool UnorderedSet::Iterator::operator!=(const SetStrategy::Iterator& other)
  const
{
  return it_ != static_cast<const UnorderedSet::Iterator *>(&other)->it_;
}

bool UnorderedSet::Iterator::operator==(const SetStrategy::Iterator& other)
  const
{
  return it_ == static_cast<const UnorderedSet::Iterator *>(&other)->it_;
}

bool UnorderedSet::Iterator::operator<(const SetStrategy::Iterator& other) const
{
  return it_ < static_cast<const UnorderedSet::Iterator *>(&other)->it_;
}

const SetPiece& UnorderedSet::Iterator::operator*() const { return *it_; }

std::shared_ptr<SetStrategy::Iterator> UnorderedSet::begin() const
{
  return std::make_shared<UnorderedSet::Iterator>(pieces_.begin());
}

std::shared_ptr<SetStrategy::Iterator> UnorderedSet::end() const
{
  return std::make_shared<UnorderedSet::Iterator>(pieces_.end());
}

std::size_t UnorderedSet::size() const { return pieces_.size(); }

void UnorderedSet::emplace(const SetPiece& mdi)
{
  if (mdi.isEmpty()) {
    return;
  }

  pieces_.emplace(pieces_.begin(), mdi);
  return;
}

void UnorderedSet::emplaceBack(const SetPiece& mdi)
{
  if (mdi.isEmpty()) {
    return;
  }

  pieces_.emplace_back(mdi);
  return;
}

bool UnorderedSet::operator==(const SetStrategy& other) const
{
  UnordSetCRef othr = static_cast<UnordSetCRef>(other);

  if (pieces_ == othr.pieces_)
    return true;

  return difference(other)->isEmpty() && other.difference(*this)->isEmpty();
}

bool UnorderedSet::operator!=(const SetStrategy& other) const
{
  return !(*this == other);
}

std::ostream& UnorderedSet::print(std::ostream& out) const
{
  std::size_t sz = size();

  out << "{";
  if (sz > 0) {
    unsigned int j = 0;
    for (const SetPiece& mdi : pieces_) { 
      if (j < sz - 1) {
        out << mdi << ", "; 
      } else {
        out << mdi;
      }

      ++j;
    }
  }
  out << "}";

  return out;
}

// Set operations --------------------------------------------------------------

unsigned int UnorderedSet::cardinal() const
{
  unsigned int result = 0;

  for (const SetPiece& mdi : pieces_) {
    result += mdi.cardinal();
  }

  return result;
}

bool UnorderedSet::isEmpty() const { return pieces_.empty(); }

MD_NAT UnorderedSet::minElem() const
{
  MD_NAT res = pieces_.begin()->minElem();
  for (const SetPiece& mdi : pieces_) {
    MD_NAT ith = mdi.minElem();
    if (ith < res)
      res = ith;
  }
  
  return res;
}

MD_NAT UnorderedSet::maxElem() const
{
  MD_NAT res = pieces_.begin()->maxElem();
  for (const SetPiece& mdi : pieces_) {
    MD_NAT ith = mdi.maxElem();
    if (res < ith)
      res = ith;
  }
  
  return res;
}

SetStratPtr UnorderedSet::intersection(const SetStrategy& other) const
{
  // Special cases to enhance performance
  if (isEmpty() || other.isEmpty()) {
    return std::make_unique<UnorderedSet>();
  }

  const MD_NAT min_elem = minElem();
  const MD_NAT max_elem = maxElem();
  const MD_NAT other_min = other.minElem();
  const MD_NAT other_max = other.maxElem();
  if (max_elem < other_min || other_max < min_elem) {
    return std::make_unique<UnorderedSet>();
  }

  UnorderedSet result;
  if (max_elem == other_min) {
    result.emplaceBack(SetPiece(max_elem));
    return std::make_unique<UnorderedSet>(result);
  }

  if (other_max == min_elem) {
    result.emplaceBack(SetPiece(min_elem));
    return std::make_unique<UnorderedSet>(result);
  }

  UnordSetCRef othr = static_cast<UnordSetCRef>(other);
  if (pieces_ == othr.pieces_) {
    return std::make_unique<UnorderedSet>(pieces_);
  }

  // General case
  for (const SetPiece& mdi1 : pieces_) {
    for (const SetPiece& mdi2 : othr.pieces_) {
      result.emplaceBack(mdi1.intersection(mdi2));
    }
  }

  return std::make_unique<UnorderedSet>(std::move(result));
}

SetStratPtr UnorderedSet::cup(const SetStrategy& other) const &
{
  UnordSetCRef othr = static_cast<UnordSetCRef>(other);

  if (isEmpty()) { 
    return std::make_unique<UnorderedSet>(othr.pieces_);
  }

  if (other.isEmpty() || pieces_ == othr.pieces_) {
    return std::make_unique<UnorderedSet>(pieces_);
  }

  if (maxElem() < othr.minElem() || othr.maxElem() < minElem()) {
    MDIUnordCollection result(pieces_.begin(), pieces_.end());
    result.insert(result.end(), othr.pieces_.begin(), othr.pieces_.end());
    return std::make_unique<UnorderedSet>(std::move(result));
  }

  // General case
  SetStratPtr diff = difference(other);

  return othr.disjointCup(*diff);
}

SetStratPtr UnorderedSet::cup(SetStrategy&& other) &&
{
  UnordSetCRef othr = static_cast<UnordSetCRef>(other);

  if (isEmpty()) { 
    return std::make_unique<UnorderedSet>(std::move(othr.pieces_));
  }

  if (other.isEmpty() || pieces_ == othr.pieces_) {
    return std::make_unique<UnorderedSet>(std::move(pieces_));
  }

  if (maxElem() < othr.minElem() || othr.maxElem() < minElem()) {
    MDIUnordCollection result = std::move(pieces_);
    result.insert(result.end(), othr.pieces_.begin(), othr.pieces_.end());
    return std::make_unique<UnorderedSet>(std::move(result));
  }

  // General case
  SetStratPtr diff = difference(other);
  return othr.disjointCup(*diff);
}

SetStratPtr UnorderedSet::complementAtom() const
{
  MDIUnordCollection res;

  SetPiece mdi = *pieces_.begin();
  SetPiece dense_mdi;
  for (const Interval& i : mdi)
    dense_mdi.emplaceBack(Interval(i.begin(), 1, i.end()));
  SetPiece during_mdi = dense_mdi;

  Interval univ_one_dim(0, 1, Inf);
  SetPiece univ(mdi.arity(), univ_one_dim);

  unsigned int dim = 0;
  for (const Interval& i : mdi) {
    MDIUnordCollection c;

    // Before interval
    if (i.begin() != 0) {
      Interval i_res(0, 1, i.begin() - 1);
      if (!i_res.isEmpty()) {
        univ[dim] = i_res;
        c.emplace_back(univ);
        univ[dim] = univ_one_dim;
      }
    }

    // "During" interval
    if (i.begin() < Inf) {
      if (i.step() > 1) {
        for (unsigned int j = 0; j < i.step() - 1; ++j) {
          Interval i_res(i.begin() + j + 1, i.step(), i.end());
          if (!i_res.isEmpty()) {
            during_mdi[dim] = i_res;
            c.emplace_back(during_mdi);
          }
        }
      }
    }

    // After interval
    if (i.end() < Inf) {
      Interval i_res(i.end() + 1, 1, Inf);
      if (!i_res.isEmpty()) {
        univ[dim] = i_res;
        c.emplace_back(univ);
        univ[dim] = univ_one_dim;
      }
    }
    univ[dim] = dense_mdi[dim];
    during_mdi[dim] = i;

    // Insert results of current dim
    for (const SetPiece& mdi : c)
      res.emplace_back(mdi);

    ++dim;
  }

  return std::make_unique<UnorderedSet>(std::move(res));
}

SetStratPtr UnorderedSet::complement() const
{
  SetStratPtr res = std::make_unique<UnorderedSet>(MDIUnordCollection());

  auto first_it = pieces_.begin();
  SetPiece first = *first_it;
  res = std::move(UnorderedSet(first).complementAtom());

  ++first_it;
  MDIUnordCollection second(first_it, pieces_.end());
  for (const SetPiece& mdi : second) {
    SetStratPtr c = UnorderedSet(mdi).complementAtom();
    res = std::move(res->intersection(*c));
  }

  return res;
}

SetStratPtr UnorderedSet::difference(const SetStrategy& other) const
{
  // Special cases
  if (isEmpty() || other.isEmpty()) {
    return std::make_unique<UnorderedSet>(pieces_);
  }

  if (maxElem() < other.minElem() || other.maxElem() < minElem()) {
    return std::make_unique<UnorderedSet>(pieces_);
  }

  // General case
  return intersection(*other.complement());
}

// Extra operations ------------------------------------------------------------

std::size_t UnorderedSet::arity() const
{
  if (isEmpty())
    return 0;

  return pieces_.begin()->arity();
}

SetStratPtr UnorderedSet::disjointCup(const SetStrategy& other) const &
{
  UnordSetCRef othr = static_cast<UnordSetCRef>(other);

  if (isEmpty()) { 
    return std::make_unique<UnorderedSet>(othr.pieces_);
  }

  if (other.isEmpty() || pieces_ == othr.pieces_) {
    return std::make_unique<UnorderedSet>(pieces_);
  }

  MDIUnordCollection result(pieces_.begin(), pieces_.end());
  for (const SetPiece& mdi : othr.pieces_) {
    result.emplace_back(mdi);
  }

  return std::make_unique<UnorderedSet>(std::move(result));
}

SetStratPtr UnorderedSet::disjointCup(SetStrategy&& other) &&
{
  UnordSetCRef othr = static_cast<UnordSetCRef>(other);

  if (isEmpty()) { 
    return std::make_unique<UnorderedSet>(std::move(othr.pieces_));
  }

  if (other.isEmpty() || pieces_ == othr.pieces_) {
    return std::make_unique<UnorderedSet>(std::move(pieces_));
  }

  MDIUnordCollection result(pieces_.begin(), pieces_.end());
  for (const SetPiece& mdi : othr.pieces_) {
    result.emplace_back(mdi);
  }

  return std::make_unique<UnorderedSet>(std::move(result));
}

SetStratPtr UnorderedSet::filterSet(bool (*f)(const SetPiece& mdi)) const
{
  MDIUnordCollection res;

  for (const SetPiece& mdi : pieces_) {
    if (f(mdi)) {
      res.emplace_back(mdi);
    }
  }

  return std::make_unique<UnorderedSet>(std::move(res));
}

SetStratPtr UnorderedSet::offset(const MD_NAT& off) const
{
  MDIUnordCollection res;

  for (const SetPiece& mdi : pieces_) {
    res.emplace_back(mdi.offset(off));
  }

  return std::make_unique<UnorderedSet>(std::move(res));
}

SetStratPtr UnorderedSet::compact() const
{
  MDIUnordCollection res;

  if (!isEmpty()) {
    std::set<SetPiece> prev(pieces_.begin(), pieces_.end());
    std::set<SetPiece> actual = prev;
    do {
      prev = actual;
      actual = std::set<SetPiece>();

      std::set<SetPiece>::iterator ith = prev.begin();
      std::set<SetPiece>::iterator last = prev.end();
      std::set<SetPiece> to_erase;
      for (; ith != last; ++ith) {
        SetPiece ith_compact = *ith;
        std::set<SetPiece>::iterator next = ith;
        ++next;
        for (; next != last; ++next) {
          MaybeMDI new_compact = ith_compact.compact(*next);
          if (new_compact) {
            ith_compact = new_compact.value();
            to_erase.insert(*next);
          }
        }

        if (to_erase.find(ith_compact) == to_erase.end())
          actual.insert(ith_compact);
      }
    } while (actual != prev);

    for (const SetPiece& mdi : actual)
      res.emplace_back(mdi);
  }

  return std::make_unique<UnorderedSet>(std::move(res));
}

} // namespace LIB

} // namespace SBG
