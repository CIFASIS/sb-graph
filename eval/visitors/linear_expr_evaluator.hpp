/** @file le_evaluator.hpp

 @brief <b>Linear expression evaluator</b>

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

#ifndef SBGRAPH_EVAL_VISITORS_LINEAR_EXPR_EVALUTOR_HPP_
#define SBGRAPH_EVAL_VISITORS_LINEAR_EXPR_EVALUTOR_HPP_

#include "ast/expr.hpp"
#include "eval/var_env.hpp"
#include "sbg/linear_expr.hpp"

#include "boost/variant.hpp"

namespace SBG {

namespace Eval {

namespace detail {

class LinearExprEvaluator
  : public boost::static_visitor<LIB::detail::LinearExpr> {
public:
  LinearExprEvaluator(VarEnv &venv);

  LIB::detail::LinearExpr operator()(AST::Integer v) const;
  LIB::detail::LinearExpr operator()(AST::Rational v) const;
  LIB::detail::LinearExpr operator()(AST::Name v) const;
  LIB::detail::LinearExpr operator()(AST::UnaryOp v) const;
  LIB::detail::LinearExpr operator()(AST::BinOp v) const;
  LIB::detail::LinearExpr operator()(AST::Call v) const;
  LIB::detail::LinearExpr operator()(AST::Interval v) const;
  LIB::detail::LinearExpr operator()(AST::MultiDimInter v) const;
  LIB::detail::LinearExpr operator()(AST::Set v) const;
  LIB::detail::LinearExpr operator()(AST::LinearExp v) const;
  LIB::detail::LinearExpr operator()(AST::MDLExp v) const;
  LIB::detail::LinearExpr operator()(AST::LinearMap v) const;
  LIB::detail::LinearExpr operator()(AST::PWLMap v) const;
  LIB::detail::LinearExpr operator()(AST::SBG v) const;
  LIB::detail::LinearExpr operator()(AST::BipartiteSBG v) const;
  LIB::detail::LinearExpr operator()(AST::DSBG v) const;
  LIB::detail::LinearExpr operator()(AST::ParenExpr v) const;

private:
  mutable VarEnv _venv;
};

} // namespace detail

} // namespace Eval

} // namespace SBG

#endif // SBGRAPH_EVAL_VISITORS_LINEAR_EXPR_EVALUTOR_HPP_
