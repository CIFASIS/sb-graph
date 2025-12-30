/** @file lexp.hpp

 @brief <b>Linear expressions implementation</b>

 A linear expression m*x+h is determined by its slope and offset. It will be
 used as the law of maps.

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

#ifndef SBG_LEXP_HPP
#define SBG_LEXP_HPP

#include "sbg/rational.hpp"

namespace SBG {

namespace LIB {

class LExp {
  member_class(RATIONAL, slope);
  member_class(RATIONAL, offset);

  /**
   * @brief Identity constructor.
   */
  LExp();

  /**
   * @brief Construct a linear expression defining the \p slope and \p offset.
   */
  LExp(RATIONAL slope, RATIONAL offset);

  bool operator==(const LExp &other) const;
  bool operator!=(const LExp &other) const;

  LExp operator+(const LExp &other) const;
  LExp operator-(const LExp &other) const;

  // Tradiotional le operations ------------------------------------------------

  /**
   * @brief Calculate the composition of \p this with \p other, i.e.
   * \p this(\p other).
   */
  LExp composition(const LExp &other) const;

  /**
   * @brief Calculates the inverse of a linear expression. If it is not bijective
   * , i.e. it is constant, then it returns inf*x-inf.
   */
  LExp inverse() const;

  // Extra operations ----------------------------------------------------------

  bool isId() const;
  bool isConstant() const;
  bool isIncreasing() const;
  RATIONAL intersectionPoint(const LExp& other) const;
};
std::ostream &operator<<(std::ostream &out, const LExp &le);

} // namespace LIB

} // namespace SBG

#endif
