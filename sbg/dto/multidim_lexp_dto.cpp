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

#include "sbg/dto/multidim_lexp_dto.hpp"

namespace SBG {

namespace API {

MDLExpDTO::MDLExpDTO() : exps_() {}
MDLExpDTO::MDLExpDTO(Util::MD_NAT x)
{
  for (Util::NAT xi : x)
    exps_.emplace_back(LExpDTO(0, Util::RATIONAL(xi)));
}
MDLExpDTO::MDLExpDTO(LExpDTO le) : exps_() { exps_.emplace_back(le); }
MDLExpDTO::MDLExpDTO(unsigned int nmbr_copies, LExpDTO le) : exps_()
{  
  for (unsigned int j = 0; j < nmbr_copies; ++j)
    exps_.emplace_back(le);
}
MDLExpDTO::MDLExpDTO(LExpDTOVector v) : exps_(v) {}

member_imp(MDLExpDTO, LExpDTOVector, exps);

void MDLExpDTO::emplaceBack(LExpDTO le) { exps_.emplace_back(le); }

LExpDTO &MDLExpDTO::operator[](std::size_t n) { return exps_[n]; }
const LExpDTO &MDLExpDTO::operator[](std::size_t n) const { return exps_[n]; }

std::ostream &operator<<(std::ostream &out, const MDLExpDTO &mdle)
{
  unsigned int sz = mdle.exps_.size();

  if (sz > 0) {
    for (unsigned int j = 0; j < sz-1; ++j) 
      out << mdle[j] << "|";
    out << mdle[sz-1];
  }

  return out;
}

} // namespace API

} // namespace SBG
