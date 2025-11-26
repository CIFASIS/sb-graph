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

#include "sbg/ord_unidim_dense_set.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Ordered Unidimensional Dense Set Implementation -----------------------------
////////////////////////////////////////////////////////////////////////////////

member_imp(OrdUnidimDenseSet, OrdUnidimDenseSet::MDIOrdCollection, pieces);

OrdUnidimDenseSet::~OrdUnidimDenseSet() {}
OrdUnidimDenseSet::OrdUnidimDenseSet() : pieces_() {}
OrdUnidimDenseSet::OrdUnidimDenseSet(MD_NAT x) : pieces_() {
  pieces_.emplace_back(SetPiece(x));
}
OrdUnidimDenseSet::OrdUnidimDenseSet(Interval i) : pieces_() {
  if (!i.isEmpty())
    pieces_.emplace_back(SetPiece(i));
}
OrdUnidimDenseSet::OrdUnidimDenseSet(SetPiece mdi) : pieces_() {
  if (!mdi.isEmpty())
    pieces_.emplace_back(mdi);
}
OrdUnidimDenseSet::OrdUnidimDenseSet(OrdUnidimDenseSet::MDIOrdCollection pieces)
  : pieces_(std::move(pieces)) {}

SetStratPtr OrdUnidimDenseSet::clone() const
{
  return std::make_unique<OrdUnidimDenseSet>(*this);
}

member_imp(OrdUnidimDenseSet::Iterator
  , OrdUnidimDenseSet::MDIOrdCollection::const_iterator, it);

OrdUnidimDenseSet::Iterator::Iterator(
  OrdUnidimDenseSet::MDIOrdCollection::const_iterator it) : it_(it) {}

void OrdUnidimDenseSet::Iterator::operator++()
{
  ++it_;
  return;
}

bool OrdUnidimDenseSet::Iterator::operator!=(const SetStrategy::Iterator& other)
  const
{
  return it_ != static_cast<const OrdUnidimDenseSet::Iterator *>(&other)->it_;
}

bool OrdUnidimDenseSet::Iterator::operator==(const SetStrategy::Iterator& other)
  const
{
  return it_ == static_cast<const OrdUnidimDenseSet::Iterator *>(&other)->it_;
}

bool OrdUnidimDenseSet::Iterator::operator<(const SetStrategy::Iterator& other)
  const
{
  return it_ < static_cast<const OrdUnidimDenseSet::Iterator *>(&other)->it_;
}

const SetPiece& OrdUnidimDenseSet::Iterator::operator*() const { return *it_; }

std::shared_ptr<SetStrategy::Iterator> OrdUnidimDenseSet::begin() const
{
  return std::make_shared<OrdUnidimDenseSet::Iterator>(pieces_.begin());
}

std::shared_ptr<SetStrategy::Iterator> OrdUnidimDenseSet::end() const
{
  return std::make_shared<OrdUnidimDenseSet::Iterator>(pieces_.end());
}

std::size_t OrdUnidimDenseSet::size() const { return pieces_.size(); }

void OrdUnidimDenseSet::emplace(const SetPiece& mdi)
{
  if (mdi.isEmpty())
    return;

  if (pieces_.empty() || pieces_.back() < mdi) {
    pieces_.emplace_back(mdi);
    return;
  }

  if (mdi < pieces_.front()) {
    pieces_.emplace(pieces_.begin(), mdi);
    return;
  }

  auto it = pieces_.begin();
  auto end = pieces_.end();
  for (; it != end; ++it) {
    if (mdi < *it)
      break;
  }
  pieces_.emplace(it, mdi);

  return;
}

void OrdUnidimDenseSet::emplaceBack(const SetPiece& mdi)
{
  if (mdi.isEmpty()) {
    return;
  }

  if (pieces_.empty() || pieces_.back() < mdi) {
    pieces_.emplace_back(mdi);
    return;
  }

  if (mdi < pieces_.front()) {
    pieces_.emplace(pieces_.begin(), mdi);
    return;
  }

  auto it = pieces_.rbegin();
  auto rend = pieces_.rend();
  for (; it != rend; ++it) {
    if (*it < mdi) {
      break;
    }
  }
  pieces_.emplace(it.base(), mdi);

  return;
}

bool OrdUnidimDenseSet::operator==(const SetStrategy& other) const
{
  SetStratPtr this_comp = compact();
  OrdUnidimDenseSetCRef ths = static_cast<OrdUnidimDenseSetCRef>(*this_comp);
  SetStratPtr other_comp = other.compact();
  OrdUnidimDenseSetCRef othr = static_cast<OrdUnidimDenseSetCRef>(*other_comp);

  return ths.pieces_ == othr.pieces_;
}

bool OrdUnidimDenseSet::operator!=(const SetStrategy& other) const
{
  return !(*this == other);
}

std::ostream& OrdUnidimDenseSet::print(std::ostream& out) const
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

unsigned int OrdUnidimDenseSet::cardinal() const
{
  unsigned int result = 0;

  for (const SetPiece& mdi : pieces_)
    result += mdi.cardinal();

  return result;
}

bool OrdUnidimDenseSet::isEmpty() const { return pieces_.empty(); }

MD_NAT OrdUnidimDenseSet::minElem() const
{
  return pieces_.begin()->minElem();
}

MD_NAT OrdUnidimDenseSet::maxElem() const
{
  auto it = pieces_.end();
  --it;
  return it->maxElem();
}

SetStratPtr OrdUnidimDenseSet::intersection(const SetStrategy& other) const
{
  // Special cases to enhance performance
  if (isEmpty() || other.isEmpty()) 
    return std::make_unique<OrdUnidimDenseSet>();

  const MD_NAT min_elem = minElem();
  const MD_NAT max_elem = maxElem();
  const MD_NAT other_min = other.minElem();
  const MD_NAT other_max = other.maxElem();
  if (max_elem < other_min || other_max < min_elem) 
    return std::make_unique<OrdUnidimDenseSet>();

  OrdUnidimDenseSet result;
  if (max_elem == other_min) {
    result.emplaceBack(SetPiece(max_elem));
    return std::make_unique<OrdUnidimDenseSet>(result);
  }

  if (min_elem == other_max) {
    result.emplaceBack(SetPiece(min_elem));
    return std::make_unique<OrdUnidimDenseSet>(result);
  }

  const OrdUnidimDenseSetCRef othr = static_cast<OrdUnidimDenseSetCRef>(other);
  if (pieces_ == othr.pieces_)
    return std::make_unique<OrdUnidimDenseSet>(pieces_);

  // General case
  MDIOrdCollection cap = boundedTraverse(&SetPiece::intersection, othr.pieces_);

  return std::make_unique<OrdUnidimDenseSet>(cap);
}

SetStratPtr OrdUnidimDenseSet::cup(const SetStrategy& other) const
{
  // Special cases
  OrdUnidimDenseSetCRef othr = static_cast<OrdUnidimDenseSetCRef>(other);
  if (isEmpty()) 
    return std::make_unique<OrdUnidimDenseSet>(othr.pieces_);

  if (other.isEmpty() || pieces_ == othr.pieces_)
    return std::make_unique<OrdUnidimDenseSet>(pieces_);

  const MD_NAT min_elem = minElem();
  const MD_NAT max_elem = maxElem();
  const MD_NAT other_min = other.minElem();
  const MD_NAT other_max = other.maxElem();
  MDIOrdCollection result;
  result.reserve(pieces_.size() + othr.pieces_.size());
  if (max_elem < other_min) {
    result.insert(result.end(), pieces_.begin(), pieces_.end());
    result.insert(result.end(), othr.pieces_.begin(), othr.pieces_.end());
    return std::make_unique<OrdUnidimDenseSet>(result);
  }

  if (other_max < min_elem) {
    result.insert(result.end(), othr.pieces_.begin(), othr.pieces_.end());
    result.insert(result.end(), pieces_.begin(), pieces_.end());
    return std::make_unique<OrdUnidimDenseSet>(result);
  }

  // General case
  SetStratPtr diff = difference(other);

  return othr.disjointCup(*diff);
}

SetStratPtr OrdUnidimDenseSet::complement() const
{
  OrdUnidimDenseSet res;

  if (isEmpty()) {
    res.emplaceBack(SetPiece(Interval(0, 1, Inf))); 
    return std::make_unique<OrdUnidimDenseSet>(res);
  }

  auto first_it = pieces_.begin();
  Interval first(0, 1, first_it->operator[](0).begin() - 1);
  if (!first.isEmpty())
    res.emplaceBack(SetPiece(first));
  NAT last = first_it->maxElem()[0];

  ++first_it;
  MDIOrdCollection second(first_it, pieces_.end());
  for (const SetPiece& mdi : second) {
    Interval ith(last + 1, 1, mdi[0].begin() - 1);
    if (!ith.isEmpty())
      res.emplaceBack(SetPiece(ith));
    last = mdi.maxElem()[0];
  }
  Interval end(last + 1, 1, Inf);
  res.emplaceBack(SetPiece(end));

  return std::make_unique<OrdUnidimDenseSet>(res);
}

SetStratPtr OrdUnidimDenseSet::difference(const SetStrategy& other) const
{
  if (isEmpty() || other.isEmpty())
    return std::make_unique<OrdUnidimDenseSet>(pieces_);

  if (maxElem() < other.minElem() || other.maxElem() < minElem()) 
    return std::make_unique<OrdUnidimDenseSet>(pieces_);

  return intersection(*other.complement());
}

// Extra operations ------------------------------------------------------------

std::size_t OrdUnidimDenseSet::arity() const
{
  if (isEmpty())
    return 0;

  return pieces_.begin()->arity();
}

SetStratPtr OrdUnidimDenseSet::disjointCup(const SetStrategy& other) const
{
  OrdUnidimDenseSetCRef othr = static_cast<OrdUnidimDenseSetCRef>(other);
  
  // Special cases
  if (isEmpty())
    return std::make_unique<OrdUnidimDenseSet>(othr.pieces_);

  if (othr.isEmpty() || pieces_ == othr.pieces_)
    return std::make_unique<OrdUnidimDenseSet>(pieces_);  

  MDIOrdCollection result;
  result.reserve(pieces_.size() + othr.pieces_.size());
  if (pieces_.back() < othr.pieces_.front()) {
    result.insert(result.end(), pieces_.begin(), pieces_.end());
    result.insert(result.end(), othr.pieces_.begin(), othr.pieces_.end());
    return std::make_unique<OrdUnidimDenseSet>(result);
  }

  if (othr.pieces_.back() < pieces_.front()) {
    result.insert(result.end(), othr.pieces_.begin(), othr.pieces_.end());
    result.insert(result.end(), pieces_.begin(), pieces_.end());
    return std::make_unique<OrdUnidimDenseSet>(result);
  }
  
  MDIOrdCollection cup = traverse(&SetPiece::least, othr.pieces_);

  return std::make_unique<OrdUnidimDenseSet>(cup);
}

SetStratPtr OrdUnidimDenseSet::filterSet(bool (*f)(const SetPiece& mdi)) const
{
  MDIOrdCollection res;

  for (const SetPiece& mdi : pieces_)
    if (f(mdi))
      res.emplace_back(mdi);

  return std::make_unique<OrdUnidimDenseSet>(res);
}

SetStratPtr OrdUnidimDenseSet::offset(const MD_NAT& off) const
{
  MDIOrdCollection res;

  for (const SetPiece& mdi : pieces_)
    res.emplace_back(mdi.offset(off));

  return std::make_unique<OrdUnidimDenseSet>(res);
}

SetStratPtr OrdUnidimDenseSet::compact() const
{
  MDIOrdCollection res;

  if (isEmpty())
    return std::make_unique<OrdUnidimDenseSet>(res);

  auto next_it = pieces_.begin();
  ++next_it;
  SetPiece compacted = *pieces_.begin();
  for (auto it = pieces_.begin(); next_it != pieces_.end(); ++it) {
    MaybeMDI ith = compacted.compact(*next_it);
    if (!ith) {
      res.emplace_back(compacted);
      compacted = *next_it;
    } else {
      compacted = ith.value();
    }

    ++next_it;
  }
  res.emplace_back(compacted);

  return std::make_unique<OrdUnidimDenseSet>(res);
}

OrdUnidimDenseSet::MDIOrdCollection OrdUnidimDenseSet::boundedTraverse(
    SetPiece (SetPiece::*f)(const SetPiece&) const, const MDIOrdCollection& other
) const
{
  MDIOrdCollection res;

  if (isEmpty() || other.empty())
    return res;

  auto it1 = pieces_.begin();
  auto end1 = pieces_.end();
  auto it2 = other.begin();
  auto end2 = other.end();
  SetPiece mdi1;
  SetPiece mdi2;
  for (int j = 0; it1 != end1 && it2 != end2; ++j) {
    mdi1 = *it1;
    mdi2 = *it2;

    SetPiece funci = (mdi1.*f)(mdi2);
    if (!funci.isEmpty())
      res.emplace_back(funci);

    if (mdi1.maxElem() < mdi2.maxElem()) {
      ++it1;
    } else { 
      ++it2;
    }
  }

  return res;
}

OrdUnidimDenseSet::MDIOrdCollection OrdUnidimDenseSet::traverse(
    SetPiece (SetPiece::*f)(const SetPiece&) const, const MDIOrdCollection& other
) const
{
  MDIOrdCollection res;

  if (isEmpty())
    return other;

  if (other.empty())
    return pieces_;
  
  auto it1 = pieces_.begin();
  auto end1 = pieces_.end();
  auto it2 = other.begin();
  auto end2 = other.end();
  SetPiece mdi1;
  SetPiece mdi2;
  for (; it1 != end1 && it2 != end2;) {
    mdi1 = *it1;
    mdi2 = *it2;

    SetPiece funci = (mdi1.*f)(mdi2);
    if (!funci.isEmpty())
      res.emplace_back(funci);

    if (mdi1.maxElem() < mdi2.maxElem()) {
      ++it1;
    } else {
      ++it2;
    }
  }

  for (; it1 != end1; ++it1) {
    mdi1 = *it1;
    res.emplace_back(mdi1);
  }

  for (; it2 != end2; ++it2) {
    mdi2 = *it2;
    res.emplace_back(mdi2);
  }

  return res;
}

} // namespace LIB

} // namespace SBG
