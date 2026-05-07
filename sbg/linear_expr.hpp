/** @file linear_expr.hpp

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

#ifndef SBGRAPH_SBG_LINEAR_EXPR_HPP_
#define SBGRAPH_SBG_LINEAR_EXPR_HPP_

#include "sbg/rational.hpp"

#include "rapidjson/document.h"

#include <iosfwd>

namespace SBG {

namespace LIB {

namespace detail {

class LinearExpr {
public:
  /**
   * @brief Identity constructor.
   */
  LinearExpr();

  /**
   * @brief Construct a linear expression defining the \p slope and \p offset.
   */
  LinearExpr(RATIONAL slope, RATIONAL offset);

  const RATIONAL& slope() const;
  const RATIONAL& offset() const;

  bool operator==(const LinearExpr &other) const;
  bool operator!=(const LinearExpr &other) const;
  LinearExpr operator+(const LinearExpr &other) const;
  LinearExpr operator-(const LinearExpr &other) const;

  // Traditional linear expression operations ----------------------------------

  /**
   * @brief Calculates the result of applying the linear expression to \p x.
   * Precondition: result is >= 0.
   */
  NAT apply(const NAT& x) const;

  /**
   * @brief Calculate the composition of \p this with \p other, i.e.
   * \p this(\p other).
   */
  LinearExpr composition(const LinearExpr &other) const;

  /**
   * @brief Calculates the inverse of a linear expression.
   * Precondition: slope should be non-zero.
   */
  LinearExpr inverse() const;

  // Extra operations ----------------------------------------------------------

  bool isId() const;
  bool isConstant() const;

  /**
   * @brief Checks if the expression is injective for all naturals.
   */
  bool isInjective() const;

  /**
   * Precondition: both slopes should be different.
   */
  RATIONAL intersectionPoint(const LinearExpr& other) const;

private:
  RATIONAL _slope;
  RATIONAL _offset;
};
std::ostream &operator<<(std::ostream &out, const LinearExpr& le);

// Non-member functions --------------------------------------------------------

rapidjson::Value toJSON(LinearExpr le
  , rapidjson::Document::AllocatorType& alloc);

} // namespace detail

} // namespace LIB

} // namespace SBG

#endif // SBGRAPH_SBG_LINEAR_EXPR_HPP_
