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
  if (isEmpty(mdi)) return true;

  if (mdi.size() == mdle.size()) {
    parallel_foreach2 (mdi.intervals_ref(), mdle.exps_ref()) {
      Interval i = boost::get<0>(items);
      LExp le = boost::get<1>(items);
      if (!compatible(i, le)) { return false; }
    }
  }

  return true;
}

template<typename Set>
SBGMap<Set>::SBGMap() : dom_(Set()), exp_(Exp()) {}
template<typename Set>
SBGMap<Set>::SBGMap(Interval i, LExp le) : dom_(Set(SetPiece(i))), exp_(Exp(le)) {}
template<typename Set>
SBGMap<Set>::SBGMap(Set dom, Exp exp) : dom_(dom), exp_(exp) {
  BOOST_FOREACH (SetPiece mdi, dom) compatible(mdi, exp);
}

member_imp_temp(template<typename Set>, SBGMap<Set>, Set, dom);
member_imp_temp(template<typename Set>, SBGMap<Set>, Exp, exp);

template<typename Set>
bool SBGMap<Set>::operator==(const SBGMap<Set> &other) const
{
  if (dom() == other.dom()) {
    if (cardinal(dom()) == 1) {
      if (image(*this) == image(other)) return true;
    }

    else if (exp() == other.exp()) return true;
  }

  return false;
}

template<typename Set>
bool SBGMap<Set>::operator!=(const SBGMap<Set> &other) const { return !(*this == other); }

template<typename Set>
std::ostream &operator<<(std::ostream &out, const SBGMap<Set> &sbgmap)
{
  out << sbgmap.dom() << " ↦ " << sbgmap.exp();

  return out;
}

// SBGMap functions ------------------------------------------------------------

template<typename Set>
SBGMap<Set> restrict(Set subdom, SBGMap<Set> sbgmap)
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

template<typename Set>
Set image(SBGMap<Set> sbgmap) { return image(sbgmap.dom(), sbgmap);}

template<typename Set>
Set image(Set subdom, SBGMap<Set> sbgmap) 
{
  Set res;

  Exp le = sbgmap.exp();
  Set capdom = intersection(sbgmap.dom_ref(), subdom);
  if (isEmpty(capdom)) return Set();

  else {
    BOOST_FOREACH (SetPiece atom, capdom) {
      SetPiece new_atom = image(atom, le);
      res.emplace(new_atom);
    }
  }

  return res;
}

template<typename Set>
Set preImage(SBGMap<Set> sbgmap) { return sbgmap.dom(); }

template<typename Set>
Set preImage(Set subcodom, SBGMap<Set> sbgmap)
{
  SBGMap inv(image(sbgmap), inverse(sbgmap.exp()));
  Set im = image(sbgmap);
  Set cap_subcodom = intersection(im, subcodom);
  Set inv_im = image(cap_subcodom, inv);

  return intersection(sbgmap.dom_ref(), inv_im);
}

template<typename Set>
SBGMap<Set> composition(SBGMap<Set> sbgmap1, SBGMap<Set> sbgmap2)
{
  Set dom1 = sbgmap1.dom(), im2 = image(sbgmap2);
  Set res_dom = intersection(im2, dom1);
  res_dom = preImage(res_dom, sbgmap2);
  Exp res_exp = composition(sbgmap1.exp(), sbgmap2.exp());

  return SBGMap(res_dom, res_exp);
}

// Extra functions -------------------------------------------------------------

template<typename Set>
SBGMap<Set> minInv(Set d, SBGMap<Set> sbgmap)
{
  Set dom = sbgmap.dom(), res_dom = image(restrict(d, sbgmap));
  Exp e = sbgmap.exp();

  if (cardinal(dom) == 1 || isConstant(e)) 
    return SBGMap<Set>(res_dom, Exp(minElem(sbgmap.dom())));

  return SBGMap<Set>(res_dom, inverse(e));
}

template<typename Set>
SBGMap<Set> minInv(SBGMap<Set> sbgmap) { return minInv(sbgmap.dom(), sbgmap); }

template<typename Set>
bool isId(SBGMap<Set> sbgmap)
{
  if (cardinal(sbgmap.dom()) == 1) return sbgmap.dom() == image(sbgmap);

  return isId(sbgmap.exp());
}

// Function should be called on a non-empty sbgmap
template<typename Set>
unsigned int nmbrDims(SBGMap<Set> sbgmap) { return sbgmap.dom().size(); }

template<typename Set>
std::size_t hash_value(const SBGMap<Set> &sbgmap)
{
  std::size_t seed = 0;
  boost::hash_combine(seed, sbgmap.dom());
  boost::hash_combine(seed, sbgmap.exp());

  return seed;
}

// Template instantiations -----------------------------------------------------

template struct SBGMap<UnordSet>;
template std::ostream &operator<<(std::ostream &out, const BaseMap &sbgmap);
template BaseMap restrict<UnordSet>(UnordSet subdom, BaseMap sbgmap);
template UnordSet image<UnordSet>(BaseMap sbgmap);
template UnordSet image<UnordSet>(UnordSet subdom, BaseMap sbgmap);
template UnordSet preImage<UnordSet>(BaseMap sbgmap);
template UnordSet preImage<UnordSet>(UnordSet subdom, BaseMap sbgmap);
template BaseMap composition<UnordSet>(BaseMap sbgmap1, BaseMap sbgmap2);
template BaseMap minInv<UnordSet>(UnordSet im, BaseMap sbgmap);
template BaseMap minInv<UnordSet>(BaseMap sbgmap);
template bool isId<UnordSet>(BaseMap sbgmap);
template unsigned int nmbrDims<UnordSet>(BaseMap sbgmap);
template std::size_t hash_value<UnordSet>(const BaseMap &sbgmap);

template struct SBGMap<OrdSet>;
template std::ostream &operator<<(std::ostream &out, const CanonMap &sbgmap);
template CanonMap restrict<OrdSet>(OrdSet subdom, CanonMap sbgmap);
template OrdSet image<OrdSet>(CanonMap sbgmap);
template OrdSet image<OrdSet>(OrdSet subdom, CanonMap sbgmap);
template OrdSet preImage<OrdSet>(CanonMap sbgmap);
template OrdSet preImage<OrdSet>(OrdSet subdom, CanonMap sbgmap);
template CanonMap composition<OrdSet>(CanonMap sbgmap1, CanonMap sbgmap2);
template CanonMap minInv<OrdSet>(OrdSet im, CanonMap sbgmap);
template CanonMap minInv<OrdSet>(CanonMap sbgmap);
template bool isId<OrdSet>(CanonMap sbgmap);
template unsigned int nmbrDims<OrdSet>(CanonMap sbgmap);
template std::size_t hash_value<OrdSet>(const CanonMap &sbgmap);

} // namespace LIB

} // namespace SBG
