/** @file multidim_lexp.hpp

 @brief <b>Multi-dimensional inear expressions implementation</b>

 <hr>

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

#ifndef SBG_MULTIDIM_LEXP_HPP
#define SBG_MULTIDIM_LEXP_HPP

#include "lexp.hpp"

namespace SBG {

namespace LIB {

typedef boost::container::vector<LExp> LExpVector;
typedef boost::container::vector<LExp>::iterator LExpVectorIt;
typedef boost::container::vector<LExp>::const_iterator LExpVectorConstIt;

struct MDLExp {
  typedef LExpVectorIt iterator;
  typedef LExpVectorConstIt const_iterator;

  member_class(LExpVector, exps);

  MDLExp();
  MDLExp(Util::MD_NAT x); // Expression mapping to x
  MDLExp(unsigned int dimensions);
  MDLExp(LExp le);
  MDLExp(unsigned int nmbr_copies, LExp le);
  MDLExp(LExpVector v);

  iterator begin();
  iterator end();

  std::size_t size() const;
  void emplaceBack(LExp le);
  LExp &operator[](std::size_t n);

  MDLExp operator+(const MDLExp &le); 
  MDLExp operator-(const MDLExp &le); 

  eq_class(MDLExp);
  lt_class(MDLExp);
};
std::ostream &operator<<(std::ostream &out, const MDLExp &mdle);

/**
 * @brief Traditional expression operations.
 */

/* @function composition
 *
 * @brief Calculate the composition of le1 with le2, i.e. le1(le2)
 */

MDLExp composition(MDLExp le1, MDLExp mdle2);
MDLExp inverse(MDLExp mdle);

/**
 * @brief Extra operations.
 */

bool isId(MDLExp mdle);
bool isConstant(MDLExp mdle);
std::size_t hash_value(const MDLExp &mdle);

typedef MDLExp Exp;

} // namespace LIB

} // namespace SBG

#endif
