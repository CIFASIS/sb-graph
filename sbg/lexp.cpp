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

#include <sbg/lexp.hpp>

namespace SBG {

LExp::LExp() : slope_(), offset_() {}
LExp::LExp(RAT slope, RAT offset) : slope_(slope), offset_(offset) {}

member_imp(LExp, RAT, slope);
member_imp(LExp, RAT, offset);

std::ostream &operator<<(std::ostream &out, const LExp &le)
{
  RAT zero, one(1), slo = le.slope();

  if (slo != zero && slo != one) {
    if (slo.value().numerator() != 1)
      out << slo.value().numerator();

    if (slo.value().denominator() != 1)
      out << "x/" << slo.value().denominator();

    else
      out << "x";
  }

  if (slo == one) out << "x";

  if (le.offset() != zero) out << "+" << le.offset();

  return out;
}

} // namespace SBG
