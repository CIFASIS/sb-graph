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

#include <sbg/map.hpp>

namespace SBG {

SBGMap::SBGMap() : dom_(), exp_() {}
SBGMap::SBGMap(SetPiece dom, LExp lexp) : dom_(dom), exp_(lexp) {
  Util::RATIONAL im_step = dom.step() * lexp.slope(), im_begin = minElem(dom) * lexp.slope() + lexp.offset();
  if (im_step.denominator() != 1 || im_begin.denominator() != 1) {
    Util::ERROR("Ill-formed SBGMap");
  }
}

member_imp(SBGMap, SetPiece, dom);
member_imp(SBGMap, LExp, exp);

bool SBGMap::operator==(const SBGMap &other) const
{
  return dom() == other.dom() && exp() == other.exp();
}

bool SBGMap::operator<(const SBGMap &other) const { return dom() < other.dom(); }

// SBGMap functions ------------------------------------------------------------

SetPiece image(SBGMap sbgmap) { return image(sbgmap.dom(), sbgmap);}

SetPiece image(SetPiece subdom, SBGMap sbgmap) 
{
  SetPiece capdom = intersection(sbgmap.dom(), subdom);
  if (isEmpty(capdom)) return SetPiece(); 

  Util::RATIONAL m = sbgmap.exp().slope(), h = sbgmap.exp().offset();

  Util::RATIONAL new_begin = m * minElem(capdom) + h; 
  Util::RATIONAL new_step = m * capdom.step();
  Util::RATIONAL new_end = m * maxElem(capdom) + h; 

  return SetPiece(new_begin.numerator(), new_step.numerator(), new_end.numerator());
}

SetPiece preImage(SBGMap sbgmap) { return preImage(image(sbgmap), sbgmap); }

SetPiece preImage(SetPiece subcodom, SBGMap sbgmap)
{
  SBGMap inv(image(sbgmap), inverse(sbgmap.exp()));
  SetPiece cap_subcodom = intersection(image(sbgmap), subcodom);

  return intersection(sbgmap.dom(), image(cap_subcodom, inv));
}

} // namespace SBG
