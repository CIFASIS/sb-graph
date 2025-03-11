/** @file multidim_lexp.hpp

 @brief <b>Multi-dimensional linear expressions implementation</b>

 A Multi-dimensional linear expression (mdle) le1 | ... | lek is an ordered
 collection of linear expressions.

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

  /**
   * @brief Empty multi-dimensional expression constructor.
   */
  MDLExp();

  /**
   * @brief Constructs a constant mdle in all dimensions that maps to x.
   */
  MDLExp(const MD_NAT &x);

  /**
   * @brief Constructs a one-dimensional mdle composed only by le.
   */
  MDLExp(const LExp &le);

  /**
   * @brief Constructs a mdle of dimension nmbr_copies with le as linear
   * expression in each dimensions.
   */
  MDLExp(unsigned int nmbr_copies, const LExp &le);

  /**
   * @brief Collection move constructor.
   */
  MDLExp(const LExpVector &v);

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

  // Traditional expression operations -----------------------------------------

  /**
   * @brief Number of dimensions of the mdle, i.e. arity(1*x+0 | 1*x+0) = 2.
   */
  std::size_t arity() const;

  /**
   * @brief Calculate the composition of mdle1 with mdle2, i.e. mdle1(mdle2)
   */
  MDLExp composition(const MDLExp &other) const;

  /**
   * @brief Calculates the inverse dimension by dimension.
   */
  MDLExp inverse() const;

  // Extra operations ----------------------------------------------------------

  bool isId() const;
  bool isConstant() const;
};
std::ostream &operator<<(std::ostream &out, const MDLExp &mdle);

typedef MDLExp Exp;

} // namespace LIB

} // namespace SBG

#endif
