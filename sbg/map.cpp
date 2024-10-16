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

// -----------------------------------------------------------------------------

template<typename Set>
SBGMap<Set>::SBGMap() : dom_(Set()), exp_(Exp()) {}
template<typename Set>
SBGMap<Set>::SBGMap(Util::MD_NAT x, Exp exp) : dom_(), exp_() {
  SetPiece mdi(x);
  dom_ = Set(mdi);
  exp_ = exp;
}
template<typename Set>
SBGMap<Set>::SBGMap(Interval i, LExp le) : dom_(), exp_() {
  dom_ = Set(SetPiece(i));
  exp_ = Exp(le);
}
template<typename Set>
SBGMap<Set>::SBGMap(SetPiece mdi, Exp exp) : dom_(), exp_() {
  dom_ = Set(mdi);
  exp_ = exp;
}
template<typename Set>
SBGMap<Set>::SBGMap(Set dom, Exp exp) : dom_(), exp_() {
  if (!dom.isEmpty()) {
    dom_ = dom;
    exp_ = exp;
  }
}

member_imp_temp(template<typename Set>, SBGMap<Set>, Set, dom);
member_imp_temp(template<typename Set>, SBGMap<Set>, Exp, exp);

template<typename Set>
bool SBGMap<Set>::operator==(const SBGMap &other) const
{
  if (dom_ == other.dom_) {
    if (dom_.cardinal() == 1) {
      return this->image() == other.image();
    }
    else 
      return exp_ == other.exp_;
  }

  return false;
}

template<typename Set>
bool SBGMap<Set>::operator!=(const SBGMap &other) const
{
  return !(*this == other);
}

template<typename Set>
SBGMap<Set> SBGMap<Set>::operator+(const SBGMap &other) const
{
  Set res_dom = dom_.intersection(other.dom_);
  Exp res_exp = exp_ + other.exp_;

  return SBGMap(res_dom, res_exp);
}

template<typename Set>
std::ostream &operator<<(std::ostream &out, const SBGMap<Set> &sbgmap)
{
  out << sbgmap.dom() << " -> " << sbgmap.exp();

  return out;
}

// SBGMap functions ------------------------------------------------------------

template<typename Set>
std::size_t SBGMap<Set>::arity() const
{
  if (dom_.isEmpty())
    return 0;

  return dom_.begin()->arity();
}

template<typename Set>
bool SBGMap<Set>::isEmpty() const { return dom_.isEmpty(); }

template<typename Set>
SBGMap<Set> SBGMap<Set>::restrict(const Set &subdom) const
{
  Set restricted_dom = dom_.intersection(subdom);
  return SBGMap(restricted_dom, exp_);
}

template<typename Set>
Set SBGMap<Set>::image() const { return image(dom_); }

template<typename Set>
Set SBGMap<Set>::image(const Set &subdom) const
{
  Set res;

  if (subdom.isEmpty())
    return Set();

  Set capdom = dom_.intersection(subdom);
  if (capdom.isEmpty())
    return Set();
  else {
    for (const SetPiece &atom : capdom)
      res.emplace(LIB::image(atom, exp_));
  }

  return res;
}

template<typename Set>
Set SBGMap<Set>::preImage() const { return dom_; }

template<typename Set>
Set SBGMap<Set>::preImage(const Set &subcodom) const
{
  Set im = image();
  Set cap_subcodom = im.intersection(subcodom);
  if (cap_subcodom.isEmpty()) 
    return Set();

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
  SBGMap inv(cap_subcodom, inv_exp);
  Set inv_im = inv.image();

  return dom_.intersection(inv_im);
}

template<typename Set>
SBGMap<Set> SBGMap<Set>::composition(const SBGMap &other) const
{
  Set res_dom = dom_.intersection(other.image());
  res_dom = other.preImage(res_dom);
  Exp res_exp = exp_.composition(other.exp_);

  return SBGMap<Set>(res_dom, res_exp);
}

// Extra functions -------------------------------------------------------------

template<typename Set>
SBGMap<Set> SBGMap<Set>::minInv() const
{
  Set res_dom = image();

  if (dom_.cardinal() == 1 || exp_.isConstant())
    return SBGMap(res_dom, Exp(dom_.minElem()));

  return SBGMap(res_dom, exp_.inverse());
}

template<typename Set>
bool SBGMap<Set>::isId() const
{
  if (dom_.cardinal() == 1)
    return dom_ == image();

  return exp_.isId();
}

template<typename Set>
std::optional<SBGMap<Set>> SBGMap<Set>::compact(const SBGMap<Set> &other) const
{
  if (exp_ == other.exp_)
    return SBGMap<Set>(dom_.cup(other.dom_).compact(), exp_);

  return {};
}

// Templates -------------------------------------------------------------------

template struct SBGMap<UnordSet>;
template std::ostream &operator<<(std::ostream &out, const BaseMap &sbgmap);

template struct SBGMap<OrdSet>;
template std::ostream &operator<<(std::ostream &out, const CanonMap &sbgmap);

} // namespace LIB

} // namespace SBG
