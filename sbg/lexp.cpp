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

LExp::LExp() : slope_(1), offset_(0) {}
LExp::LExp(RAT slope, RAT offset) : slope_(slope), offset_(offset) {}

member_imp(LExp, RAT, slope);
member_imp(LExp, RAT, offset);

LExp LExp::operator+(const LExp &r)
{
  return LExp(slope() + r.slope(), offset() + r.offset());
}

LExp LExp::operator-(const LExp &r)
{
  return LExp(slope() - r.slope(), offset() - r.offset());
}

LExp LExp::operator*(const LExp &r)
{
  Util::ERROR_UNLESS(isZero(r.slope())
                     , "LIB::LExp::operator*: result is not a LExp");

  return LExp(slope() * r.offset(), offset()); 
}

bool LExp::operator==(const LExp &other) const
{
  return slope() == other.slope() && offset() == other.offset();
}

bool LExp::operator<(const LExp &other) const 
{
  if (slope() < other.slope()) return true;

  return offset() < other.offset();
}

std::ostream &operator<<(std::ostream &out, const LExp &le)
{
  RAT slo = le.slope(), off = le.offset();

  if (!isZero(slo) && !isOne(slo)) {
    if (slo.numerator() != 1)
      out << slo.numerator();

    if (slo.denominator() != 1)
      out << "x/" << slo.denominator();

    else
      out << "x";
  }

  if (isOne(slo)) out << "x";

  if (!isZero(off)) {
    if (off > 0 && !isZero(slo))
      out << "+" << off;

    else
      out << off;
  }

  if (isZero(slo) && isZero(off)) out << "0"; 

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

  // Non constant map
  RAT slo = le.slope();
  if (slo != zero) {
    new_slope = RAT(slo.denominator(), slo.numerator());
    new_offset = (-le.offset())/slo;
  }

  // Constant map
  else {
    new_slope = RAT(Util::INT_Inf, 1);
    new_offset = RAT(-Util::INT_Inf, 1);
  }

  return LExp(new_slope, new_offset);
}

LExp mod(LExp le1, LExp le2) 
{
  Util::ERROR_UNLESS(isZero(le2.slope())
                     , "LIB::LExp::mod: not supported currently");
 
  return LExp();
}

LExp floorDiv(LExp le1, LExp le2)
{
  Util::ERROR_UNLESS(isZero(le2.slope())
                     , "LIB::LExp::floorDiv: result is not a LExp");

  Util::RATIONAL div = le2.offset();
  return LExp(le1.slope() / div, le1.offset() / div);
}

LExp ceilDiv(LExp le1, LExp le2)
{
  Util::ERROR_UNLESS(isZero(le2.slope())
                     , "LIB::LExp::ceilDiv: result is not a LExp");

  Util::RATIONAL div = le2.offset();
  return LExp(le1.slope() / div, le1.offset() / div);
}

bool isId(LExp le) { return le.slope() == 1 && le.offset() == 0; }

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
