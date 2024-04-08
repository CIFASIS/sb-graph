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
  Util::RATIONAL rat_inf(Util::INT_Inf, 1);
  if (le.slope() == rat_inf || le.slope() > rat_inf)
    return;

  Util::RATIONAL st_rat(i.step()), min_rat(i.minElem());
  Util::RATIONAL im_step = st_rat * le.slope()
                 , im_begin = min_rat * le.slope() + le.offset();
  if (im_step.denominator() != 1 || im_begin.denominator() != 1) 
    if (im_step.numerator() != 0 || im_begin.numerator() != 0) {
      Util::ERROR("LIB::SBGMap::compatible: incompatible i/le");
      return;
    }

  return;
}

void compatible(SetPiece mdi, Exp mdle)
{
  Util::ERROR_UNLESS(mdi.size() == mdle.size()
               , "LIB::SBGMap::compatible: dimensions don't match");

  for (unsigned int j = 0; j < mdi.size(); ++j)
    compatible(mdi[j], mdle[j]);

  return;
}

Interval image(Interval i, LExp le) {
  Util::RATIONAL m = le.slope(), h = le.offset();
  Util::NAT new_begin = 0, new_step = 0, new_end = 0;

  if (le.isId())
    return i;

  if (le.isConstant()) {
    Util::NAT off = le.offset().toNat();
    return Interval(off, 1, off);
  }

  Util::RATIONAL rat_inf(Util::INT_Inf, 1);
  if (m == rat_inf || m > rat_inf)
    return Interval(0, 1, Util::Inf);

  // Increasing expression
  if (m > 0) {
    new_begin = (m * i.minElem() + h).toNat();
    new_step = (m * i.step()).toNat();
    new_end = (m * i.maxElem() + h).toNat();
  }

    // Decreasing expression
  else if (m < 0) {
    new_begin = (m * i.minElem() + h).toNat();
    new_step = (m * i.step()).toNat();
    new_end = (m * i.maxElem() + h).toNat();
  }

  return Interval(new_begin, new_step, new_end);
}

SetPiece image(SetPiece mdi, Exp mdle)
{
  if (mdi.isEmpty())
    return mdi;

  Util::ERROR_UNLESS(mdi.size() == mdle.size()
      , "LIB::SBGMap::image: dimensions don't match");

  SetPiece res;
  for (unsigned int j = 0; j < mdi.size(); ++j)
    res.emplaceBack(image(mdi[j], mdle[j]));

  return res;
}

// -----------------------------------------------------------------------------

template<typename Set>
SBGMap<Set>::SBGMap() : dom_(Set()), exp_(Exp()) {}
template<typename Set>
SBGMap<Set>::SBGMap(Util::MD_NAT x, Exp exp) : dom_(), exp_() {
  SetPiece mdi(x);
  compatible(mdi, exp);
  dom_ = Set(mdi);
  exp_ = exp;
}
template<typename Set>
SBGMap<Set>::SBGMap(Interval i, LExp le) : dom_(), exp_() {
  compatible(i, le);
  dom_ = Set(SetPiece(i));
  exp_ = Exp(le);
}
template<typename Set>
SBGMap<Set>::SBGMap(SetPiece mdi, Exp exp) : dom_(), exp_() {
  compatible(mdi, exp);
  dom_ = Set(mdi);
  exp_ = exp;
}
template<typename Set>
SBGMap<Set>::SBGMap(Set dom, Exp exp) : dom_(), exp_() {
  if (!dom.isEmpty()) {
    for (const SetPiece &mdi : dom)
      compatible(mdi, exp);
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
std::ostream &operator<<(std::ostream &out, const SBGMap<Set> &sbgmap)
{
  out << sbgmap.dom() << " -> " << sbgmap.exp();

  return out;
}

// SBGMap functions ------------------------------------------------------------

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

  Util::MD_NAT min = cap_subcodom.minElem();
  Exp inv_exp = exp_.inverse();
  for (unsigned int j = 0; j < inv_exp.size(); ++j) {
    Util::RATIONAL m = exp_[j].slope(), h = exp_[j].offset();
    Util::RATIONAL rat_inf(Util::INT_Inf, 1);
    if (m == rat_inf && h == -rat_inf) {
      inv_exp[j] = LExp(0, min[j]);
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
SBGMap<Set> SBGMap<Set>::minInv(const Set &allowed) const
{
  Set res_dom = restrict(allowed).image();

  if (dom_.cardinal() == 1 || exp_.isConstant())
    return SBGMap<Set>(res_dom, Exp(dom_.minElem()));

  return SBGMap<Set>(res_dom, exp_.inverse());
}

template<typename Set>
SBGMap<Set> SBGMap<Set>::minInv() const { return minInv(dom_); }

template<typename Set>
bool SBGMap<Set>::isId() const
{
  if (dom_.cardinal() == 1)
    return dom_ == image();

  return exp_.isId();
}

// Function should be called on a non-empty sbgmap
template<typename Set>
unsigned int SBGMap<Set>::nmbrDims() const { return dom_.begin()->size(); }

template<typename Set>
std::optional<SBGMap<Set>> SBGMap<Set>::compact(const SBGMap<Set> &other) const
{
  if (exp_ == other.exp_)
    return SBGMap<Set>(dom_.cup(other.dom_).compact(), exp_);

  return {};
}

template<typename Set>
std::size_t hash_value(const SBGMap<Set> &sbgmap)
{
  std::size_t seed = 0;
  boost::hash_combine(seed, sbgmap.dom());
  boost::hash_combine(seed, sbgmap.exp());

  return seed;
}

// Templates -------------------------------------------------------------------

template struct SBGMap<UnordSet>;
template std::ostream &operator<<(std::ostream &out, const BaseMap &sbgmap);
template std::size_t hash_value<UnordSet>(const BaseMap &sbgmap);

template struct SBGMap<OrdSet>;
template std::ostream &operator<<(std::ostream &out, const CanonMap &sbgmap);
template std::size_t hash_value<OrdSet>(const CanonMap &sbgmap);

} // namespace LIB

} // namespace SBG
