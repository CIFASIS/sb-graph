/** @file expression.hpp

 @brief <b>Multi-dimensional Expression implementation</b>

 The current implementation for expressions that will be used as laws to define
 maps is an ordered collection of linear expressions.

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

#ifndef SBGRAPH_SBG_EXPRESSION_HPP_
#define SBGRAPH_SBG_EXPRESSION_HPP_

#include "sbg/expression_impl.hpp"
#include "sbg/fixed_points.hpp"
#include "sbg/linear_expr.hpp"
#include "sbg/multidim_inter.hpp"

#include <vector>

namespace SBG {

namespace LIB {

namespace detail {

class MapDetail;

} // namespace detail

class Expression {
public:
  /**
   * @brief Empty multi-dimensional expression constructor.
   */
  Expression();

  /**
   * @brief Constructs a constant mdle in all dimensions that maps to \p x.
   */
  Expression(const MD_NAT& x);

  /**
   * @brief Constructs a one-dimensional linear expression.
   */
  Expression(const RATIONAL& slope, const RATIONAL& offset);

  /**
   * @brief Constructs a multi-dimensional expression of arity \p nmbr_copies
   * with \p the same linear expression in each dimension.
   */
  Expression(std::size_t n, const RATIONAL& slope, const RATIONAL& offset);

  bool operator==(const Expression& other) const;
  bool operator!=(const Expression& other) const;
  Expression operator+(const Expression& other) const;
  Expression operator-(const Expression& other) const;
  std::ostream& print(std::ostream& out) const;

  // Traditional expression operations -----------------------------------------

  /**
   * @brief Number of dimensions of the mdle, i.e. arity(1*x+0 | 1*x+0) = 2.
   */
  std::size_t arity() const;

  MD_NAT apply(const MD_NAT& x) const;

  /**
   * @brief Calculate the composition of \p this with \p other, i.e.
   * \p this(\p other).
   */
  Expression composition(const Expression& other) const;

  /**
   * @brief Calculates the inverse dimension by dimension.
   * Precondition: the expression in each dimension should be bijective for all
   * naturals.
   */
  Expression inverse() const;

  // Extra operations ----------------------------------------------------------

  bool isId() const;
  bool isConstant() const;

  /**
   * @brief Checks if the expression is injective for all multi-dimensional
   * naturals.
   */
  bool isInjective() const;

  /**
   * @brief Calculates all the elements x ∈  [0:1:Inf]^k such that replacing x
   * in the current expression returns x as result.
   */
  FixedPointsInfo fixedPoints() const;

  /**
   * @brief Given two expressions f and g such that arity(f) = n and
   * arity(g) = m it returns an expression h such that arity(h) = n+m and
   * g(x1, ..., x(n+m)) = (f(x1, ..., xn), g(x(n+1), ..., x(n+m))) flattened.
   */
  Expression cartesianProduct(const Expression& other) const;

private:
  detail::ExpressionImpl _impl;

  Expression(const detail::ExpressionImpl& impl);
  Expression(detail::ExpressionImpl&& impl);

  detail::LinearExpr& operator[](std::size_t n);
  const detail::LinearExpr& operator[](std::size_t n) const;

  friend class detail::MapDetail;
};
std::ostream& operator<<(std::ostream& out, const Expression& mdle);

} // namespace LIB

} // namespace SBG

#endif // SBGRAPH_SBG_EXPRESSION_HPP_
