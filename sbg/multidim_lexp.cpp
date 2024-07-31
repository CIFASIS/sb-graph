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

#include "sbg/multidim_lexp.hpp"

namespace SBG {

namespace LIB {

MDLExp::MDLExp() : exps_() {}
MDLExp::MDLExp(Util::MD_NAT x)
{
  Util::ERROR_UNLESS(x.arity() > 0, "LIB::MDLE1: empty not allowed");

  for (Util::NAT xi : x)
    exps_.emplace_back(LExp(0, Util::RATIONAL(xi)));
}
MDLExp::MDLExp(LExp le) : exps_() { exps_.emplace_back(le); }
MDLExp::MDLExp(unsigned int nmbr_copies, LExp le) : exps_()
{  
  Util::ERROR_UNLESS(nmbr_copies > 0, "LIB::MDLE2: empty not allowed");

  for (unsigned int j = 0; j < nmbr_copies; ++j)
    exps_.emplace_back(le);
}
MDLExp::MDLExp(LExpVector v) : exps_(v) {}

member_imp(MDLExp, LExpVector, exps);

MDLExp::iterator MDLExp::begin() { return exps_.begin(); }
MDLExp::iterator MDLExp::end() { return exps_.end(); }
MDLExp::const_iterator MDLExp::begin() const { return exps_.begin(); }
MDLExp::const_iterator MDLExp::end() const { return exps_.end(); }

void MDLExp::emplaceBack(LExp le) { exps_.emplace_back(le); }

LExp &MDLExp::operator[](std::size_t n) { return exps_[n]; }
const LExp &MDLExp::operator[](std::size_t n) const { return exps_[n]; }

bool MDLExp::operator==(const MDLExp &other) const
{
  return exps_ == other.exps_;
}

bool MDLExp::operator!=(const MDLExp &other) const
{
  return !(*this == other);
}

MDLExp MDLExp::operator+(const MDLExp &other) const
{
  MDLExp res;
  for (unsigned int j = 0; j < arity(); ++j)
    res.emplaceBack(operator[](j) + other[j]);

  return res;
}

MDLExp MDLExp::operator-(const MDLExp &other) const
{
  MDLExp res;
  for (unsigned int j = 0; j < arity(); ++j)
    res.emplaceBack(operator[](j) - other[j]);

  return res;
}

std::ostream &operator<<(std::ostream &out, const MDLExp &mdle)
{
  unsigned int sz = mdle.arity();

  if (sz > 0) {
    for (unsigned int j = 0; j < sz-1; ++j) 
      out << mdle[j] << "|";
    out << mdle[sz-1];
  }

  return out;
}

// Linear expression functions -------------------------------------------------

std::size_t MDLExp::arity() const { return exps_.size(); }

MDLExp MDLExp::composition(const MDLExp &other) const
{
  Util::ERROR_UNLESS(arity() == other.arity()
                     , "LIB::MDLE::composition: dimensions don't match");

  MDLExp res;

  for (unsigned int j = 0; j < arity(); ++j)
    res.emplaceBack(operator[](j).composition(other[j]));

  return res;
}

MDLExp MDLExp::inverse() const
{
  MDLExp res;

  for (const LExp &le : exps_)
    res.emplaceBack(le.inverse());

  return res;
}

bool MDLExp::isId() const
{
  for (const LExp &le : exps_)
    if (!le.isId())
      return false;

  return true;
}

bool MDLExp::isConstant() const
{
  for (const LExp &le : exps_)
    if (!le.isConstant())
      return false;

  return true;
}

} // namespace LIB

} // namespace SBG
