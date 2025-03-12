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

Interval image(Interval i, LExp le) {
  RATIONAL m = le.slope(), h = le.offset();
  NAT new_begin = 0, new_step = 0, new_end = 0;

  RATIONAL rat_inf(INT_Inf, 1);
  if (m == rat_inf || m > rat_inf)
    return Interval(0, 1, Inf);

  if (le.isId())
    return i;

  if (le.isConstant()) {
    NAT off = le.offset().toNat();
    return Interval(off, 1, off);
  }

  if (i.begin() == i.end()) {
    NAT x = (m * i.begin() + h).toNat();
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
// Map Implementation ----------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

member_move_imp(Map, Set, dom);
member_move_imp(Map, Exp, exp);

Map::~Map() {}
Map::Map(const SetAF &fact) : fact_(fact), dom_(fact.createSet()) {}
Map::Map(const SetAF &fact, MD_NAT x, Exp exp)
  : fact_(fact), dom_(fact.createSet(x)), exp_(exp) {}
Map::Map(const SetAF &fact, Interval i, LExp le)
  : fact_(fact), dom_(fact.createSet(i)), exp_(Exp(le)) {}
Map::Map(const SetAF &fact, SetPiece mdi, Exp exp)
  : fact_(fact), dom_(fact.createSet(mdi)), exp_(exp) {}
Map::Map(const SetAF &fact, Set s, Exp exp)
  : fact_(fact), dom_(std::move(s)), exp_(exp) {}

bool Map::operator==(const Map &other) const
{
  if (dom_ == other.dom()) {
    if (dom_.cardinal() == 1) {
      return image() == other.image();
    }
    else 
      return exp_ == other.exp();
  }

  return false;
}

bool Map::operator!=(const Map &other) const
{
  return !(*this == other);
}

Map Map::operator+(const Map &other) const
{
  Set res_dom = dom_.intersection(other.dom());
  Exp res_exp = exp_ + other.exp();

  return Map(fact_, res_dom, res_exp);
}

std::ostream &operator<<(std::ostream &out, const Map &m)
{
  out << m.dom() << " -> " << m.exp();

  return out;
}

// Map operations --------------------------------------------------------------

std::size_t Map::arity() const
{
  if (dom_.isEmpty())
    return 0;

  return dom_.arity();
}

bool Map::isEmpty() const { return dom_.isEmpty(); }

Map Map::restrict(const Set &subdom) const
{
  return Map(fact_, dom_.intersection(subdom), exp_);
}

Set Map::image() const { return image(dom_); }

Set Map::image(const Set &subdom) const
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
      // TODO: optimization?
      for (const SetPiece &mdi : capdom) {
        res = res.cup(fact_.createSet(SBG::LIB::image(mdi, exp_)));
      }
    }
  }

  return res;
}

Set Map::preImage(const Set &subcodom) const
{
  Set im = image();
  Set cap_subcodom = im.intersection(subcodom);
  if (cap_subcodom.isEmpty()) 
    return fact_.createSet();

  RATIONAL rat_inf(INT_Inf, 1);
  Set inv_dom = cap_subcodom;
  Exp inv_exp = exp_.inverse();
  for (unsigned int j = 0; j < inv_exp.arity(); ++j) {
    RATIONAL m = exp_[j].slope(), h = exp_[j].offset();
    if (m == rat_inf && h == -rat_inf) {
      for (SetPiece mdi : cap_subcodom)
        mdi[j] = Interval(0, 1, Inf);
      inv_exp[j] = LExp(1, 0);
    }
  }
  Map inv(fact_, cap_subcodom, inv_exp);
  Set inv_im = inv.image();

  return dom_.intersection(inv_im);
}

Map Map::composition(const Map &other) const
{
  if (exp_.isId())
    return Map(fact_, std::move(dom_), exp_);

  Set res_dom = dom_.intersection(other.image());
  res_dom = other.preImage(res_dom);
  Exp res_exp = exp_.composition(other.exp_);

  return Map(fact_, res_dom, res_exp);
}

// Extra operations ------------------------------------------------------------

Map Map::minInv() const
{
  Set res_dom = image();
  Exp res_exp;

  if (dom_.cardinal() == 1 || exp_.isConstant()) 
    res_exp = Exp(dom_.minElem());
  else
    res_exp = exp_.inverse();

  return Map(fact_, res_dom, res_exp);
}

bool Map::isId() const
{
  if (dom_.cardinal() == 1)
    return dom_ == image();

  return exp_.isId();
}

MaybeMap Map::compact(const Map &other) const
{
  Set res_dom = fact_.createSet();
  if (exp_ == other.exp())
    return Map(fact_, dom_.cup(other.dom()), exp_);

  return {};
}

} // namespace LIB

} // namespace SBG
