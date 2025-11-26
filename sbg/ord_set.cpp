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
#include <forward_list>

#include "sbg/ord_set.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Ordered Set Implementation --------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

// Auxiliary functions - Ordered Sets ------------------------------------------

/**
 * @brief Checks if the both set pieces overlap.
 */
bool doInt(const SetPiece& mdi1, const SetPiece& mdi2)
{
  const auto max1 = mdi1.maxElem();
  const auto min1 = mdi1.minElem();
  const auto max2 = mdi2.maxElem();
  const auto min2 = mdi2.minElem();
  const unsigned int arity = mdi1.arity();

  for (unsigned int j = 0; j < arity; ++j) {
    if (max1[j] < min2[j] || max2[j] < min1[j])
      return false;
  }

  return true;
}

// Member functions - Ordered Sets ---------------------------------------------

member_imp(OrderedSet, OrderedSet::MDIOrdCollection, pieces);

OrderedSet::~OrderedSet() {}
OrderedSet::OrderedSet() : pieces_() {}
OrderedSet::OrderedSet(MD_NAT x) : pieces_() {
  pieces_.push_back(SetPiece(x));
}
OrderedSet::OrderedSet(Interval i) : pieces_() {
  if (!i.isEmpty())
    pieces_.push_back(SetPiece(i));
}
OrderedSet::OrderedSet(SetPiece mdi) : pieces_() {
  if (!mdi.isEmpty())
    pieces_.push_back(mdi);
}
OrderedSet::OrderedSet(OrderedSet::MDIOrdCollection pieces)
  : pieces_(std::move(pieces)) {}

SetStratPtr OrderedSet::clone() const
{
  return std::make_unique<OrderedSet>(*this);
}

member_imp(OrderedSet::Iterator, OrderedSet::MDIOrdCollection::const_iterator
  , it);

OrderedSet::Iterator::Iterator(OrderedSet::MDIOrdCollection::const_iterator it)
  : it_(it) {}

void OrderedSet::Iterator::operator++()
{
  ++it_;
  return;
}

bool OrderedSet::Iterator::operator!=(const SetStrategy::Iterator& other)
  const
{
  return it_ != static_cast<const OrderedSet::Iterator *>(&other)->it_;
}

bool OrderedSet::Iterator::operator==(const SetStrategy::Iterator& other) const
{
  return it_ == static_cast<const OrderedSet::Iterator *>(&other)->it_;
}

bool OrderedSet::Iterator::operator<(const SetStrategy::Iterator& other) const
{
  return it_ < static_cast<const OrderedSet::Iterator *>(&other)->it_;
}

const SetPiece& OrderedSet::Iterator::operator*() const { return *it_; }

std::shared_ptr<SetStrategy::Iterator> OrderedSet::begin() const
{
  return std::make_shared<OrderedSet::Iterator>(pieces_.begin());
}

std::shared_ptr<SetStrategy::Iterator> OrderedSet::end() const
{
  return std::make_shared<OrderedSet::Iterator>(pieces_.end());
}

std::size_t OrderedSet::size() const { return pieces_.size(); }

void OrderedSet::emplace(const SetPiece& mdi)
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
  for (; it != pieces_.end(); ++it) {
    if (mdi < *it)
      break;
  }
  pieces_.emplace(it, mdi);
    
  return;
}

void OrderedSet::emplaceBack(const SetPiece& mdi)
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

  auto it = pieces_.rbegin();
  for (; it != pieces_.rend(); ++it) {
    if (*it < mdi)
      break;
  }
  pieces_.emplace(it.base(), mdi);
    
  return;
}

void OrderedSet::emplaceHint(NAT hint, const SetPiece& mdi)
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
  std::advance(it, hint);
  for (; it != end; ++it) { 
    if (mdi < *it)
      break;
  }
  pieces_.emplace(it, mdi);

  return;
}

NAT OrderedSet::advanceHint(NAT hint, const SetPiece& mdi)
{ 
  auto it = pieces_.begin();
  auto end = pieces_.end();
  std::advance(it, hint);
  for (; it != end; ++it) {
    if (mdi < *it)
      break;
    ++hint;
  }
  
  return hint;
}

bool OrderedSet::operator==(const SetStrategy& other) const
{ 
  SetStratPtr this_comp = compact();
  OrdSetCRef ths = static_cast<OrdSetCRef>(*this_comp);
  SetStratPtr other_comp = other.compact();
  OrdSetCRef othr = static_cast<OrdSetCRef>(*other_comp);
  
  if (ths.pieces_ == othr.pieces_) {
    return true;
  } else {
    return (ths.difference(othr))->isEmpty()
     && (othr.difference(ths))->isEmpty();
  }
}

bool OrderedSet::operator!=(const SetStrategy& other) const
{
  return !(*this == other);
}

std::ostream& OrderedSet::print(std::ostream& out) const
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

unsigned int OrderedSet::cardinal() const
{
  unsigned int result = 0;

  for (const SetPiece& mdi : pieces_)
    result += mdi.cardinal();

  return result;
}

bool OrderedSet::isEmpty() const { return pieces_.empty(); }

MD_NAT OrderedSet::minElem() const
{
  return pieces_.begin()->minElem();
}

MD_NAT OrderedSet::maxElem() const
{ 
  MD_NAT res = pieces_.begin()->maxElem();
  for (const SetPiece& mdi : pieces_) {
    MD_NAT ith = mdi.maxElem();
    if (res < ith)
      res = ith;
  }
  
  return res;
}

SetStratPtr OrderedSet::intersection(const SetStrategy& other) const 
{     
  // Special cases
  if (isEmpty() || other.isEmpty())
    return std::make_unique<OrderedSet>();

  const MD_NAT min_elem = minElem();
  const MD_NAT max_elem = maxElem();
  const MD_NAT other_min = other.minElem();
  const MD_NAT other_max = other.maxElem();
  if (max_elem < other_min || other_max < min_elem)
    return std::make_unique<OrderedSet>();

  OrderedSet result;
  if (max_elem == other_min) {
    result.emplaceBack(SetPiece(max_elem));
    return std::make_unique<OrderedSet>(result);
  }

  if (min_elem == other_max) {
    result.emplaceBack(SetPiece(min_elem));
    return std::make_unique<OrderedSet>(result);
  }

  const OrdSetCRef othr = static_cast<OrdSetCRef>(other);
  if (pieces_ == othr.pieces_)
    return std::make_unique<OrderedSet>(pieces_);
 
  // General case
  MDIOrdCollection short_set = pieces_;
  MDIOrdCollection long_set = othr.pieces_;
  if (othr.pieces_.size() < pieces_.size()) {
    short_set = othr.pieces_;
    long_set  = pieces_;
  }
 
  // Indexes list corresponding to remaining pieces in short_set 
  std::forward_list<size_t> indexes;
  int short_size = short_set.size();
  for (int i = short_size - 1; i >= 0; --i) {
    indexes.push_front(i);
  }

  NAT global_pos = 0;
  auto short_begin = short_set.begin();
  for (const SetPiece& long_elem : long_set) {
    const MD_NAT long_min = long_elem.minElem();
    const MD_NAT long_max = long_elem.maxElem();

    auto prev_index = indexes.before_begin();
    auto curr_index = indexes.begin();
    global_pos = result.advanceHint(global_pos, long_elem);
    while (curr_index != indexes.end()) {
      const size_t idx = *curr_index;
      const SetPiece short_elem = *(short_begin + idx);
      const auto short_min = short_elem.minElem();
      const auto short_max = short_elem.maxElem();

      // Here short_elem is "before" long_elem, so it is also "before" all the
      // remaining sets in long_set, thus it can be discarded. 
      if (short_max < long_min) {
        curr_index = indexes.erase_after(prev_index);
        continue;
      }

      // Here short_elem is "after" long_elm, so no comparison is needed, and
      // the loop of long_set continues to check if this short_elem interacts
      // with the following elements of long_set. 
      if (long_max < short_min)
        break;

      if (doInt(short_elem,long_elem)) {
        const SetPiece inter = long_elem.intersection(short_elem);
        result.emplaceHint(global_pos, inter);
      }

      ++prev_index;
      ++curr_index;
    }

    if (indexes.empty())
        break;
  }

  return std::make_unique<OrderedSet>(result);
}

SetStratPtr OrderedSet::cup(const SetStrategy& other) const
{ 
  // Special cases
  OrdSetCRef othr = static_cast<OrdSetCRef>(other);
  if (isEmpty()) 
    return std::make_unique<OrderedSet>(othr.pieces_);

  if (other.isEmpty() || pieces_ == othr.pieces_)
    return std::make_unique<OrderedSet>(pieces_);

  const MD_NAT min_elem = minElem();
  const MD_NAT max_elem = maxElem();
  const MD_NAT other_min = other.minElem();
  const MD_NAT other_max = other.maxElem();
  MDIOrdCollection result;
  result.reserve(pieces_.size() + othr.pieces_.size());
  if (max_elem < other_min) {
    result.insert(result.end(), pieces_.begin(), pieces_.end());
    result.insert(result.end(), othr.pieces_.begin(), othr.pieces_.end());
    return std::make_unique<OrderedSet>(result);
  }

  if (other_max < min_elem) {
    result.insert(result.end(), othr.pieces_.begin(), othr.pieces_.end());
    result.insert(result.end(), pieces_.begin(), pieces_.end());
    return std::make_unique<OrderedSet>(result);
  }

  // General case
  SetStratPtr diff = difference(other);
  
  return othr.disjointCup(*diff);
}

SetStratPtr OrderedSet::complementAtom() const 
{
  MDIOrdCollection res;

  const SetPiece& mdi = *pieces_.begin();
  SetPiece dense_mdi;
  
  for (const Interval& i : mdi) 
    dense_mdi.emplaceBack(Interval(i.begin(), 1, i.end()));

  SetPiece during_mdi = dense_mdi;

  const Interval univ_one_dim(0, 1, Inf);
  SetPiece univ(mdi.arity(), univ_one_dim);

  unsigned int dim = 0;
  int global_pos = 0;

  for (const Interval& i : mdi) {
    int local_pos = global_pos;

    if (i.begin() != 0) {
      Interval i_res(0, 1, i.begin() - 1);
      if (!i_res.isEmpty()) {
        univ[dim] = i_res;
        res.insert(res.begin() + local_pos, univ);
        ++local_pos;
        ++global_pos;
        univ[dim] = univ_one_dim;
      }
    }

    if (i.begin() < Inf && i.step() > 1) {
      for (unsigned int j = 0; j < i.step() - 1; ++j) {
        Interval i_res(i.begin() + j + 1, i.step(), i.end());
        if (!i_res.isEmpty()) {
          during_mdi[dim] = i_res;
          res.insert(res.begin() + local_pos, during_mdi);
          ++local_pos;
        }
      }
    }
    
    if (i.end() < Inf) {
      Interval i_res(i.end() + 1, 1, Inf);
      if (!i_res.isEmpty()) {
        univ[dim] = i_res;
        res.insert(res.begin() + local_pos, univ);
        ++local_pos;
        univ[dim] = univ_one_dim;
      }
    }

    univ[dim] = dense_mdi[dim];
    during_mdi[dim] = i;
    ++dim;
  }
  
  return std::make_unique<OrderedSet>(res);
}

void OrderedSet::intersectionComp(const SetStrategy& other
  , const SetPiece& mdi, SetStrategy& rem) 
{
  // Special cases
  if (isEmpty() || other.isEmpty())
    return;

  OrderedSet result;
  const MD_NAT min_elem = minElem();
  const MD_NAT max_elem = maxElem();
  const MD_NAT other_min = other.minElem();
  const MD_NAT other_max = other.maxElem();
  if (max_elem < other_min || other_max < min_elem) {
    pieces_ = std::move(result.pieces_);
    return;
  }

  if (max_elem == other_min) {
    result.emplaceBack(SetPiece(max_elem));
    pieces_ = std::move(result.pieces_);
    return;
  }

  if (min_elem == other_max) {
    result.emplaceBack(SetPiece(min_elem));
    pieces_ = std::move(result.pieces_);
    return;
  }

  OrdSetCRef othr = static_cast<OrdSetCRef>(other);
  if (pieces_ == othr.pieces_) {
    pieces_ = std::move(result.pieces_);
    return;
  }

  // General case
  size_t i = 0;
  for (const SetPiece& elem : pieces_) {
    if (!(elem.maxElem() < mdi.minElem()))
      break;
    rem.emplaceBack(elem);
    ++i;
  } 

  NAT global_pos = 0;
  for (auto it = pieces_.begin() + i; it != pieces_.end(); ++it) {
    const SetPiece& elem = *it;

    global_pos = result.advanceHint(global_pos, elem);
    if (doInt(elem, mdi)) {
      for (const SetPiece& othr_elem : othr.pieces_) {
        if (doInt(othr_elem, elem)) {
          auto inter = elem.intersection(othr_elem);
          result.emplaceHint(global_pos, inter);
        }
      }
    } else {
      result.emplaceHint(global_pos, elem);
    }
  }

  pieces_ = std::move(result.pieces_);
  return;
}

SetStratPtr OrderedSet::complement() const
{ 
  if (isEmpty())
    return std::make_unique<OrderedSet>(); 
  
    
  auto first_it = pieces_.begin();
  SetPiece first = *first_it;
  OrderedSet result = static_cast<OrdSetCRef>(
    *(OrderedSet(first).complementAtom()));

  ++first_it;
  MDIOrdCollection second(first_it, pieces_.end());
  OrderedSet remnant;
  for (const SetPiece& mdi : second) {
    SetStratPtr c = OrderedSet(mdi).complementAtom();
    result.intersectionComp(*c, mdi, remnant);
  }
  
  return remnant.disjointCup(result);
}

SetStratPtr OrderedSet::difference(const SetStrategy& other) const
{ 
  // Special cases
  if (isEmpty() || other.isEmpty()) 
    return std::make_unique<OrderedSet>(pieces_);

  if (maxElem() < other.minElem() || other.maxElem() < minElem()) 
    return std::make_unique<OrderedSet>(pieces_);
 
  // General case 
  return intersection(*other.complement());
}

// Extra operations ------------------------------------------------------------

std::size_t OrderedSet::arity() const
{
  if (isEmpty())
    return 0;

  return pieces_.begin()->arity();
}

SetStratPtr OrderedSet::disjointCup(const SetStrategy& other) const
{
  OrdSetCRef othr = static_cast<OrdSetCRef>(other);
  
  // Special cases
  if (isEmpty())
    return std::make_unique<OrderedSet>(othr.pieces_);

  if (othr.isEmpty() || pieces_ == othr.pieces_)
    return std::make_unique<OrderedSet>(pieces_);  

  MDIOrdCollection result;
  result.reserve(pieces_.size() + othr.pieces_.size());
  if (pieces_.back() < othr.pieces_.front()) {
    result.insert(result.end(), pieces_.begin(), pieces_.end());
    result.insert(result.end(), othr.pieces_.begin(), othr.pieces_.end());
    return std::make_unique<OrderedSet>(result);
  }

  if (othr.pieces_.back() < pieces_.front()) {
    result.insert(result.end(), othr.pieces_.begin(), othr.pieces_.end());
    result.insert(result.end(), pieces_.begin(), pieces_.end());
    return std::make_unique<OrderedSet>(result);
  }

  // General case
  auto it1 = pieces_.begin();
  auto end1 = pieces_.end();
  auto it2 = othr.pieces_.begin();
  auto end2 = othr.pieces_.end();
  while (it1 != end1 && it2 != end2) {
    if (*it1 < *it2) {
      result.emplace_back(*it1);
      ++it1;
    } else {
      result.emplace_back(*it2); 
      ++it2;
    }
  }
  result.insert(result.end(), it1, end1);
  result.insert(result.end(), it2, end2);
  
  return std::make_unique<OrderedSet>(result);
}

SetStratPtr OrderedSet::filterSet(bool (*f)(const SetPiece& mdi)) const
{
  MDIOrdCollection res;

  for (const SetPiece& mdi : pieces_) {
    if (f(mdi))
      res.push_back(mdi);
  }

  return std::make_unique<OrderedSet>(res);
}

SetStratPtr OrderedSet::offset(const MD_NAT& off) const
{
  MDIOrdCollection res;

  for (const SetPiece& mdi : pieces_)
    res.emplace_back(mdi.offset(off));

  return std::make_unique<OrderedSet>(res);
}

SetStratPtr OrderedSet::compact() const
{
  MDIOrdCollection res;

  if (!isEmpty()) {
    std::set<SetPiece> prev(pieces_.begin(), pieces_.end()), actual = prev;
    do {
      prev = actual;
      actual = std::set<SetPiece>();

      std::set<SetPiece>::iterator ith = prev.begin(), last = prev.end();
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

    for (const SetPiece& mdi : actual) {
      res.emplace_back(mdi);
    }
  }

  return std::make_unique<OrderedSet>(res);
}

} // namespace LIB

} // namespace SBG
