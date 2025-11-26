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

#include "sbg/set.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Set Abstract Strategy Constructors ------------------------------------------
////////////////////////////////////////////////////////////////////////////////

SetStrategy::SetStrategy() {}
SetStrategy::SetStrategy(const MD_NAT& x) {}
SetStrategy::SetStrategy(const Interval& i) {}
SetStrategy::SetStrategy(const SetPiece& mdi) {}

////////////////////////////////////////////////////////////////////////////////
// Set Interface ---------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

Set::Set(SetStratPtr strat) : strategy_(std::move(strat)) {}
Set::Set(const Set& other)
  : strategy_(other.strategy_ ? other.strategy_->clone() : nullptr) {}

Set::Iterator::Iterator(std::shared_ptr<SetStrategy::Iterator> it)
  : it_(std::move(it)) {}

void Set::Iterator::operator++()
{
  ++(*it_);
  return;
}

SetPiece Set::Iterator::operator*() const { return **it_; }

bool Set::Iterator::operator!=(const Iterator& other) const { return *it_ != *other.it_; }

bool Set::Iterator::operator==(const Iterator& other) const { return *it_ == *other.it_; }

bool Set::Iterator::operator<(const Iterator& other) const { return it_ < other.it_; }

Set::Iterator Set::begin() const { return strategy_->begin(); }
Set::Iterator Set::end() const { return strategy_->end(); }

std::size_t Set::size() const { return strategy_->size(); }

void Set::emplace(SetPiece mdi)
{
  strategy_->emplace(mdi);
  return;
}
void Set::emplaceBack(SetPiece mdi)
{
  strategy_->emplaceBack(mdi);
  return;
}

bool Set::operator==(const Set& other) const
{
  return *strategy_ == *other.strategy_;
}

bool Set::operator!=(const Set& other) const { return !(*this == other); }

Set& Set::operator=(const Set& other)
{
  if (this !=& other)
    strategy_ = other.strategy_->clone();

  return *this;
}

Set& Set::operator=(Set&& other)
{
  if (this !=& other)
    strategy_ = std::move(other.strategy_);

  return *this;
}

std::ostream& Set::print(std::ostream& out) const
{
  strategy_->print(out);
  return out;
}

std::ostream& operator<<(std::ostream& out, const Set& s)
{
  s.print(out);
  return out;
}

unsigned int Set::cardinal() const { return strategy_->cardinal(); }

bool Set::isEmpty() const { return strategy_->isEmpty(); }

MD_NAT Set::minElem() const { return strategy_->minElem(); }

MD_NAT Set::maxElem() const { return strategy_->maxElem(); }

Set Set::intersection(const Set& other) const
{
  return Set(strategy_->intersection(*other.strategy_));
}

Set Set::cup(const Set& other) const
{
  return Set(strategy_->cup(*other.strategy_));
}

Set Set::complement() const
{
  return Set(strategy_->complement());
}

Set Set::difference(const Set& other) const
{
  return Set(strategy_->difference(*other.strategy_));
}

std::size_t Set::arity() const  { return strategy_->arity(); }

Set Set::disjointCup(const Set& other) const
{
  return Set(strategy_->disjointCup(*other.strategy_));
}

Set Set::filterSet(bool (*f)(const SetPiece& mdi)) const
{
  return strategy_->filterSet(f);
}

Set Set::offset(const MD_NAT& off) const
{
  return strategy_->offset(off);
}

Set Set::compact() const { return strategy_->compact(); }

}  // namespace LIB

}  // namespace SBG
