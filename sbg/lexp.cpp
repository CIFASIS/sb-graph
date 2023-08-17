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

#include "sbg/lexp.hpp"

namespace SBG {

namespace LIB {

LExp::LExp() : slope_(), offset_() {}
LExp::LExp(RAT slope, RAT offset) : slope_(slope), offset_(offset) {}

member_imp(LExp, RAT, slope);
member_imp(LExp, RAT, offset);

LExp LExp::operator+(const LExp &r) { return LExp(slope() + r.slope(), offset() + r.offset()); }

LExp LExp::operator-(const LExp &r) { return LExp(slope() - r.slope(), offset() - r.offset()); }

bool LExp::operator==(const LExp &other) const { return slope() == other.slope() && offset() == other.offset(); }

bool LExp::operator<(const LExp &other) const 
{
  if (slope() < other.slope()) return true;

  return offset() < other.offset();
}

std::ostream &operator<<(std::ostream &out, const LExp &le)
{
  RAT zero, one(1), slo = le.slope();

  if (slo != zero && slo != one) {
    if (slo.numerator() != 1)
      out << slo.numerator();

    if (slo.denominator() != 1)
      out << "x/" << slo.denominator();

    else
      out << "x";
  }

  if (slo == one) out << "x";

  if (le.offset() != zero) {
    if (le.offset() > 0 && le.slope() != 0)
      out << "+" << le.offset();

    else
      out << le.offset();
  }

  return out;
}

// Linear expression functions -------------------------------------------------

LExp composition(LExp le1, LExp le2)
{
  RAT new_slope = le2.slope() * le1.slope();
  RAT new_offset = le1.slope() * le2.offset() + le1.offset();

  return LExp(new_slope, new_offset);
}

LExp inverse(LExp le)
{
  RAT zero, one(1);
  RAT new_slope(0, 1), new_offset(0, 1);

  // 
  RAT slo = le.slope();
  if (slo != zero) {
    new_slope = RAT(slo.denominator(), slo.numerator());
    new_offset = (-le.offset())/slo;
  }

  else {
    new_slope = RAT(Util::Inf, 1);
    new_offset = RAT(-Util::Inf, 1);
  }

  return LExp(new_slope, new_offset);
}

bool isId(LExp le) { return le.slope() == 1 && le.offset() == 0;}

bool isConstant(LExp le) { return le.slope() == 0; }

std::size_t hash_value(const LExp &le)
{
  std::size_t seed = 0;
  boost::hash_combine(seed, le.slope().numerator());
  boost::hash_combine(seed, le.slope().denominator());
  boost::hash_combine(seed, le.offset().numerator());
  boost::hash_combine(seed, le.offset().denominator());

  return seed;
}

} // namespace LIB

} // namespace SBG
