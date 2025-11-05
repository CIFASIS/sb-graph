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

#include "sbg/pw_map.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// PWMap Abstract Strategy Constructors ----------------------------------------
////////////////////////////////////////////////////////////////////////////////

PWMapStrategy::PWMapStrategy() {}

////////////////////////////////////////////////////////////////////////////////
// PWMap Interface -------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

PWMap::PWMap(PWMapStratPtr strat) : strategy_(std::move(strat)) {}

PWMap::PWMap(const PWMap &other)
  : strategy_(other.strategy_ ? other.strategy_->clone() : nullptr) {}

PWMap::Iterator::Iterator(std::shared_ptr<PWMapStrategy::Iterator> it)
  : it_(std::move(it)) {}

void PWMap::Iterator::operator++()
{
  ++(*it_);
  return;
}

Map PWMap::Iterator::operator*() const { return **it_; }

bool PWMap::Iterator::operator!=(const Iterator &other) const
{
  return *it_ != *other.it_;
}

PWMap::Iterator PWMap::begin() const { return strategy_->begin(); }
PWMap::Iterator PWMap::end() const { return strategy_->end(); }

void PWMap::emplaceBack(const Map &m)
{
  strategy_->emplaceBack(m);
  return;
}

bool PWMap::operator==(const PWMap &other) const
{
  return *strategy_ == *other.strategy_;
}

bool PWMap::operator!=(const PWMap &other) const { return !(*this == other); }

PWMap &PWMap::operator=(const PWMap &other)
{
  if (this != &other)
    strategy_ = other.strategy_->clone();

  return *this;
}

PWMap &PWMap::operator=(PWMap &&other)
{
  if (this != &other)
    strategy_ = std::move(other.strategy_);

  return *this;
}

std::ostream &PWMap::print(std::ostream &out) const
{
  strategy_->print(out);
  return out;
}

std::ostream &operator<<(std::ostream &out, const PWMap &pw)
{
  pw.print(out);
  return out;
}

PWMap PWMap::operator+(const PWMap &other) const
{
  return strategy_->operator+(*other.strategy_);
}

PWMap PWMap::operator-(const PWMap &other) const
{
  return strategy_->operator-(*other.strategy_);
}

std::size_t PWMap::arity() const { return strategy_->arity(); }

bool PWMap::isEmpty() const { return strategy_->isEmpty(); }

Set PWMap::dom() const { return strategy_->dom(); }

PWMap PWMap::restrict(const Set &subdom) const
{
  return strategy_->restrict(subdom);
}

Set PWMap::image() const { return strategy_->image(); }

Set PWMap::image(const Set &subdom) const { return strategy_->image(subdom); }

Set PWMap::preImage(const Set &subcodom) const
{
  return strategy_->preImage(subcodom);
}

PWMap PWMap::inverse() const { return strategy_->inverse(); }

PWMap PWMap::composition(const PWMap &other) const
{
  return strategy_->composition(*other.strategy_);
}

PWMap PWMap::mapInf(unsigned int n) const { return strategy_->mapInf(n); }

PWMap PWMap::mapInf() const { return strategy_->mapInf(); }

Set PWMap::fixedPoints() const { return strategy_->fixedPoints(); }

PWMap PWMap::concatenation(const PWMap &other) const
{
  return strategy_->concatenation(*other.strategy_);
}

PWMap PWMap::combine(const PWMap &other) const
{
  return strategy_->combine(*other.strategy_);
}

PWMap PWMap::reduce() const { return strategy_->reduce(); }

PWMap PWMap::minMap(const PWMap &other) const
{
  return strategy_->minMap(*other.strategy_);
}

PWMap PWMap::minAdjMap(const PWMap &other) const
{
  return strategy_->minAdjMap(*other.strategy_);
}

PWMap PWMap::firstInv(const Set &subdom) const
{
  return strategy_->firstInv(subdom);
}

PWMap PWMap::firstInv() const { return strategy_->firstInv(); }

PWMap PWMap::filterMap(bool (*f)(const Map &)) const
{
  return strategy_->filterMap(f);
}

Set PWMap::equalImage(const PWMap &other) const
{
  return strategy_->equalImage(*other.strategy_);
}

Set PWMap::sharedImage() const { return strategy_->sharedImage(); }

PWMap PWMap::offsetDom(const MD_NAT &off) const
{
  return strategy_->offsetDom(off);
}

PWMap PWMap::offsetDom(const PWMap &off) const
{
  return strategy_->offsetDom(*off.strategy_);
}

PWMap PWMap::offsetImage(const MD_NAT &off) const
{
  return strategy_->offsetImage(off);
}

PWMap PWMap::offsetImage(const Exp &off) const
{
  return strategy_->offsetImage(off);
}

PWMap PWMap::compact() const { return strategy_->compact(); }

} // namespace LIB

} // namespace SBG;

