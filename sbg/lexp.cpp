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
  RAT zero, one(1);

  if (le.slope() != zero && le.slope() != one) out << le.slope();

  out << "x";

  if (le.offset() != zero) out << le.offset();

  return out;
}

} // namespace SBG
