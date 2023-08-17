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

SBGMap::SBGMap() : dom_(), exp_() {}
SBGMap::SBGMap(Set dom, Exp lexp) : dom_(dom), exp_(lexp) {
  BOOST_FOREACH (SetPiece atom, dom.pieces()) {
    Util::RATIONAL im_step = atom.step() * lexp.slope(), im_begin = minElem(atom) * lexp.slope() + lexp.offset();
    if (im_step.denominator() != 1 || im_begin.denominator() != 1)
      Util::ERROR("Ill-formed SBGMap");
  }
}

member_imp(SBGMap, Set, dom);
member_imp(SBGMap, Exp, exp);

bool SBGMap::operator==(const SBGMap &other) const
{
  if (!(exp() == other.exp())) return false;

  return dom() == other.dom();
}

bool SBGMap::operator!=(const SBGMap &other) const
{
  return !(*this == other);
}

std::ostream &operator<<(std::ostream &out, const SBGMap &sbgmap)
{
  out << sbgmap.dom() << " ↦ " << sbgmap.exp();

  return out;
}

// SBGMap functions ------------------------------------------------------------

SBGMap restrict(Set subdom, SBGMap sbgmap)
{
  Set restricted_dom = intersection(sbgmap.dom(), subdom);
  return SBGMap(restricted_dom, sbgmap.exp());
}

Set image(SBGMap sbgmap) { return image(sbgmap.dom(), sbgmap);}

Set image(Set subdom, SBGMap sbgmap) 
{
  InterSet res;

  Exp le = sbgmap.exp();

  if (isId(le)) return subdom;

  if (isConstant(le)) {
    Util::NAT off = toNat(le.offset());
    return Set(SetPiece(off, 1, off));
  }

  Set capdom = intersection(sbgmap.dom(), subdom);
  if (isEmpty(capdom)) return Set(); 

  else {
    Util::RATIONAL m = le.slope(), h = le.offset();
     
    if (m == 0) {
      SetPiece ith(toNat(h), 1, toNat(h));
      res.emplace_hint(res.cend(), ith);
    }

    // Non-decreasing expression
    else if (m > 0) {
      BOOST_FOREACH (SetPiece atom, capdom.pieces()) {
        Util::RATIONAL new_begin = m * minElem(atom) + h; 
        Util::RATIONAL new_step = m * atom.step();
        Util::RATIONAL new_end = m * maxElem(atom) + h; 

        SetPiece ith(new_begin.numerator(), new_step.numerator(), new_end.numerator());
        res.emplace_hint(res.cend(), ith);
      }
    }

    // Decreasing expression
    else {
      BOOST_REVERSE_FOREACH (SetPiece atom, capdom.pieces()) {
        Util::RATIONAL new_begin = m * minElem(atom) + h; 
        Util::RATIONAL new_step = m * atom.step();
        Util::RATIONAL new_end = m * maxElem(atom) + h; 

        SetPiece ith(new_begin.numerator(), new_step.numerator(), new_end.numerator());
        res.emplace_hint(res.cend(), ith);
      }
    }
  }

  return Set(res);
}

Set preImage(SBGMap sbgmap) { return sbgmap.dom(); }

Set preImage(Set subcodom, SBGMap sbgmap)
{
  SBGMap inv(image(sbgmap), inverse(sbgmap.exp()));
  Set cap_subcodom = intersection(image(sbgmap), subcodom);

  return intersection(sbgmap.dom(), image(cap_subcodom, inv));
}

SBGMap composition(SBGMap sbgmap1, SBGMap sbgmap2)
{
  Set res_dom = intersection(image(sbgmap2), sbgmap1.dom());
  res_dom = preImage(res_dom, sbgmap2);
  Exp res_exp = composition(sbgmap1.exp(), sbgmap2.exp());

  return SBGMap(Set(res_dom), res_exp);
}

// Extra functions -------------------------------------------------------------

MaybeMap canonize(SBGMap sbgmap1, SBGMap sbgmap2)
{
  if (sbgmap1.exp() == sbgmap2.exp()) {
    Set new_dom = concatenation(sbgmap1.dom(), sbgmap2.dom());
    return SBGMap(new_dom, sbgmap1.exp());
  }

  return {};
}

std::size_t hash_value(const SBGMap &sbgmap)
{
  std::size_t seed = 0;
  boost::hash_combine(seed, sbgmap.dom());
  boost::hash_combine(seed, sbgmap.exp());

  return seed;
}

} // namespace LIB

} // namespace SBG
