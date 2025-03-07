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

#include "sbg/map.hpp"

namespace SBG {

namespace LIB {

// Auxiliary functions ---------------------------------------------------------

void compatible(Interval i, LExp le)
{
  Util::RATIONAL m = le.slope(), h = le.offset(), rat_inf(Util::INT_Inf, 1);
  if (m == rat_inf && h == -rat_inf)
    return;
  if (h == rat_inf || m > rat_inf)
    return;

  if (i.begin() == i.end()) {
    ((Util::RATIONAL) i.begin() * le.slope() + le.offset()).toNat();
    return;
  }

  Util::RATIONAL st_rat(i.step()), min_rat(i.begin());
  Util::RATIONAL im_step = st_rat * m
                 , im_begin = min_rat * m + h;
  if (im_step.denominator() != 1 || im_begin.denominator() != 1) 
    if (im_step.numerator() != 0 || im_begin.numerator() != 0) {
      Util::ERROR("compatible: incompatible ", i, " with ", le, "\n");
      return;
    }

  return;
}

void compatible(SetPiece mdi, Exp mdle)
{
  Util::ERROR_UNLESS(mdi.arity() == mdle.arity()
    , "compatible: dimensions of ", mdi, " and ", mdle, " don't match\n");

  for (unsigned int j = 0; j < mdi.arity(); ++j)
    compatible(mdi[j], mdle[j]);

  return;
}

Interval image(Interval i, LExp le) {
  Util::RATIONAL m = le.slope(), h = le.offset();
  Util::NAT new_begin = 0, new_step = 0, new_end = 0;

  Util::RATIONAL rat_inf(Util::INT_Inf, 1);
  if (m == rat_inf || m > rat_inf)
    return Interval(0, 1, Util::Inf);

  if (le.isId())
    return i;

  if (le.isConstant()) {
    Util::NAT off = le.offset().toNat();
    return Interval(off, 1, off);
  }

  if (i.begin() == i.end()) {
    Util::NAT x = (m * i.begin() + h).toNat();
    return Interval(x, 1, x);
  }

  // Increasing expression
  if (m > 0) {
    new_begin = (m * i.begin() + h).toNat();
    new_step = (m * i.step()).toNat();
    new_end = (m * i.end() + h).toNat();
  }

  // Decreasing expression
  else if (m < 0) {
    new_begin = (m * i.end() + h).toNat();
    new_step = (-m * i.step()).toNat();
    new_end = (m * i.begin() + h).toNat();
  }

  return Interval(new_begin, new_step, new_end);
}

SetPiece image(SetPiece mdi, Exp mdle)
{
  if (mdi.isEmpty())
    return mdi;

  SetPiece res;
  for (unsigned int j = 0; j < mdi.arity(); ++j)
    res.emplaceBack(image(mdi[j], mdle[j]));

  return res;
}

////////////////////////////////////////////////////////////////////////////////
// Map Delegate constructors ---------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

MapDelegate::MapDelegate(const SetAF &fact)
  : fact_(fact), dom_(fact.createSet()), exp_() {}
MapDelegate::MapDelegate(const SetAF &fact, Util::MD_NAT x, Exp exp)
  : fact_(fact), dom_(fact.createSet(x)), exp_(exp) {}
MapDelegate::MapDelegate(const SetAF &fact, Interval i, LExp le)
  : fact_(fact), dom_(fact.createSet(i)), exp_(Exp(le)) {}
MapDelegate::MapDelegate(const SetAF &fact, SetPiece mdi, Exp exp)
  : fact_(fact), dom_(fact.createSet(mdi)), exp_(exp) {}
MapDelegate::MapDelegate(const SetAF &fact, Set s, Exp exp)
  : fact_(fact), dom_(std::move(s)), exp_(exp) {}

////////////////////////////////////////////////////////////////////////////////
// Map Set Implementation ------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

MapSetDeleg::~MapSetDeleg() {}
MapSetDeleg::MapSetDeleg(const SetAF &fact) : MapDelegate(fact) {}
MapSetDeleg::MapSetDeleg(const SetAF &fact, Util::MD_NAT x, Exp exp)
  : MapDelegate(fact, x, exp) {}
MapSetDeleg::MapSetDeleg(const SetAF &fact, Interval i, LExp le)
  : MapDelegate(fact, i, le) {}
MapSetDeleg::MapSetDeleg(const SetAF &fact, SetPiece mdi, Exp exp)
  : MapDelegate(fact, mdi, exp) {}
MapSetDeleg::MapSetDeleg(const SetAF &fact, Set s, Exp exp)
  : MapDelegate(fact, std::move(s), exp) {}

bool MapSetDeleg::operator==(const MapDelegate &other) const
{
  MapSetDelegCRef othr = static_cast<MapSetDelegCRef>(other);

  if (dom_ == othr.dom()) {
    if (dom_.cardinal() == 1) {
      return image() == othr.image();
    }
    else 
      return exp_ == othr.exp();
  }

  return false;
}

bool MapSetDeleg::operator!=(const MapDelegate &other) const
{
  return !(*this == other);
}

MapDelegPtr MapSetDeleg::operator+(const MapDelegate &other)
{
  MapSetDelegCRef othr = static_cast<MapSetDelegCRef>(other);
  Set res_dom = dom_.intersection(othr.dom());
  Exp res_exp = exp_ + othr.exp();

  return std::make_unique<MapSetDeleg>(fact_, res_dom, res_exp);
}

std::ostream &MapSetDeleg::print(std::ostream &out) const
{
  out << dom_ << " -> " << exp_;

  return out;
}

// Map operations --------------------------------------------------------------

Set MapSetDeleg::dom() const { return std::move(dom_); }

Exp MapSetDeleg::exp() const { return exp_; }

std::size_t MapSetDeleg::arity() const
{
  if (dom_.isEmpty())
    return 0;

  return dom_.arity();
}

bool MapSetDeleg::isEmpty() const { return dom_.isEmpty(); }

MapDelegPtr MapSetDeleg::restrict(const Set &subdom)
{
  return std::make_unique<MapSetDeleg>(fact_, dom_.intersection(subdom), exp_);
}

Set MapSetDeleg::image() const { return image(dom_); }

Set MapSetDeleg::image(const Set &subdom) const
{
  Set res = fact_.createSet();

  if (subdom.isEmpty())
    return res;

  Set capdom = dom_.intersection(subdom);
  if (capdom.isEmpty())
    return res;
  else {
    // Check if all expressions are bijective; in the affirmative case all
    // images can be added without further checks
    bool cond = true;
    for (const LExp &le : exp_)
      if (le.isConstant())
        cond = false;

    if (cond) {
      for (const SetPiece &mdi : capdom)
        res.emplace(SBG::LIB::image(mdi, exp_));
    }
    else {
      // TODO
      for (const SetPiece &mdi : capdom) {
        res.cup(fact_.createSet(SBG::LIB::image(mdi, exp_)));
      }
    }
  }

  return res;
}

Set MapSetDeleg::preImage() const { return std::move(dom_); }

Set MapSetDeleg::preImage(const Set &subcodom) const
{
  Set im = image();
  Set cap_subcodom = im.intersection(subcodom);
  if (cap_subcodom.isEmpty()) 
    return fact_.createSet();

  Util::RATIONAL rat_inf(Util::INT_Inf, 1);
  Set inv_dom = cap_subcodom;
  Exp inv_exp = exp_.inverse();
  for (unsigned int j = 0; j < inv_exp.arity(); ++j) {
    Util::RATIONAL m = exp_[j].slope(), h = exp_[j].offset();
    if (m == rat_inf && h == -rat_inf) {
      for (SetPiece mdi : cap_subcodom)
        mdi[j] = Interval(0, 1, Util::Inf);
      inv_exp[j] = LExp(1, 0);
    }
  }
  MapSetDeleg inv(fact_, cap_subcodom, inv_exp);
  Set inv_im = inv.image();

  return dom_.intersection(inv_im);
}

MapDelegPtr MapSetDeleg::composition(const MapDelegate &other)
{
  if (exp_.isId())
    return std::make_unique<MapSetDeleg>(fact_, std::move(dom_), exp_);

  Set res_dom = dom_.intersection(other.image());
  res_dom = other.preImage(res_dom);
  MapSetDelegCRef othr = static_cast<MapSetDelegCRef>(other);
  Exp res_exp = exp_.composition(othr.exp_);

  return std::make_unique<MapSetDeleg>(fact_, res_dom, res_exp);
}

// Extra operations ------------------------------------------------------------

MapDelegPtr MapSetDeleg::minInv()
{
  Set res_dom = image();
  Exp res_exp;

  if (dom_.cardinal() == 1 || exp_.isConstant()) 
    res_exp = Exp(dom_.minElem());
  else
    res_exp = exp_.inverse();

  return std::make_unique<MapSetDeleg>(fact_, res_dom, res_exp);
}

bool MapSetDeleg::isId() const
{
  if (dom_.cardinal() == 1)
    return dom_ == image();

  return exp_.isId();
}

MapDelegPtr MapSetDeleg::compact(const MapDelegate &other)
{
  MapSetDelegCRef othr = static_cast<MapSetDelegCRef>(other);

  Set res_dom = fact_.createSet();
  if (exp_ == othr.exp())
    res_dom = dom_.cup(othr.dom());

  return std::make_unique<MapSetDeleg>(fact_, res_dom, exp_);
}

////////////////////////////////////////////////////////////////////////////////
// Map Implementation ----------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

Map::Map(MapDelegPtr deleg) : delegate_(std::move(deleg)) {}

bool Map::operator==(const Map &other) const
{
  return *delegate_ == *other.delegate_;
}

bool Map::operator!=(const Map &other) const
{
  return !(*this == other);
}

Map Map::operator+(const Map &other) const
{
  return Map(delegate_->operator+(*other.delegate_));
}

std::ostream &Map::print(std::ostream &out) const
{
  return delegate_->print(out);
}

std::ostream &operator<<(std::ostream &out, const Map &m)
{
  m.print(out);
  return out;
}

Set Map::dom() const { return delegate_->dom(); }

Exp Map::exp() const { return delegate_->exp(); }

std::size_t Map::arity() const { return delegate_->arity(); }

bool Map::isEmpty() const { return delegate_->isEmpty(); }

Map Map::restrict(const Set &subdom) const
{
  return Map(delegate_->restrict(subdom));
}

Set Map::image() const { return delegate_->image(); }

Set Map::image(const Set &subdom) const { return delegate_->image(subdom); }

Set Map::preImage() const { return delegate_->preImage(); }

Set Map::preImage(const Set &subdom) const
{
  return delegate_->preImage(subdom);
}

Map Map::composition(const Map &other) const
{
  return Map(delegate_->composition(*other.delegate_));
}

Map Map::minInv() const
{
  return Map(delegate_->minInv());
}

bool Map::isId() const { return delegate_->isId(); }

} // namespace LIB

} // namespace SBG
