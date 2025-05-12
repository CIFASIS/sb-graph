/*******************************************************************************

 This file is part of Set--Based Graph Library.

 SBG Library is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 SBG Library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the delegied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with SBG Library.  If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#include "sbg/set.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Set Delegate Constructors ---------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

SetDelegate::SetDelegate() {}
SetDelegate::SetDelegate(const MD_NAT &x) {}
SetDelegate::SetDelegate(const Interval &i) {}
SetDelegate::SetDelegate(const SetPiece &mdi) {}

////////////////////////////////////////////////////////////////////////////////
// Unordered Set Implementation ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

member_imp(UnorderedSet, MDIUnordSet, pieces);

UnorderedSet::~UnorderedSet() {}
UnorderedSet::UnorderedSet() : pieces_() {}
UnorderedSet::UnorderedSet(const MD_NAT &x) : pieces_() {
  pieces_.push_back(SetPiece(x));
}
UnorderedSet::UnorderedSet(const Interval &i) : pieces_() {
  if (!i.isEmpty())
    pieces_.push_back(SetPiece(i));
}
UnorderedSet::UnorderedSet(const SetPiece &mdi) : pieces_() {
  if (!mdi.isEmpty())
    pieces_.push_back(mdi);
}
UnorderedSet::UnorderedSet(const MDIUnordSet &pieces)
  : pieces_(std::move(pieces)) {}

SetDelegPtr UnorderedSet::clone() const
{
  return std::make_unique<UnorderedSet>(*this);
}

member_imp(UnorderedSet::Iterator, MDIUnordSet::const_iterator, it);

UnorderedSet::Iterator::Iterator(MDIUnordSet::const_iterator it) : it_(it) {}

void UnorderedSet::Iterator::operator++()
{
  ++it_;
  return;
}

bool UnorderedSet::Iterator::operator!=(const SetDelegate::Iterator &other)
  const
{
  return it_ != static_cast<const UnorderedSet::Iterator *>(&other)->it_;
}

const SetPiece &UnorderedSet::Iterator::operator*() const { return *it_; }

std::shared_ptr<SetDelegate::Iterator> UnorderedSet::begin() const
{
  return std::make_shared<UnorderedSet::Iterator>(pieces_.begin());
}

std::shared_ptr<SetDelegate::Iterator> UnorderedSet::end() const
{
  return std::make_shared<UnorderedSet::Iterator>(pieces_.end());
}

std::size_t UnorderedSet::size() const { return pieces_.size(); }

void UnorderedSet::emplace(const SetPiece &mdi)
{
  if (!mdi.isEmpty())
    pieces_.push_back(mdi);
  return;
}
void UnorderedSet::emplaceBack(const SetPiece &mdi)
{
  if (!mdi.isEmpty())
    pieces_.push_back(mdi);
  return;
}

bool UnorderedSet::operator==(const SetDelegate &other) const
{
  UnordSetCRef othr = static_cast<UnordSetCRef>(other);

  if (pieces_ == othr.pieces_)
    return true;

  return difference(other)->isEmpty() && other.difference(*this)->isEmpty();
}

bool UnorderedSet::operator!=(const SetDelegate &other) const
{
  return !(*this == other);
}

std::ostream &UnorderedSet::print(std::ostream &out) const
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

unsigned int UnorderedSet::cardinal() const
{
  unsigned int result = 0;

  for (const SetPiece &mdi : pieces_)
    result += mdi.cardinal();

  return result;
}

bool UnorderedSet::isEmpty() const { return pieces_.empty(); }

MD_NAT UnorderedSet::minElem() const
{
  MD_NAT res = pieces_.begin()->minElem();
  for (const SetPiece &mdi : pieces_) {
    MD_NAT ith = mdi.minElem();
    if (ith < res)
      res = ith;
  }
  
  return res;
}

MD_NAT UnorderedSet::maxElem() const
{
  MD_NAT res = pieces_.begin()->maxElem();
  for (const SetPiece &mdi : pieces_) {
    MD_NAT ith = mdi.maxElem();
    if (res < ith)
      res = ith;
  }
  
  return res;
}

SetDelegPtr UnorderedSet::intersection(const SetDelegate &other) const
{
  MDIUnordSet res;

  // Special cases to enhance performance
  if (isEmpty() || other.isEmpty())
    return std::make_unique<UnorderedSet>(res);

  if (maxElem() < other.minElem())
    return std::make_unique<UnorderedSet>(res);

  if (other.maxElem() < minElem()) 
    return std::make_unique<UnorderedSet>(res);

  if (maxElem() == other.minElem()) {
    res.push_back(SetPiece(maxElem()));
    return std::make_unique<UnorderedSet>(res);
  }

  if (other.maxElem() == minElem()) {
    res.push_back(SetPiece(minElem()));
    return std::make_unique<UnorderedSet>(res);
  }

  UnordSetCRef othr = static_cast<UnordSetCRef>(other);
  if (pieces_ == othr.pieces_)
    return std::make_unique<UnorderedSet>(pieces_);

  UnorderedSet aux_res;
  for (const SetPiece &mdi1 : pieces_)
    for (const SetPiece &mdi2 : othr.pieces_)
      aux_res.emplace(mdi1.intersection(mdi2));

  return std::make_unique<UnorderedSet>(aux_res.pieces_);
}

SetDelegPtr UnorderedSet::cup(const SetDelegate &other) const
{
  UnordSetCRef othr = static_cast<UnordSetCRef>(other);

  if (isEmpty()) 
    return std::make_unique<UnorderedSet>(othr.pieces_);

  if (other.isEmpty() || pieces_ == othr.pieces_)
    return std::make_unique<UnorderedSet>(pieces_);

  if (maxElem() < othr.minElem()) {
    MDIUnordSet un(pieces_.begin(), pieces_.end());

    for (const SetPiece &mdi : othr.pieces_) 
      un.push_back(mdi);

    return std::make_unique<UnorderedSet>(un);
  }

  if (othr.maxElem() < minElem()) {
    MDIUnordSet un(othr.pieces_.begin(), othr.pieces_.end());

    for (const SetPiece &mdi : pieces_) 
      un.push_back(mdi);

    return std::make_unique<UnorderedSet>(un);
  }

  SetDelegPtr diff = difference(other);
  UnordSetCRef diff_cast = static_cast<UnordSetCRef>(*diff);

  return othr.disjointCup(diff_cast);
}

SetDelegPtr UnorderedSet::complementAtom() const
{
  MDIUnordSet res;

  SetPiece mdi = *pieces_.begin();
  SetPiece dense_mdi;
  for (const Interval &i : mdi)
    dense_mdi.emplaceBack(Interval(i.begin(), 1, i.end()));
  SetPiece during_mdi = dense_mdi;

  Interval univ(0, 1, Inf);
  SetPiece all(mdi.arity(), univ);

  unsigned int dim = 0;
  for (const Interval &i : mdi) {
    MDIUnordSet c;

    // Before interval
    if (i.begin() != 0) {
      Interval i_res(0, 1, i.begin() - 1);
      if (!i_res.isEmpty()) {
        all[dim] = i_res;
        c.push_back(all);
        all[dim] = univ;
      }
    }

    // "During" interval
    if (i.begin() < Inf) {
      if (i.step() > 1) {
        for (unsigned int j = 0; j < i.step() - 1; ++j) {
          Interval i_res(i.begin() + j + 1, i.step(), i.end());
          if (!i_res.isEmpty()) {
            during_mdi[dim] = i_res;
            c.push_back(during_mdi);
          }
        }
      }
    }

    // After interval
    if (i.end() < Inf) {
      Interval i_res(i.end() + 1, 1, Inf);
      if (!i_res.isEmpty()) {
        all[dim] = i_res;
        c.push_back(all);
        all[dim] = univ;
      }
    }
    all[dim] = dense_mdi[dim];
    during_mdi[dim] = i;

    // Insert results of current dim
    for (const SetPiece &mdi : c)
      res.push_back(mdi);

    ++dim;
  }

  return std::make_unique<UnorderedSet>(res);
}

SetDelegPtr UnorderedSet::complement() const
{
  SetDelegPtr res = std::make_unique<UnorderedSet>(MDIUnordSet());

  auto first_it = pieces_.begin();
  SetPiece first = *first_it;
  res = std::move(UnorderedSet(first).complementAtom());

  ++first_it;
  MDIUnordSet second(first_it, pieces_.end());
  for (const SetPiece &mdi : second) {
    SetDelegPtr c = UnorderedSet(mdi).complementAtom();
    res = std::move(res->intersection(*c));
  }

  return res;
}

SetDelegPtr UnorderedSet::difference(const SetDelegate &other) const
{
  if (isEmpty() || other.isEmpty())
    return std::make_unique<UnorderedSet>(*this);

  UnordSetCRef othr = static_cast<UnordSetCRef>(other);
  return intersection(*othr.complement());
}

// Extra operations ------------------------------------------------------------

std::size_t UnorderedSet::arity() const
{
  if (isEmpty())
    return 0;

  return pieces_.begin()->arity();
}

SetDelegPtr UnorderedSet::disjointCup(const SetDelegate &other) const
{
  UnordSetCRef othr = static_cast<UnordSetCRef>(other);

  if (isEmpty()) 
    return std::make_unique<UnorderedSet>(othr.pieces_);

  if (other.isEmpty() || pieces_ == othr.pieces_)
    return std::make_unique<UnorderedSet>(pieces_);

  MDIUnordSet res(pieces_.begin(), pieces_.end());
  for (const SetPiece &mdi : othr.pieces_)
    res.push_back(mdi);

  return std::make_unique<UnorderedSet>(res);
}

SetDelegPtr UnorderedSet::filterSet(bool (*f)(const SetPiece &mdi)) const
{
  MDIUnordSet res;

  for (const SetPiece &mdi : pieces_)
    if (f(mdi))
      res.push_back(mdi);

  return std::make_unique<UnorderedSet>(res);
}

SetDelegPtr UnorderedSet::offset(const MD_NAT &off) const
{
  MDIUnordSet res;

  for (const SetPiece &mdi : pieces_)
    res.push_back(mdi.offset(off));

  return std::make_unique<UnorderedSet>(res);
}

SetDelegPtr UnorderedSet::compact() const
{
  // New idea TODO
  //MDIUnordSet old_compact = pieces_, compact = old_compact;
  //SetPiece ith(compact.begin());
  //do {
  //  for (const SetPiece &mdi : compact) {
  //    auto ith_compact = ith.compact(mdi);
  //    if (ith_compact) 
  //      ith = ith_compact.value();
  //  }
  //  MDIUnordSet aux_compact = compact;
  //  for (const SetPiece &mdi : aux_compact) {
  //    if (!ith.intersection(mdi).isEmpty())
  //      compact.erase(mdi);
  //  }
  //  compact.emplace(ith);
  //} while (old_compact != compact);

  //std::shared_ptr<UnorderedSet> res = std::make_shared<UnorderedSet>();
  //res->pieces_ = pieces_.compact();

  return std::make_unique<UnorderedSet>(pieces_);
}

////////////////////////////////////////////////////////////////////////////////
// Ordered Set Implementation (1 dimension, dense intervals) -------------------
////////////////////////////////////////////////////////////////////////////////

member_imp(OrderedDenseSet, MDIOrdSet, pieces);

OrderedDenseSet::~OrderedDenseSet() {}
OrderedDenseSet::OrderedDenseSet() : pieces_() {}
OrderedDenseSet::OrderedDenseSet(MD_NAT x) : pieces_() {
  pieces_.push_back(SetPiece(x));
}
OrderedDenseSet::OrderedDenseSet(Interval i) : pieces_() {
  if (!i.isEmpty())
    pieces_.push_back(SetPiece(i));
}
OrderedDenseSet::OrderedDenseSet(SetPiece mdi) : pieces_() {
  if (!mdi.isEmpty())
    pieces_.push_back(mdi);
}
OrderedDenseSet::OrderedDenseSet(MDIOrdSet pieces)
  : pieces_(std::move(pieces)) {}

SetDelegPtr OrderedDenseSet::clone() const
{
  return std::make_unique<OrderedDenseSet>(*this);
}

member_imp(OrderedDenseSet::Iterator, MDIOrdSet::const_iterator, it);

OrderedDenseSet::Iterator::Iterator(MDIOrdSet::const_iterator it) : it_(it) {}

void OrderedDenseSet::Iterator::operator++()
{
  ++it_;
  return;
}

bool OrderedDenseSet::Iterator::operator!=(const SetDelegate::Iterator &other)
  const
{
  return it_ != static_cast<const OrderedDenseSet::Iterator *>(&other)->it_;
}

const SetPiece &OrderedDenseSet::Iterator::operator*() const { return *it_; }

std::shared_ptr<SetDelegate::Iterator> OrderedDenseSet::begin() const
{
  return std::make_shared<OrderedDenseSet::Iterator>(pieces_.begin());
}

std::shared_ptr<SetDelegate::Iterator> OrderedDenseSet::end() const
{
  return std::make_shared<OrderedDenseSet::Iterator>(pieces_.end());
}

std::size_t OrderedDenseSet::size() const { return pieces_.size(); }

void OrderedDenseSet::emplace(const SetPiece &mdi)
{
  if (!mdi.isEmpty())
    pieces_.push_back(mdi);
  return;
}
void OrderedDenseSet::emplaceBack(const SetPiece &mdi)
{
  if (!mdi.isEmpty())
    pieces_.emplace(pieces_.end(), mdi);
  return;
}

bool OrderedDenseSet::operator==(const SetDelegate &other) const
{
  SetDelegPtr this_comp = compact();
  OrdDenseSetCRef ths = static_cast<OrdDenseSetCRef>(*this_comp);
  SetDelegPtr other_comp = other.compact();
  OrdDenseSetCRef othr = static_cast<OrdDenseSetCRef>(*other_comp);

  return ths.pieces_ == othr.pieces_;
}

bool OrderedDenseSet::operator!=(const SetDelegate &other) const
{
  return !(*this == other);
}

std::ostream &OrderedDenseSet::print(std::ostream &out) const
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

unsigned int OrderedDenseSet::cardinal() const
{
  unsigned int result = 0;

  for (const SetPiece &mdi : pieces_)
    result += mdi.cardinal();

  return result;
}

bool OrderedDenseSet::isEmpty() const { return pieces_.empty(); }

MD_NAT OrderedDenseSet::minElem() const
{
  return pieces_.begin()->minElem();
}

MD_NAT OrderedDenseSet::maxElem() const
{
  auto it = pieces_.end();
  --it;
  return it->maxElem();
}

SetDelegPtr OrderedDenseSet::intersection(const SetDelegate &other) const
{
  MDIOrdSet res;

  // Special cases to enhance performance
  if (isEmpty() || other.isEmpty()) 
    return std::make_unique<OrderedDenseSet>(res);

  if (maxElem() < other.minElem()) 
    return std::make_unique<OrderedDenseSet>(res);

  if (other.maxElem() < minElem()) 
    return std::make_unique<OrderedDenseSet>(res);

  if (maxElem() == other.minElem()) {
    res.push_back(SetPiece(maxElem()));
    return std::make_unique<OrderedDenseSet>(res);
  }

  if (other.maxElem() == minElem()) {
    res.push_back(SetPiece(minElem()));
    return std::make_unique<OrderedDenseSet>(res);
  }

  OrdDenseSetCRef othr = static_cast<OrdDenseSetCRef>(other);
  if (pieces_ == othr.pieces_)
    return std::make_unique<OrderedDenseSet>(pieces_);

  MDIOrdSet cap = boundedTraverse(&SetPiece::intersection, othr.pieces_);

  return std::make_unique<OrderedDenseSet>(cap);
}

SetDelegPtr OrderedDenseSet::cup(const SetDelegate &other) const
{
  OrdDenseSetCRef othr = static_cast<OrdDenseSetCRef>(other);

  if (isEmpty()) 
    return std::make_unique<OrderedDenseSet>(othr.pieces_);

  if (other.isEmpty() || pieces_ == othr.pieces_)
    return std::make_unique<OrderedDenseSet>(pieces_);

  if (maxElem() < othr.minElem()) {
    MDIOrdSet un(pieces_.begin(), pieces_.end());

    for (const SetPiece &mdi : othr.pieces_) 
      un.push_back(mdi);

    return std::make_unique<OrderedDenseSet>(un);
  }

  if (othr.maxElem() < minElem()) {
    MDIOrdSet un(othr.pieces_.begin(), othr.pieces_.end());

    for (const SetPiece &mdi : pieces_) 
      un.push_back(mdi);

    return std::make_unique<OrderedDenseSet>(un);
  }

  SetDelegPtr diff = difference(other);
  OrdDenseSetCRef diff_cast = static_cast<OrdDenseSetCRef>(*diff);

  return othr.disjointCup(diff_cast);
}

SetDelegPtr OrderedDenseSet::complement() const
{
  OrderedDenseSet res;

  if (isEmpty()) {
    res.emplaceBack(SetPiece(Interval(0, 1, Inf))); 
    return std::make_unique<OrderedDenseSet>(res);
  }

  auto first_it = pieces_.begin();
  Interval first(0, 1, first_it->operator[](0).begin() - 1);
  if (!first.isEmpty())
    res.emplaceBack(SetPiece(first));
  NAT last = first_it->maxElem()[0];

  ++first_it;
  MDIOrdSet second(first_it, pieces_.end());
  for (const SetPiece &mdi : second) {
    Interval ith(last + 1, 1, mdi[0].begin() - 1);
    if (!ith.isEmpty())
      res.emplaceBack(SetPiece(ith));
    last = mdi.maxElem()[0];
  }
  Interval end(last + 1, 1, Inf);
  res.emplaceBack(SetPiece(end));

  return std::make_unique<OrderedDenseSet>(res);
}

SetDelegPtr OrderedDenseSet::difference(const SetDelegate &other) const
{
  if (isEmpty() || other.isEmpty())
    return std::make_unique<OrderedDenseSet>(*this);

  OrdDenseSetCRef othr = static_cast<OrdDenseSetCRef>(other);
  return intersection(*othr.complement());
}

// Extra operations ------------------------------------------------------------

std::size_t OrderedDenseSet::arity() const
{
  if (isEmpty())
    return 0;

  return pieces_.begin()->arity();
}

SetDelegPtr OrderedDenseSet::disjointCup(const SetDelegate &other) const
{
  OrdDenseSetCRef othr = static_cast<OrdDenseSetCRef>(other);
  MDIOrdSet cup = traverse(&SetPiece::least, othr.pieces_);

  return std::make_unique<OrderedDenseSet>(cup);
}

SetDelegPtr OrderedDenseSet::filterSet(bool (*f)(const SetPiece &mdi)) const
{
  MDIOrdSet res;

  for (const SetPiece &mdi : pieces_)
    if (f(mdi))
      res.push_back(mdi);

  return std::make_unique<OrderedDenseSet>(res);
}

SetDelegPtr OrderedDenseSet::offset(const MD_NAT &off) const
{
  MDIOrdSet res;

  for (const SetPiece &mdi : pieces_)
    res.push_back(mdi.offset(off));

  return std::make_unique<OrderedDenseSet>(res);
}

SetDelegPtr OrderedDenseSet::compact() const
{
  MDIOrdSet res;

  if (isEmpty())
    return std::make_unique<OrderedDenseSet>(res);

  auto next_it = pieces_.begin();
  ++next_it;
  SetPiece compacted = *pieces_.begin();
  for (auto it = pieces_.begin(); next_it != pieces_.end(); ++it) {
    MaybeMDI ith = compacted.compact(*next_it);
    if (!ith) {
      res.push_back(compacted);
      compacted = *next_it;
    }
    else
      compacted = ith.value();

    ++next_it;
  }
  res.push_back(compacted);

  return std::make_unique<OrderedDenseSet>(res);
}

MDIOrdSet OrderedDenseSet::boundedTraverse(
    SetPiece (SetPiece::*f)(const SetPiece &) const, const MDIOrdSet &other
) const
{
  MDIOrdSet res;

  if (isEmpty() || other.empty())
    return res;

  auto it1 = pieces_.begin(), it2 = other.begin();
  auto end1 = pieces_.end(), end2 = other.end();

  SetPiece mdi1, mdi2;
  for (int j = 0; it1 != end1 && it2 != end2; ++j) {
    mdi1 = *it1;
    mdi2 = *it2;

    SetPiece funci = (mdi1.*f)(mdi2);
    if (!funci.isEmpty())
      res.emplace(res.end(), funci);

    if (mdi1.maxElem() < mdi2.maxElem())
      ++it1;
    else 
      ++it2;
  }

  return res;
}

MDIOrdSet OrderedDenseSet::traverse(
    SetPiece (SetPiece::*f)(const SetPiece &) const, const MDIOrdSet &other
) const
{
  MDIOrdSet res;

  if (isEmpty())
    return other;

  if (other.empty())
    return pieces_;
  
  auto it1 = pieces_.begin(), it2 = other.begin();
  auto end1 = pieces_.end(), end2 = other.end();

  SetPiece mdi1, mdi2;
  for (; it1 != end1 && it2 != end2;) {
    mdi1 = *it1;
    mdi2 = *it2;

    SetPiece funci = (mdi1.*f)(mdi2);
    if (!funci.isEmpty())
      res.emplace(res.end(), funci);

    if (mdi1.maxElem() < mdi2.maxElem())
      ++it1;
    else
      ++it2;
  }

  for (; it1 != end1; ++it1) {
    mdi1 = *it1;
    res.emplace(res.end(), mdi1);
  }

  for (; it2 != end2; ++it2) {
    mdi2 = *it2;
    res.emplace(res.end(), mdi2);
  }

  return res;
}


////////////////////////////////////////////////////////////////////////////////
// Ordered Set Implementation --------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

member_imp(OrderedSet, MDIOrdSet, pieces);

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
OrderedSet::OrderedSet(MDIOrdSet pieces): pieces_(std::move(pieces)) {}

SetDelegPtr OrderedSet::clone() const
{
  return std::make_unique<OrderedSet>(*this);
}

member_imp(OrderedSet::Iterator, MDIOrdSet::const_iterator, it);

OrderedSet::Iterator::Iterator(MDIOrdSet::const_iterator it) : it_(it) {}

void OrderedSet::Iterator::operator++()
{
  ++it_;
  return;
}

bool OrderedSet::Iterator::operator!=(const SetDelegate::Iterator &other)
  const
{
  return it_ != static_cast<const OrderedSet::Iterator *>(&other)->it_;
}

const SetPiece &OrderedSet::Iterator::operator*() const { return *it_; }

std::shared_ptr<SetDelegate::Iterator> OrderedSet::begin() const
{
  return std::make_shared<OrderedSet::Iterator>(pieces_.begin());
}

std::shared_ptr<SetDelegate::Iterator> OrderedSet::end() const
{
  return std::make_shared<OrderedSet::Iterator>(pieces_.end());
}

std::size_t OrderedSet::size() const { return pieces_.size(); }

void OrderedSet::emplace(const SetPiece &mdi)
{
  if (!mdi.isEmpty()){
    auto it = pieces_.begin();
    auto fin = pieces_.end();
    
    while (it != fin && *it < mdi) 
        ++it;

    pieces_.insert(it, std::move(mdi));
  }
  return;
}
void OrderedSet::emplaceBack(const SetPiece &mdi)
{
  if (!mdi.isEmpty())
    pieces_.emplace(pieces_.end(), mdi);
  return;
}

bool OrderedSet::operator==(const SetDelegate &other) const
{
  SetDelegPtr this_comp = compact();
  OrdSetCRef ths = static_cast<OrdSetCRef>(*this_comp);
  SetDelegPtr other_comp = other.compact();
  OrdSetCRef othr = static_cast<OrdSetCRef>(*other_comp);

  return ths.pieces_ == othr.pieces_;
}

bool OrderedSet::operator!=(const SetDelegate &other) const
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


SetDelegPtr OrderedSet::intersection(const SetDelegate &other) const 
{
    const OrdSetCRef othr = static_cast<OrdSetCRef>(other);
    MDIOrdSet inter;

    if (isEmpty() || other.isEmpty())
        return std::make_unique<OrderedSet>(inter);

    if (pieces_ == othr.pieces_)
        return std::make_unique<OrderedSet>(pieces_);

    if (maxElem() < othr.minElem() || othr.maxElem() < minElem())
        return std::make_unique<OrderedSet>(inter);

    const OrderedSet* shortSet = this;
    const OrderedSet* longSet  = &othr;

    if (othr.pieces_.size() < pieces_.size()) {
        shortSet = &othr;
        longSet  = this;
    }

    std::forward_list<size_t> longIndices;
    auto liIt = longIndices.before_begin();

    const size_t longSize = longSet->pieces_.size();
    for (size_t i = 0; i < longSize; ++i)
        liIt = longIndices.insert_after(liIt, i);

    const auto& longPieces = longSet->pieces_;
    const auto& shortPieces = shortSet->pieces_;

    unsigned int posGlobal = 0;

    for (const auto& shortElem : shortPieces) {
        const auto& shortMin = shortElem.minElem();
        const auto& shortMax = shortElem.maxElem();
        const auto shortMinX = shortMin[0];
        const auto shortMaxX = shortMax[0];

        auto liPrev = longIndices.before_begin();
        auto liCurr = longIndices.begin();

        
        auto interInsertIt = inter.begin();
        std::advance(interInsertIt, posGlobal);

        
        while (interInsertIt != inter.end() && *interInsertIt < shortElem) {
            ++posGlobal;
            ++interInsertIt;
        }

        while (liCurr != longIndices.end()) {
            const size_t idx = *liCurr;
            const auto& longElem = longPieces[idx];

            const auto& longMin = longElem.minElem();
            const auto& longMax = longElem.maxElem();
            const auto longMinX = longMin[0];
            const auto longMaxX = longMax[0];

            if (longMaxX < shortMinX) {
                liCurr = longIndices.erase_after(liPrev);
                continue;
            }

            if (shortMaxX < longMinX)
                break;

            if (!(longMax.menorThan(shortMin)) &&
                !(shortMax.menorThan(longMin))) {
                const auto interRes = shortElem.intersection(longElem);
                if (!interRes.isEmpty()) {
                    auto insertIt = inter.begin();
                    std::advance(insertIt, posGlobal);
                    while (insertIt != inter.end() && *insertIt < interRes)
                        ++insertIt;
                    inter.insert(insertIt, interRes);
                }
            }

            ++liPrev;
            ++liCurr;
        }

        if (longIndices.empty())
            break;
    }

    return std::make_unique<OrderedSet>(inter);
}



SetDelegPtr OrderedSet::cup(const SetDelegate &other) const
{
    OrdSetCRef othr = static_cast<OrdSetCRef>(other);

    if (isEmpty()) 
        return std::make_unique<OrderedSet>(othr.pieces_);

    if (other.isEmpty() || pieces_ == othr.pieces_)
        return std::make_unique<OrderedSet>(pieces_);

    const auto this_max = maxElem();
    const auto this_min = minElem();
    const auto other_max = othr.maxElem();
    const auto other_min = othr.minElem();

    if (this_max < other_min) {
        MDIOrdSet result;
        result.reserve(pieces_.size() + othr.pieces_.size());
        result.insert(result.end(), pieces_.begin(), pieces_.end());
        result.insert(result.end(), othr.pieces_.begin(), othr.pieces_.end());
        return std::make_unique<OrderedSet>(result);
    }

    if (other_max < this_min) {
        MDIOrdSet result;
        result.reserve(othr.pieces_.size() + pieces_.size());
        result.insert(result.end(), othr.pieces_.begin(), othr.pieces_.end());
        result.insert(result.end(), pieces_.begin(), pieces_.end());
        return std::make_unique<OrderedSet>(result);
    }

    
    SetDelegPtr diff = difference(other);
    OrdSetCRef diff_cast = static_cast<OrdSetCRef>(*diff);

    return othr.disjointCup(diff_cast);
}


SetDelegPtr OrderedSet::complementAtom() const
{
  MDIOrdSet res;
  
  const SetPiece& mdi = *pieces_.begin();
  SetPiece dense_mdi;
  for (const Interval &i : mdi)
    dense_mdi.emplaceBack(Interval(i.begin(), 1, i.end()));
  SetPiece during_mdi = dense_mdi;

  Interval univ(0, 1, Inf);
  SetPiece all(mdi.arity(), univ);
  unsigned int dim = 0;
  int posGlobal = 0;
  auto it = res.end();
  for (const Interval &i : mdi) {
    int posLocal = posGlobal;

    if (i.begin() != 0) {
      Interval i_res(0, 1, i.begin() - 1);
      if (!i_res.isEmpty()) {
        all[dim] = i_res;
        
        it = res.end() - posLocal;
        
        res.insert(it,all);
        ++posLocal;
        ++posGlobal;
        all[dim] = univ;
      }
    }

    if (i.begin() < Inf && i.step() > 1) {
      for (unsigned int j = 0; j < i.step() - 1; ++j) {
        Interval i_res(i.begin() + j + 1, i.step(), i.end());
        if (!i_res.isEmpty()) {
          during_mdi[dim] = i_res;
          
          it = res.end() - posLocal;
          res.insert(it, during_mdi);
          ++posLocal;
          
        }
      }
    }

    if (i.end() < Inf) {
      Interval i_res(i.end() + 1, 1, Inf);
      if (!i_res.isEmpty()) {
        all[dim] = i_res;
        
        it = res.end() - posLocal;
        res.insert(it, all);
        ++posLocal;
        
        
        all[dim] = univ;
      }
    }

    all[dim] = dense_mdi[dim];
    during_mdi[dim] = i;
    ++dim;
  }
  std::reverse(res.begin(),res.end());
  return std::make_unique<OrderedSet>(res);
}

SetDelegPtr OrderedSet::interForCompl(const SetDelegate &other, const SetPiece &mdi, unsigned int* last) const {
  OrdSetCRef othr = static_cast<OrdSetCRef>(other);
  MDIOrdSet inter;

  if (isEmpty() || other.isEmpty())
      return std::make_unique<OrderedSet>(inter);

  if (pieces_ == othr.pieces_)
      return std::make_unique<OrderedSet>(pieces_);

  auto itn = pieces_.begin();
  std::advance(itn, *last);

  for (auto it = pieces_.begin(); it != itn; ++it)
      inter.push_back(*it);

  while (itn != pieces_.end() && itn->maxElem()[0] < mdi.minElem()[0]) {
      inter.push_back(*itn);
      ++(*last);
      ++itn;
  }

  std::forward_list<size_t> indices;
  auto liIt = indices.before_begin();
  const size_t longSize = othr.pieces_.size();
  for (size_t i = 0; i < longSize; ++i)
      liIt = indices.insert_after(liIt, i);

  auto longBegin = othr.pieces_.begin();
  unsigned int posGlobal = *last;
  auto fin = pieces_.end();
  auto current = pieces_.begin() + *last;

  while (current != fin) {
      const SetPiece &element = *current;
      const auto elementMin = element.minElem();
      const auto elementMax = element.maxElem();
      const auto elementMinX = elementMin[0];
      const auto elementMaxX = elementMax[0];

      auto liPrev = indices.before_begin();
      auto liCurr = indices.begin();

      bool doInt = (!(elementMax.menorThan(mdi.minElem())) && !(mdi.maxElem().menorThan(elementMin)));
      
      auto interInsertIt = inter.begin();
      std::advance(interInsertIt, posGlobal);

        
      while (interInsertIt != inter.end() && *interInsertIt < element) {
          ++posGlobal;
          ++interInsertIt;
      }
 
      while (liCurr != indices.end()) {
          size_t idx = *liCurr;
          const SetPiece &othrElem = *(longBegin + idx);
          const auto othrElemMin = othrElem.minElem();
          const auto othrElemMax = othrElem.maxElem();
          const auto othrElemMinX = othrElemMin[0];
          const auto othrElemMaxX = othrElemMax[0];


          if (othrElemMaxX < elementMinX) {
              liCurr = indices.erase_after(liPrev);
              continue;
          }

          if (elementMaxX < othrElemMinX)
              break;

          if (doInt && !(othrElemMax.menorThan(elementMin)) && !(elementMax.menorThan(othrElemMin))) {
              auto interRes = element.intersection(othrElem);
              if (!interRes.isEmpty()) {
                  auto it = inter.begin();
                  std::advance(it, posGlobal);

                  while (it != inter.end() && *it < interRes) 
                      ++it;
                      
                  inter.insert(it, std::move(interRes));
              }
          }

          ++liPrev;
          ++liCurr;
      }
      
      if(!doInt){
          auto it = inter.begin();
          std::advance(it, posGlobal);
          while (it != inter.end() && *it < element) 
                  ++it;
          inter.insert(it,element);

      }
      if (indices.empty())
          break;

      ++current;
  }

  return std::make_unique<OrderedSet>(inter);
}





SetDelegPtr OrderedSet::complement() const
{
  OrderedSet res;
  
  auto first_it = pieces_.begin();
  SetPiece first = *first_it;
  res = static_cast<OrdSetCRef>(*(OrderedSet(first).complementAtom()));

  ++first_it;
  unsigned int last=0;
  MDIOrdSet second(first_it, pieces_.end());
  for (const SetPiece &mdi : second) {
    SetDelegPtr c = OrderedSet(mdi).complementAtom();
    res = static_cast<OrdSetCRef>(*(res.interForCompl(*c,mdi,&last)));

  }

  return std::make_unique<OrderedSet>(res);
}

SetDelegPtr OrderedSet::difference(const SetDelegate &other) const
{
  MDIOrdSet emptyDiff;
  OrdSetCRef othr = static_cast<OrdSetCRef>(other);

  if (isEmpty()) 
    return std::make_unique<OrderedSet>(emptyDiff);
    
  if (othr.isEmpty())
    return std::make_unique<OrderedSet>(pieces_);

  if (maxElem() < othr.minElem()) 
    return std::make_unique<OrderedSet>(pieces_);
  
  if (othr.maxElem() < minElem()) 
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

SetDelegPtr OrderedSet::disjointCup(const SetDelegate &other) const
{
  OrdSetCRef othr = static_cast<OrdSetCRef>(other);
  MDIOrdSet cup = traverse(othr.pieces_);
  
  return std::make_unique<OrderedSet>(cup);
}

SetDelegPtr OrderedSet::filterSet(bool (*f)(const SetPiece &mdi)) const
{
  MDIOrdSet res;

  for (const SetPiece &mdi : pieces_)
    if (f(mdi))
      res.push_back(mdi);

  return std::make_unique<OrderedSet>(res);
}

SetDelegPtr OrderedSet::offset(const MD_NAT &off) const
{
  MDIOrdSet res;

  for (const SetPiece &mdi : pieces_)
    res.push_back(mdi.offset(off));

  return std::make_unique<OrderedSet>(res);
}

SetDelegPtr OrderedSet::compact() const
{
  MDIOrdSet res;

  if (isEmpty())
    return std::make_unique<OrderedSet>(res);

  auto next_it = pieces_.begin();
  ++next_it;
  SetPiece compacted = *pieces_.begin();
  for (auto it = pieces_.begin(); next_it != pieces_.end(); ++it) {
    MaybeMDI ith = compacted.compact(*next_it);
    if (!ith) {
      res.push_back(compacted);
      compacted = *next_it;
    }
    else
      compacted = ith.value();

    ++next_it;
  }
  res.push_back(compacted);

  return std::make_unique<OrderedSet>(res);
}


MDIOrdSet OrderedSet::traverse(const MDIOrdSet &other) const 
{
  if (isEmpty())
    return other;

  if (other.empty())
    return pieces_;  

  MDIOrdSet res;
  res.reserve(pieces_.size() + other.size());
  
  if (pieces_.back().minElem() < other.front().minElem()) {
    res.insert(res.end(), pieces_.begin(), pieces_.end());
    res.insert(res.end(), other.begin(), other.end());
    return res;
  }

  if (other.back().minElem() < pieces_.front().minElem()) {
    res.insert(res.end(), other.begin(), other.end());
    res.insert(res.end(), pieces_.begin(), pieces_.end());
    return res;
  }

  auto it1 = pieces_.begin(), it2 = other.begin();
  auto end1 = pieces_.end(), end2 = other.end();

  while (it1 != end1 && it2 != end2) {
    if (it1->minElem() < it2->minElem()) {
      res.emplace_back(*it1);
      ++it1;
    } else {
      res.emplace_back(*it2); 
      ++it2;
    }
  }


  res.insert(res.end(), it1, end1);
  res.insert(res.end(), it2, end2);

  return res;
}


////////////////////////////////////////////////////////////////////////////////
// Set Implementation ----------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

Set::Set(SetDelegPtr deleg) : delegate_(std::move(deleg)) {}
Set::Set(const Set &other)
  : delegate_(other.delegate_ ? other.delegate_->clone() : nullptr) {}

Set::Iterator::Iterator(std::shared_ptr<SetDelegate::Iterator> it)
  : it_(std::move(it)) {}

void Set::Iterator::operator++()
{
  ++(*it_);
  return;
}

SetPiece Set::Iterator::operator*() const { return **it_; }

bool Set::Iterator::operator!=(const Iterator &other) const
{
  return *it_ != *other.it_;
}

Set::Iterator Set::begin() const { return delegate_->begin(); }
Set::Iterator Set::end() const { return delegate_->end(); }

std::size_t Set::size() const { return delegate_->size(); }

void Set::emplace(SetPiece mdi)
{
  delegate_->emplace(mdi);
  return;
}
void Set::emplaceBack(SetPiece mdi)
{
  delegate_->emplaceBack(mdi);
  return;
}

bool Set::operator==(const Set &other) const
{
  return *delegate_ == *other.delegate_;
}

bool Set::operator!=(const Set &other) const { return !(*this == other); }

Set &Set::operator=(const Set &other)
{
  if (this != &other)
    delegate_ = other.delegate_->clone();

  return *this;
}

Set &Set::operator=(Set &&other)
{
  if (this != &other)
    delegate_ = std::move(other.delegate_);

  return *this;
}

std::ostream &Set::print(std::ostream &out) const
{
  delegate_->print(out);
  return out;
}

std::ostream &operator<<(std::ostream &out, const Set &s)
{
  s.print(out);
  return out;
}

unsigned int Set::cardinal() const { return delegate_->cardinal(); }

bool Set::isEmpty() const { return delegate_->isEmpty(); }

MD_NAT Set::minElem() const { return delegate_->minElem(); }

MD_NAT Set::maxElem() const { return delegate_->maxElem(); }

Set Set::intersection(const Set &other) const
{
  return Set(delegate_->intersection(*other.delegate_));
}

Set Set::cup(const Set &other) const
{
  return Set(delegate_->cup(*other.delegate_));
}

Set Set::complement() const { return Set(delegate_->complement()); }

Set Set::difference(const Set &other) const
{
  return Set(delegate_->difference(*other.delegate_));
}

std::size_t Set::arity() const  { return delegate_->arity(); }

Set Set::disjointCup(const Set &other) const
{
  return Set(delegate_->disjointCup(*other.delegate_));
}

Set Set::filterSet(bool (*f)(const SetPiece &mdi)) const
{
  return delegate_->filterSet(f);
}

Set Set::offset(const MD_NAT &off) const
{
  return delegate_->offset(off);
}

Set Set::compact() const { return delegate_->compact(); }

} // namespace LIB

} // namespace SBG
