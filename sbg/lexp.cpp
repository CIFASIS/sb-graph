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

bool LExp::operator==(const LExp &other) const
{
  return slope_ == other.slope_ && offset_ == other.offset_;
}

bool LExp::operator!=(const LExp &other) const { return !(*this == other); }

bool LExp::operator<(const LExp &other) const
{
  if (slope_ < other.slope_)
    return true;

  return offset_ < other.offset_;
}

LExp LExp::operator+(const LExp &other) const
{
  return LExp(slope_ + other.slope_, offset_ + other.offset_);
}

LExp LExp::operator-(const LExp &other) const
{
  return LExp(slope_ - other.slope_, offset_ - other.offset_);
}

LExp LExp::operator*(const LExp &other) const
{
  Util::ERROR_UNLESS(other.slope_ == 0
                     , "LIB::LExp::operator*: result is not a LExp");

  return LExp(slope_ * other.offset_, offset_);
}

std::ostream &operator<<(std::ostream &out, const LExp &le)
{
  RAT slo = le.slope_, off = le.offset_;

  if (slo != 0 && slo != 1) {
    if (slo.numerator() != 1)
      out << slo.numerator();

    if (slo.denominator() != 1)
      out << "x/" << slo.denominator();

    else
      out << "x";
  }

  if (slo == 1)
    out << "x";

  if (off != 0) {
    if (off > 0 && slo != 0)
      out << "+" << off;

    else
      out << off;
  }

  if (slo == 0 && off == 0)
    out << "0";

  return out;
}

// Linear expression functions -------------------------------------------------

LExp LExp::composition(const LExp &other)const
{
  RAT new_slope = other.slope_ * slope_;
  RAT new_offset = slope_ * other.offset_ + offset_;

  return LExp(new_slope, new_offset);
}

LExp LExp::inverse() const
{
  RAT zero, one(1);
  RAT new_slope(0, 1), new_offset(0, 1);

  // Non constant map
  RAT slo = slope_;
  if (slo != zero) {
    new_slope = RAT(slo.denominator(), slo.numerator());
    new_offset = (-offset_)/slo;
  }

  // Constant map
  else {
    new_slope = RAT(Util::INT_Inf, 1);
    new_offset = RAT(-Util::INT_Inf, 1);
  }

  return LExp(new_slope, new_offset);
}

LExp LExp::mod(const LExp &other) const
{
  Util::ERROR_UNLESS(other.slope_ == 0
                     , "LIB::LExp::mod: not supported currently");
 
  return LExp();
}

LExp LExp::floorDiv(const LExp &other) const
{
  Util::ERROR_UNLESS(other.slope_ == 0
                     , "LIB::LExp::floorDiv: result is not a LExp");

  Util::RATIONAL div = other.offset_;
  return LExp(slope_ / div, offset_ / div);
}

LExp LExp::ceilDiv(const LExp &other) const
{
  Util::ERROR_UNLESS(other.slope_ == 0
                     , "LIB::LExp::ceilDiv: result is not a LExp");

  Util::RATIONAL div = other.offset_;
  return LExp(slope_ / div, offset_ / div);
}

bool LExp::isId() const { return slope_ == 1 && offset_ == 0; }

bool LExp::isConstant() const { return slope_ == 0; }

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
