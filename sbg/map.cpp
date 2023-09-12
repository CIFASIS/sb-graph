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
  Util::RATIONAL im_step = i.step() * le.slope(), im_begin = minElem(i) * le.slope() + le.offset();
  if (im_step.denominator() != 1 || im_begin.denominator() != 1) {
    Util::ERROR("Ill-formed SBGMap");
    return false;
  }

  return true;
}

bool compatible(SetPiece mdi, Exp mdle)
{
  parallel_foreach2 (mdi.intervals_ref(), mdle.exps_ref()) {
    Interval i = boost::get<0>(items);
    LExp le = boost::get<1>(items);
    if (!compatible(i, le)) return false;
  }

  return true;
}

template<typename Set>
SBGMap<Set>::SBGMap() : dom_(), exp_() {}
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
  if (!(exp() == other.exp())) return false;

  return dom() == other.dom();
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

  parallel_foreach2 (mdi.intervals(), mdle.exps()) {
    Interval i = boost::get<0>(items);
    LExp le = boost::get<1>(items);
    res.emplaceBack(image(i, le));
  }

  return res;
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

  return SBGMap(Set(res_dom), res_exp);
}

// Extra functions -------------------------------------------------------------

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
template std::size_t hash_value<UnordSet>(const BaseMap &sbgmap);

template struct SBGMap<OrdSet>;
template std::ostream &operator<<(std::ostream &out, const CanonMap &sbgmap);
template CanonMap restrict<OrdSet>(OrdSet subdom, CanonMap sbgmap);
template OrdSet image<OrdSet>(CanonMap sbgmap);
template OrdSet image<OrdSet>(OrdSet subdom, CanonMap sbgmap);
template OrdSet preImage<OrdSet>(CanonMap sbgmap);
template OrdSet preImage<OrdSet>(OrdSet subdom, CanonMap sbgmap);
template CanonMap composition<OrdSet>(CanonMap sbgmap1, CanonMap sbgmap2);
template std::size_t hash_value<OrdSet>(const CanonMap &sbgmap);

} // namespace LIB

} // namespace SBG
