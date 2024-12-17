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

#include "sbg/dto/lexp_dto.hpp"

namespace SBG {

namespace API {

LExpDTO::LExpDTO() : slope_(1), offset_(0) {}
LExpDTO::LExpDTO(RAT slope, RAT offset) : slope_(slope), offset_(offset) {}

member_imp(LExpDTO, RAT, slope);
member_imp(LExpDTO, RAT, offset);

std::ostream &operator<<(std::ostream &out, const LExpDTO &le)
{
  RAT slo = le.slope_, off = le.offset_;

  if (slo == 0) {
    out << off;
    return out;
  }

  if (slo == 1) {
    out << "x";
  } else {
    if (slo.numerator() != 1)
      out << slo.numerator();

    if (slo.denominator() != 1)
      out << "x/" << slo.denominator();

    else
      out << "x";
  }

  if (off != 0) {
      out << "+" << off;
  }

  return out;
}

} // namespace API

} // namespace SBG
