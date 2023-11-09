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

bool compatible(Interval i, LExp le)
{
  Util::RATIONAL st_rat(i.step()), min_rat(minElem(i)); 
  Util::RATIONAL im_step = st_rat * le.slope(), im_begin = min_rat * le.slope() + le.offset();
  if (im_step.denominator() != 1 || im_begin.denominator() != 1) {
    if (im_step.numerator() != 0 || im_begin.numerator() != 0) {
      Util::ERROR("Ill-formed SBGMap");
      return false;
    }
  }

  return true;
}

bool compatible(SetPiece mdi, Exp mdle)
{
  if (isEmpty(mdi)) {
    Util::ERROR("LIB::SBGMap::compatible: empty mdis not allowed");
    return false;
  }

  if (mdi.size() == mdle.size()) {
    parallel_foreach2 (mdi.intervals_ref(), mdle.exps_ref()) {
      Interval i = boost::get<0>(items);
      LExp le = boost::get<1>(items);
      if (!compatible(i, le)) { return false; }
    }
  }

  return true;
}

SBGMap::SBGMap() : dom_(Set()), exp_(Exp()) {}
SBGMap::SBGMap(Interval i, LExp le) : dom_(), exp_() {
  if (compatible(i, le)) {
    set_dom(Set(SetPiece(i)));
    set_exp(Exp(le));
  }
}
SBGMap::SBGMap(SetPiece mdi, Exp mdle) : dom_(), exp_() {
  if (compatible(mdi, mdle)) {
    set_dom(Set(mdi));
    set_exp(mdle);
  }
}
SBGMap::SBGMap(Set dom, Exp exp) : dom_(dom), exp_(exp) {
  auto f = [exp](SetPiece mdi) {
    compatible(mdi, exp);
    return;
  };
  foreach(f, dom);
}

member_imp(SBGMap, Set, dom);
member_imp(SBGMap, Exp, exp);

bool SBGMap::operator==(const SBGMap &other) const
{
  if (dom() == other.dom()) {
    if (cardinal(dom()) == 1) {
      if (image(*this) == image(other)) return true;
    }

    else if (exp() == other.exp()) return true;
  }

  return false;
}

bool SBGMap::operator!=(const SBGMap &other) const { return !(*this == other); }

std::ostream &operator<<(std::ostream &out, const SBGMap &sbgmap)
{
  out << sbgmap.dom() << " ↦ " << sbgmap.exp();

  return out;
}

// SBGMap functions ------------------------------------------------------------

SBGMap restrict(Set subdom, SBGMap sbgmap)
{
  Set restricted_dom = intersection(sbgmap.dom_ref(), subdom);
  return SBGMap(restricted_dom, sbgmap.exp());
}

Interval image(Interval i, LExp le) {
  Util::RATIONAL m = le.slope(), h = le.offset(); 
  Util::NAT new_begin = 0, new_step = 0, new_end = 0;

  if (isId(le)) return i;

  if (isConstant(le)) {
    Util::NAT off = toNat(le.offset());
    return Interval(off, 1, off);
  }

  Util::RATIONAL rat_inf(Util::INT_Inf, 1);
  if (m == rat_inf || m > rat_inf) return Interval(0, 1, Util::Inf);

  // Increasing expression
  if (m > 0) {
    new_begin = toNat(m * minElem(i) + h); 
    new_step = toNat(m * i.step());
    new_end = toNat(m * maxElem(i) + h); 
  }

  // Decreasing expression
  else if (m < 0) {
    new_begin = toNat(m * minElem(i) + h);
    new_step = toNat(m * i.step());
    new_end = toNat(m * maxElem(i) + h); 
  }

  return Interval(new_begin, new_step, new_end);
}

SetPiece image(SetPiece mdi, Exp mdle)
{
  SetPiece res;

  if (isEmpty(mdi)) return mdi;

  if (mdi.size() == mdle.size()) {
    parallel_foreach2 (mdi.intervals_ref(), mdle.exps_ref()) {
      Interval i = boost::get<0>(items);
      LExp le = boost::get<1>(items);
      res.emplaceBack(image(i, le));
    }
 
    return res;
  }

  Util::ERROR("LIB::SBGMap::image: dimensions don't match");
  return SetPiece();
}

Set image(SBGMap sbgmap) { return image(sbgmap.dom(), sbgmap);}

Set image(Set subdom, SBGMap sbgmap)
{
  Set res;

  Exp le = sbgmap.exp();
  Set capdom = intersection(sbgmap.dom_ref(), subdom);

  auto f = [le, &res](SetPiece atom) {
    SetPiece new_atom = image(atom, le);
    res.emplace(new_atom);
  };
  foreach(f, capdom);

  return res;
}

Set preImage(SBGMap sbgmap) { return sbgmap.dom(); }

Set preImage(Set subcodom, SBGMap sbgmap)
{
  SBGMap inv(image(sbgmap), inverse(sbgmap.exp()));
  Set im = image(sbgmap);
  Set cap_subcodom = intersection(im, subcodom);
  Set inv_im = image(cap_subcodom, inv);

  return intersection(sbgmap.dom_ref(), inv_im);
}

SBGMap composition(SBGMap sbgmap1, SBGMap sbgmap2)
{
  Set dom1 = sbgmap1.dom(), im2 = image(sbgmap2);
  Set res_dom = intersection(im2, dom1);
  res_dom = preImage(res_dom, sbgmap2);
  Exp res_exp = composition(sbgmap1.exp(), sbgmap2.exp());

  return SBGMap(res_dom, res_exp);
}

// Extra functions -------------------------------------------------------------

SBGMap minInv(Set d, SBGMap sbgmap)
{
  Set dom = sbgmap.dom(), res_dom = image(restrict(d, sbgmap));
  Exp e = sbgmap.exp();

  if (cardinal(dom) == 1 || isConstant(e)) 
    return SBGMap(res_dom, Exp(minElem(sbgmap.dom())));

  return SBGMap(res_dom, inverse(e));
}

SBGMap minInv(SBGMap sbgmap) { return minInv(sbgmap.dom(), sbgmap); }

bool isId(SBGMap sbgmap)
{
  if (cardinal(sbgmap.dom()) == 1) return sbgmap.dom() == image(sbgmap);

  return isId(sbgmap.exp());
}

// Function should be called on a non-empty sbgmap
unsigned int nmbrDims(SBGMap sbgmap) { return sbgmap.dom().size(); }

std::size_t hash_value(const SBGMap &sbgmap)
{
  std::size_t seed = 0;
  boost::hash_combine(seed, sbgmap.dom());
  boost::hash_combine(seed, sbgmap.exp());

  return seed;
}

} // namespace LIB

} // namespace SBG
