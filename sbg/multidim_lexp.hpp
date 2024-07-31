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

typedef std::vector<LExp> LExpVector;
typedef LExpVector::iterator LExpVectorIt;
typedef LExpVector::const_iterator LExpVectorConstIt;

struct MDLExp {
  member_class(LExpVector, exps);

  MDLExp();
  MDLExp(Util::MD_NAT x); // Expression mapping to x
  MDLExp(LExp le);
  MDLExp(unsigned int nmbr_copies, LExp le);
  MDLExp(LExpVector v);

  typedef LExpVectorIt iterator;
  typedef LExpVectorConstIt const_iterator;
  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;
  void emplaceBack(LExp le);
  LExp &operator[](std::size_t n);
  const LExp &operator[](std::size_t n) const;

  bool operator==(const MDLExp &other) const;
  bool operator!=(const MDLExp &other) const;

  MDLExp operator+(const MDLExp &other) const;
  MDLExp operator-(const MDLExp &other) const;

  /**
   * @brief Traditional expression operations.
   */

  std::size_t arity() const;
  /* @function composition
   *
   * @brief Calculate the composition of le1 with le2, i.e. le1(le2)
   */
  MDLExp composition(const MDLExp &other) const;
  MDLExp inverse() const;

  /**
   * @brief Extra operations.
   */
  bool isId() const;
  bool isConstant() const;
};
std::ostream &operator<<(std::ostream &out, const MDLExp &mdle);

typedef MDLExp Exp;

} // namespace LIB

} // namespace SBG

#endif
