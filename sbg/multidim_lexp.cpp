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
MDLExp::MDLExp(Util::MD_NAT x) { BOOST_FOREACH (Util::NAT xi, x) emplaceBack(LExp(0, Util::RATIONAL(xi))); }
MDLExp::MDLExp(unsigned int dimensions) : exps_() {
  for (unsigned int j = 0; j < dimensions; j++) emplaceBack(LExp());
}
MDLExp::MDLExp(LExp le) : exps_() { emplaceBack(le); }
MDLExp::MDLExp(unsigned int nmbr_copies, LExp le) : exps_()
{  
  for (unsigned int j = 0; j < nmbr_copies; j++)
    emplaceBack(le);
}
MDLExp::MDLExp(LExpVector v) : exps_(v) {}

member_imp(MDLExp, LExpVector, exps);

MDLExp::iterator MDLExp::begin() { return exps_ref().begin(); }
MDLExp::iterator MDLExp::end() { return exps_ref().end(); }

std::size_t MDLExp::size() const { return exps().size(); }

void MDLExp::emplaceBack(LExp le) { exps_ref().emplace_back(le); }

LExp &MDLExp::operator[](std::size_t n) { return exps_ref()[n]; }

MDLExp MDLExp::operator+(const MDLExp &r)
{
  MDLExp aux1 = *this, aux2 = r;
  if (aux1.size() == aux2.size()) {
    MDLExp res;

    parallel_foreach2 (aux1.exps_ref(), aux2.exps_ref()) {
      LExp le1 = boost::get<0>(items), le2 = boost::get<1>(items);
      res.emplaceBack(le1 + le2);
    }

    return res;
  }

  Util::ERROR("LIB::MDLExp::operator+: dimensions don't match");
  return MDLExp();
}

MDLExp MDLExp::operator-(const MDLExp &r)
{
  MDLExp aux1 = *this, aux2 = r;
  if (aux1.size() == aux2.size()) {
    MDLExp res;
 
    parallel_foreach2 (aux1.exps_ref(), aux2.exps_ref()) {
      LExp le1 = boost::get<0>(items), le2 = boost::get<1>(items);
      res.emplaceBack(le1 - le2);
    }

    return res;
  }

  Util::ERROR("LIB::MDLExp::operator-: dimensions don't match");
  return MDLExp();
}

bool MDLExp::operator==(const MDLExp &other) const { return exps() == other.exps(); }

bool MDLExp::operator<(const MDLExp &other) const 
{
  MDLExp aux1 = *this, aux2 = other;
  if (aux1.size() == aux2.size()) {
    parallel_foreach2 (aux1.exps_ref(), aux2.exps_ref()) {
      LExp le1 = boost::get<0>(items), le2 = boost::get<1>(items);
      if (le1 < le2) return true;
    }

    return false;
  }

  Util::ERROR("LIB::MDLExp::operator<: dimensions don't match");
  return false;
}

std::ostream &operator<<(std::ostream &out, const MDLExp &mdle)
{
  MDLExp aux = mdle;
  unsigned int sz = aux.exps().size();

  if (sz > 0) {
    auto it = aux.begin();
    for (unsigned int j = 0; j < sz-1; ++j) {
      out << *it << "|";
      ++it;
    }
    out << *it;
  }

  return out;
}

// Linear expression functions -------------------------------------------------

MDLExp composition(MDLExp mdle1, MDLExp mdle2)
{
  if (mdle1.size() == mdle2.size()) {
    MDLExp res;

    parallel_foreach2 (mdle1.exps_ref(), mdle2.exps_ref()) {
      LExp le1 = boost::get<0>(items), le2 = boost::get<1>(items);
      res.emplaceBack(composition(le1, le2));
    }

    return res;
  }

  Util::ERROR("LIB::MDLExp::composition: dimensions don't match");
  return MDLExp();
}

MDLExp inverse(MDLExp mdle)
{
  MDLExp res;

  BOOST_FOREACH (LExp le, mdle) res.emplaceBack(inverse(le));

  return res;
}

bool isId(MDLExp mdle)
{
  BOOST_FOREACH (LExp le, mdle)
    if (!isId(le)) return false;

  return true;
}

bool isConstant(MDLExp mdle)
{
  BOOST_FOREACH (LExp le, mdle)
    if (!isConstant(le)) return false;

  return true;
}

std::size_t hash_value(const MDLExp &mdle)
{
  std::size_t seed = 0;

  MDLExp aux = mdle;
  boost::hash_range(seed, aux.begin(), aux.end());

  return seed;
}

} // namespace LIB

} // namespace SBG
