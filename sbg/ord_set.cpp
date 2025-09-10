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


bool doInt(const SetPiece &mdi1, const SetPiece &mdi2)
{
  const auto max1 = mdi1.maxElem();
  const auto min1 = mdi1.minElem();
  const auto max2 = mdi2.maxElem();
  const auto min2 = mdi2.minElem();
  const unsigned int arity = mdi1.arity();

  for (unsigned int j = 0; j < arity; ++j) {
    if (max1[j] < min2[j] || max2[j] < min1[j]) {
      return false;  // No intersection detected
    }
  }
  return true;  // Intersection detected
}


void emplaceHint(OrderedSet::MDIOrdCollection &set, const SetPiece &mdi
  , NAT hint)
{
  auto end = set.end();
  auto it = set.begin();
  std::advance(it,hint);
  
  //Finding the position for mdi
  while (it != end) { 
    if (*it < mdi)
      ++it;
    else
      break;
  }

  set.insert(it, mdi);

  return;
}


void advanceHint(OrderedSet::MDIOrdCollection &set, const SetPiece &mdi
  , NAT &hint)
{ 
  auto end = set.end();
  auto it = set.begin();
  std::advance(it,hint);
  
  //Finding the position for mdi
  while (it != end) {
    if (*it < mdi){
      ++it;
      ++hint;
    }
    else
      break;
  }
  
  return;
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

bool OrderedSet::Iterator::operator!=(const SetStrategy::Iterator &other)
  const
{
  return it_ != static_cast<const OrderedSet::Iterator *>(&other)->it_;
}

const SetPiece &OrderedSet::Iterator::operator*() const { return *it_; }

std::shared_ptr<SetStrategy::Iterator> OrderedSet::begin() const
{
  return std::make_shared<OrderedSet::Iterator>(pieces_.begin());
}

std::shared_ptr<SetStrategy::Iterator> OrderedSet::end() const
{
  return std::make_shared<OrderedSet::Iterator>(pieces_.end());
}

std::size_t OrderedSet::size() const { return pieces_.size(); }

void OrderedSet::emplace(const SetPiece &mdi)
{
  if (!mdi.isEmpty())
    emplaceHint(pieces_, mdi, 0);
    
  return;
}
void OrderedSet::emplaceBack(const SetPiece &mdi)
{   
  if (!mdi.isEmpty()) {
    if (pieces_.empty() || pieces_.back() < mdi)  
      pieces_.push_back(mdi);
    else
      emplace(mdi);
  }
    
  return;
}

bool OrderedSet::operator==(const SetStrategy &other) const
{ 
  SetStratPtr this_comp = compact();
  OrdSetCRef ths = static_cast<OrdSetCRef>(*this_comp);
  SetStratPtr other_comp = other.compact();
  OrdSetCRef othr = static_cast<OrdSetCRef>(*other_comp);
  
  if(ths.pieces_ == othr.pieces_)
    return true;
  else
    return (ths.difference(othr))->isEmpty()
      && (othr.difference(ths))->isEmpty();
}

bool OrderedSet::operator!=(const SetStrategy &other) const
{
  return !(*this == other);
}

std::ostream &OrderedSet::print(std::ostream &out) const
{
  std::size_t sz = size();

  out << "{";
  if (sz > 0) {
    unsigned int j = 0;
    for (const SetPiece &mdi : pieces_) { 
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

// Set operations --------------------------------------------------------------

unsigned int OrderedSet::cardinal() const
{
  unsigned int result = 0;

  for (const SetPiece &mdi : pieces_)
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
  for (const SetPiece &mdi : pieces_) {
    MD_NAT ith = mdi.maxElem();
    if (res < ith)
      res = ith;
  }
  
  return res;
}


SetStratPtr OrderedSet::intersection(const SetStrategy &other) const 
{    
  const OrdSetCRef othr = static_cast<OrdSetCRef>(other);
  MDIOrdCollection inter;
  
  // Special cases
  if (isEmpty() || other.isEmpty())
    return std::make_unique<OrderedSet>(inter);

  if (pieces_ == othr.pieces_)
    return std::make_unique<OrderedSet>(pieces_);

  if (maxElem() < othr.minElem() || othr.maxElem() < minElem())
    return std::make_unique<OrderedSet>(inter);
  
  // General case
  const OrderedSet* short_set = this;
  const OrderedSet* long_set  = &othr;

  if (othr.pieces_.size() < pieces_.size()) {
     short_set = &othr;
     long_set  = this;
  }
  
  std::forward_list<size_t> short_indexes;
  auto li_it = short_indexes.before_begin();
  const size_t short_size = short_set->pieces_.size();
  for (size_t i = 0; i < short_size; ++i)
    li_it = short_indexes.insert_after(li_it, i);

  const auto &long_pieces = long_set->pieces_;
  auto short_begin = short_set->pieces_.begin();

  NAT global_pos = 0;

  for (const auto& long_elem : long_pieces) {
    const auto long_min = long_elem.minElem();
    const auto long_max = long_elem.maxElem();

    auto li_prev = short_indexes.before_begin();
    auto li_curr = short_indexes.begin();
    
    advanceHint(inter, long_elem, global_pos);

    while (li_curr != short_indexes.end()) {
      const size_t idx = *li_curr;
      const SetPiece short_elem = *(short_begin + idx);

      const auto short_min = short_elem.minElem();
      const auto short_max = short_elem.maxElem();

      if (short_max < long_min) {
        li_curr = short_indexes.erase_after(li_prev);
        continue;
      }

      if (long_max < short_min)
        break;

      if (doInt(short_elem,long_elem)) {
        const auto inter_res = long_elem.intersection(short_elem);
        if (!inter_res.isEmpty()) 
          emplaceHint(inter, inter_res, global_pos);
      }

      ++li_prev;
      ++li_curr;
    }

    if (short_indexes.empty())
        break;
  }

  return std::make_unique<OrderedSet>(inter);
}



SetStratPtr OrderedSet::cup(const SetStrategy &other) const
{
  OrdSetCRef othr = static_cast<OrdSetCRef>(other);
  
  // Special cases
  if (isEmpty()) 
    return std::make_unique<OrderedSet>(othr.pieces_);

  if (other.isEmpty() || pieces_ == othr.pieces_)
    return std::make_unique<OrderedSet>(pieces_);

  if (maxElem() < othr.minElem()) {
    MDIOrdCollection result;
    result.reserve(pieces_.size() + othr.pieces_.size());
    result.insert(result.end(), pieces_.begin(), pieces_.end());
    result.insert(result.end(), othr.pieces_.begin(), othr.pieces_.end());
    return std::make_unique<OrderedSet>(result);
  }

  if (othr.maxElem() < minElem()) {
    MDIOrdCollection result;
    result.reserve(othr.pieces_.size() + pieces_.size());
    result.insert(result.end(), othr.pieces_.begin(), othr.pieces_.end());
    result.insert(result.end(), pieces_.begin(), pieces_.end());
    return std::make_unique<OrderedSet>(result);
  }

  // General case
  SetStratPtr diff = difference(other);
  OrdSetCRef diff_cast = static_cast<OrdSetCRef>(*diff);
  
  return othr.disjointCup(diff_cast);
}


SetStratPtr OrderedSet::complementAtom() const 
{
  MDIOrdCollection res;

  const SetPiece& mdi = *pieces_.begin();
  SetPiece dense_mdi;
  
  for (const Interval& i : mdi) 
    dense_mdi.emplaceBack(Interval(i.begin(), 1, i.end()));
  

  SetPiece during_mdi = dense_mdi;

  const Interval univ(0, 1, Inf);
  SetPiece all(mdi.arity(), univ);

  unsigned int dim = 0;
  int global_pos = 0;

  for (const Interval& i : mdi) {
    int local_pos = global_pos;

    if (i.begin() != 0) {
      Interval i_res(0, 1, i.begin() - 1);
      if (!i_res.isEmpty()) {
        all[dim] = i_res;
        res.insert(res.begin() + local_pos, all);
        ++local_pos;
        ++global_pos;
        all[dim] = univ;
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
        all[dim] = i_res;
        res.insert(res.begin() + local_pos, all);
        ++local_pos;
        all[dim] = univ;
      }
    }

    all[dim] = dense_mdi[dim];
    during_mdi[dim] = i;
    ++dim;
  }
  
  return std::make_unique<OrderedSet>(res);
}


SetStratPtr OrderedSet::intersectionComp(const SetStrategy &other
  , const SetPiece &mdi, SetStrategy &rem) const 
{
  OrdSetCRef othr = static_cast<OrdSetCRef>(other);
  MDIOrdCollection inter;

  if (isEmpty() || other.isEmpty())
      return std::make_unique<OrderedSet>(inter);

  if (pieces_ == othr.pieces_)
      return std::make_unique<OrderedSet>(pieces_);

  OrdSetRef remnant = static_cast<OrdSetRef>(rem);
  size_t pos = 0;
  while (pos < pieces_.size() && pieces_[pos].maxElem() < mdi.minElem()) {
      remnant.pieces_.emplace_back(pieces_[pos]);
      ++pos;
  } 

  NAT global_pos = 0;
  for (auto current = pieces_.begin() + pos; current != pieces_.end()
    ; ++current) {
    const SetPiece &elem = *current;

    advanceHint(inter, elem, global_pos);

    bool do_intersection = doInt(elem, mdi);

    if (do_intersection) {
      for (size_t i = 0; i < othr.pieces_.size(); ++i) {
        const SetPiece &othr_elem = othr.pieces_[i];
        if (doInt(othr_elem, elem)) {
          auto inter_res = elem.intersection(othr_elem);
          if (!inter_res.isEmpty())
            emplaceHint(inter, inter_res, global_pos);
        }
      }
    } else {
      emplaceHint(inter, elem, global_pos);
    }
  }

  return std::make_unique<OrderedSet>(inter);
}




SetStratPtr OrderedSet::complement() const
{ 
  if(isEmpty())
    return std::make_unique<OrderedSet>(pieces_);  
  
  OrderedSet res;
  OrderedSet remnant;
    
  auto first_it = pieces_.begin();
  SetPiece first = *first_it;
  res = static_cast<OrdSetCRef>(*(OrderedSet(first).complementAtom()));

  ++first_it;
  MDIOrdCollection second(first_it, pieces_.end());
  for (const SetPiece &mdi : second) {
    SetStratPtr c = OrderedSet(mdi).complementAtom();
    res = static_cast<OrdSetCRef>(*(res.intersectionComp(*c,mdi,remnant)));
  }
  
  return remnant.disjointCup(res);
}

SetStratPtr OrderedSet::difference(const SetStrategy &other) const
{ 
  OrdSetCRef othr = static_cast<OrdSetCRef>(other);
  
  // Special cases
  if (isEmpty() || othr.isEmpty()) 
    return std::make_unique<OrderedSet>(pieces_);

  if (maxElem() < othr.minElem() || other.maxElem() < minElem()) 
    return std::make_unique<OrderedSet>(pieces_);
  
  return intersection(*othr.complement());
}

// Extra operations ------------------------------------------------------------

std::size_t OrderedSet::arity() const
{
  if (isEmpty())
    return 0;

  return pieces_.begin()->arity();
}

SetStratPtr OrderedSet::disjointCup(const SetStrategy &other) const
{
  OrdSetCRef othr = static_cast<OrdSetCRef>(other);
  
  // Special cases
  if (isEmpty())
    return std::make_unique<OrderedSet>(othr.pieces_);

  if (othr.isEmpty())
    return std::make_unique<OrderedSet>(pieces_);  

  MDIOrdCollection res;
  res.reserve(pieces_.size() + othr.pieces_.size());
  
  if (pieces_.back() < othr.pieces_.front()) {
    res.insert(res.end(), pieces_.begin(), pieces_.end());
    res.insert(res.end(), othr.pieces_.begin(), othr.pieces_.end());
    return std::make_unique<OrderedSet>(res);
  }

  if (othr.pieces_.back() < pieces_.front()) {
    res.insert(res.end(), othr.pieces_.begin(), othr.pieces_.end());
    res.insert(res.end(), pieces_.begin(), pieces_.end());
    return std::make_unique<OrderedSet>(res);
  }

  // General cases
  auto it1 = pieces_.begin(), it2 = othr.pieces_.begin();
  auto end1 = pieces_.end(), end2 = othr.pieces_.end();

  while (it1 != end1 && it2 != end2) {
    if (*it1 < *it2) {
      res.emplace_back(*it1);
      ++it1;
    } else {
      res.emplace_back(*it2); 
      ++it2;
    }
  }

  res.insert(res.end(), it1, end1);
  res.insert(res.end(), it2, end2);
  
  return std::make_unique<OrderedSet>(res);
}

SetStratPtr OrderedSet::filterSet(bool (*f)(const SetPiece &mdi)) const
{
  MDIOrdCollection res;

  for (const SetPiece &mdi : pieces_)
    if (f(mdi))
      res.push_back(mdi);

  return std::make_unique<OrderedSet>(res);
}

SetStratPtr OrderedSet::offset(const MD_NAT &off) const
{
  MDIOrdCollection res;

  for (const SetPiece &mdi : pieces_)
    res.push_back(mdi.offset(off));

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

    for (const SetPiece &mdi : actual)
      res.push_back(mdi);
  }

  return std::make_unique<OrderedSet>(res);
}

} // namespace LIB

} // namespace SBG
