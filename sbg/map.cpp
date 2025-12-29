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

  if (le.isId()) {
    return i;
  }

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

member_imp(Map, Set, dom);
member_imp(Map, Exp, exp);

Map::~Map() {}
Map::Map() : dom_(SET_FACT.createSet()) {}
Map::Map(MD_NAT x, Exp exp)
  : dom_(SET_FACT.createSet(x)), exp_(exp) {}
Map::Map(Interval i, LExp le)
  : dom_(SET_FACT.createSet(i)), exp_(Exp(le)) {}
Map::Map(SetPiece mdi, Exp exp)
  : dom_(SET_FACT.createSet(mdi)), exp_(exp) {}
Map::Map(Set s, Exp exp)
  : dom_(std::move(s)), exp_(exp) {}

bool Map::operator==(const Map& other) const
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

bool Map::operator!=(const Map& other) const
{
  return !(*this == other);
}

Map& Map::operator=(const Map& other)
{
  dom_ = other.dom_;
  exp_ = other.exp_;

  return *this;
}

Map Map::operator+(const Map& other) const
{
  Set res_dom = dom_.intersection(other.dom());
  Exp res_exp = exp_ + other.exp();

  return Map(res_dom, res_exp);
}

std::ostream& operator<<(std::ostream& out, const Map& m)
{
  out << m.dom() << " -> " << m.exp();

  return out;
}

// Map operations --------------------------------------------------------------

std::size_t Map::arity() const { return exp_.arity(); }

bool Map::isEmpty() const { return dom_.isEmpty(); }

Map Map::restrict(const Set& subdom) const
{
  return Map(dom_.intersection(subdom), exp_);
}

Set Map::image() const { return image(dom_); }

Set Map::image(const Set& subdom) const
{
  Set res = SET_FACT.createSet();

  if (subdom.isEmpty())
    return res;

  Set capdom = dom_.intersection(subdom);
  if (capdom.isEmpty()) {
    return res;
  } else {
    // Check if all expressions are bijective; if so all partial images can be
    // added without further checks
    bool cond = true;
    for (const LExp& le : exp_) {
      if (le.isConstant()) {
        cond = false;
      }
    }

    if (cond) {
      for (const SetPiece& mdi : capdom) {
        res.emplaceBack(SBG::LIB::image(mdi, exp_));
      }
    }
    else {
      for (const SetPiece& mdi : capdom) {
        Set ith_img = SET_FACT.createSet(SBG::LIB::image(mdi, exp_));
        res = std::move(res).cup(std::move(ith_img));
      }
    }
  }

  return res;
}

Set Map::preImage(const Set& subcodom) const
{
  Set im = image();
  Set cap_subcodom = im.intersection(subcodom);
  if (cap_subcodom.isEmpty()) 
    return SET_FACT.createSet();

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
  Map inv(cap_subcodom, inv_exp);
  Set inv_im = inv.image();

  return dom_.intersection(inv_im);
}

Map Map::composition(const Map& other) const
{
  Set res_dom = dom_.intersection(other.image());
  if (!res_dom.isEmpty()) {
    res_dom = other.preImage(res_dom);
  }
  Exp res_exp = exp_.composition(other.exp_);

  return Map(res_dom, res_exp);
}

Set Map::fixedPoints() const
{
  Interval univ_one_dim(0, 1, Inf);
  SetPiece allowed;
  for (unsigned int j = 0; j < arity(); ++j) {
    LExp jth = exp_[j];

    if (jth.isId())
      allowed.emplaceBack(univ_one_dim);

    else if (jth.isConstant())
      allowed.emplaceBack(Interval(jth.offset().toNat()));
 
    else
      return SET_FACT.createSet();
  }

  return dom_.intersection(SET_FACT.createSet(allowed));
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

  return Map(res_dom, res_exp);
}

bool Map::isId() const
{
  if (dom_.cardinal() == 1)
    return dom_ == image();

  return exp_.isId();
}

Set Map::lessImage(const Map& other) const
{
  Set result = SET_FACT.createSet();

  int ar = arity();
  SetPiece min_in_m1(ar, Interval(0, 1, Inf));
  Exp exp1 = exp_;
  Exp exp2 = other.exp_;

  Set cap_dom = dom_.intersection(other.dom_);
  if (cap_dom.isEmpty())
    return cap_dom; 

  for (int k = 0; k < ar; ++k) {
    LExp linear_exp1 = exp1[k];
    LExp linear_exp2 = exp2[k];

    RATIONAL m1 = linear_exp1.slope();
    RATIONAL m2 = linear_exp2.slope();
    if (m1 == m2) {
      RATIONAL h1 = linear_exp1.offset();
      RATIONAL h2 = linear_exp2.offset();
      if (h1 < h2) {
        result.emplaceBack(min_in_m1);
      }
      break;
    }
    else {
      RATIONAL point = linear_exp1.intersectionPoint(linear_exp2);
      if (point >= 0) {
        NAT floor = point.floor();
        NAT ceil = point.ceiling();
        if (ceil == floor) {
          NAT floor_minus = floor == 0 ? 0 : floor - 1;
          NAT ceil_plus = ceil == Inf ? Inf : ceil + 1;
          Interval kth = m1 < m2 ? Interval(ceil_plus, 1, Inf)
            : Interval (0, 1, floor_minus);
          min_in_m1[k] = kth;
          result.emplaceBack(min_in_m1);
          min_in_m1[k] = Interval(ceil, 1, floor);
        }
        else {
          break;
        }
      }
      else {
        if (m1 < m2) {
          result.emplaceBack(min_in_m1);
        }
        break;
      }
    }
  }

  result = result.intersection(cap_dom);
  return result;
}

MaybeMap Map::compact(const Map& other) const
{
  Set res_dom = SET_FACT.createSet();
  if (exp_ == other.exp())
    return Map(dom_.cup(other.dom()).compact(), exp_);

  return {};
}

} // namespace LIB

} // namespace SBG
