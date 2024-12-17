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

#include "sbg/dto/map_dto.hpp"

namespace SBG {

namespace API {

SBGMapDTO::SBGMapDTO() : dom_(SetDTO()), exp_(ExpDTO()) {}
SBGMapDTO::SBGMapDTO(Util::MD_NAT x, ExpDTO exp) : dom_(), exp_() {
  SetPieceDTO mdi(x);
  dom_ = SetDTO(mdi);
  exp_ = exp;
}
SBGMapDTO::SBGMapDTO(IntervalDTO i, LExpDTO le) : dom_(), exp_() {
  dom_ = SetDTO(SetPieceDTO(i));
  exp_ = ExpDTO(le);
}
SBGMapDTO::SBGMapDTO(SetPieceDTO mdi, ExpDTO exp) : dom_(), exp_() {
  dom_ = SetDTO(mdi);
  exp_ = exp;
}
SBGMapDTO::SBGMapDTO(SetDTO dom, ExpDTO exp) : dom_(), exp_() {
  if (!dom.isEmpty()) {
    dom_ = dom;
    exp_ = exp;
  }
}

member_imp(SBGMapDTO, SetDTO, dom);
member_imp(SBGMapDTO, ExpDTO, exp);

bool SBGMapDTO::isEmpty() const { return dom_.isEmpty(); }

std::ostream &operator<<(std::ostream &out, const SBGMapDTO &sbgmap)
{
  out << sbgmap.dom_ << " -> " << sbgmap.exp_;

  return out;
}

} // namespace LIB

} // namespace SBG
