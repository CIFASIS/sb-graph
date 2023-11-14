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
  Util::ERROR_UNLESS(x.size() > 0, "LIB::MDLE1: empty not allowed");

  for (Util::NAT xi : x)
    exps_.emplace_back(LExp(0, Util::RATIONAL(xi)));
}
MDLExp::MDLExp(unsigned int dimensions) : exps_() {
  Util::ERROR_UNLESS(dimensions > 0, "LIB::MDLE2: empty not allowed");

  for (unsigned int j = 0; j < dimensions; j++)
    exps_.emplace_back(LExp());
}
MDLExp::MDLExp(LExp le) : exps_() { exps_.emplace_back(le); }
MDLExp::MDLExp(unsigned int nmbr_copies, LExp le) : exps_()
{  
  Util::ERROR_UNLESS(nmbr_copies > 0, "LIB::MDLE3: empty not allowed");

  for (unsigned int j = 0; j < nmbr_copies; j++)
    exps_.emplace_back(le);
}
MDLExp::MDLExp(LExpVector v) : exps_(v) {}

member_imp(MDLExp, LExpVector, exps);

MDLExp::iterator MDLExp::begin() { return exps_.begin(); }
MDLExp::iterator MDLExp::end() { return exps_.end(); }
MDLExp::const_iterator MDLExp::begin() const { return exps_.begin(); }
MDLExp::const_iterator MDLExp::end() const { return exps_.end(); }

std::size_t MDLExp::size() const { return exps_.size(); }

void MDLExp::emplaceBack(LExp le) { exps_.emplace_back(le); }

LExp &MDLExp::operator[](std::size_t n) { return exps_[n]; }
const LExp &MDLExp::operator[](std::size_t n) const { return exps_[n]; }

MDLExp MDLExp::operator+(const MDLExp &r)
{
  MDLExp aux1 = *this, aux2 = r;

  Util::ERROR_UNLESS(aux1.size() == aux2.size()
                     , "LIB::MDLE::operator+: dimensions don't match");

  MDLExp res;
  for (auto const &[le1, le2] : boost::combine(aux1, aux2)) 
    res.emplaceBack(le1 + le2);

  return res;
}

MDLExp MDLExp::operator-(const MDLExp &r)
{
  MDLExp aux1 = *this, aux2 = r;

  Util::ERROR_UNLESS(aux1.size() == aux2.size()
                     , "LIB::MDLE::operator-: dimensions don't match");

  MDLExp res;
  for (auto const &[le1, le2] : boost::combine(aux1, aux2)) 
    res.emplaceBack(le1 - le2);

  return res;
}

bool MDLExp::operator==(const MDLExp &other) const { return exps() == other.exps(); }

bool MDLExp::operator<(const MDLExp &other) const 
{
  MDLExp aux1 = *this, aux2 = other;

  Util::ERROR_UNLESS(aux1.size() == aux2.size()
                     , "LIB::MDLE::operator<: dimensions don't match");

  for (auto const &[le1, le2] : boost::combine(aux1, aux2)) 
    if (le1 < le2)
      return true;

  return false;
}

std::ostream &operator<<(std::ostream &out, const MDLExp &mdle)
{
  unsigned int sz = mdle.size();

  if (sz > 0) {
    for (unsigned int j = 0; j < sz-1; ++j) 
      out << mdle[j] << "|";
    out << mdle[sz-1];
  }

  return out;
}

// Linear expression functions -------------------------------------------------

MDLExp composition(MDLExp mdle1, MDLExp mdle2)
{
  Util::ERROR_UNLESS(mdle1.size() == mdle2.size()
                     , "LIB::MDLE::composition: dimensions don't match");

  MDLExp res;

  for (auto const &[le1, le2] : boost::combine(mdle1, mdle2)) 
    res.emplaceBack(composition(le1, le2));

  return res;
}

MDLExp inverse(MDLExp mdle)
{
  MDLExp res;

  for (LExp le : mdle)
    res.emplaceBack(inverse(le));

  return res;
}

bool isId(MDLExp mdle)
{
  for (LExp le : mdle)
    if (!isId(le))
      return false;

  return true;
}

bool isConstant(MDLExp mdle)
{
  for (LExp le : mdle)
    if (!isConstant(le))
      return false;

  return true;
}

std::size_t hash_value(const MDLExp &mdle)
{
  return boost::hash_range(mdle.begin(), mdle.end());
}

} // namespace LIB

} // namespace SBG
