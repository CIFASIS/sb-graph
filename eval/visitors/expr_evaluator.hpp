/** @file expr_evaluator.hpp

 @brief <b>Expression evaluator</b>

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

#ifndef SBGRAPH_EVAL_VISITORS_EXPR_EVALUATOR_HPP_ 
#define SBGRAPH_EVAL_VISITORS_EXPR_EVALUATOR_HPP_

#include "ast/expr.hpp"
#include "eval/base_type.hpp"
#include "eval/eval_context.hpp"

#include <boost/variant.hpp>

namespace SBG {

namespace Eval {

namespace detail {

class ExprEvaluator : public boost::static_visitor<ExprBaseType> {
public:
  ExprEvaluator(EvalContext& eval_ctx);

  ExprBaseType operator()(AST::Natural v) const;
  ExprBaseType operator()(AST::Rational v) const;
  ExprBaseType operator()(AST::Name v) const;
  ExprBaseType operator()(AST::UnaryOp v) const;
  ExprBaseType operator()(AST::BinOp v) const;
  ExprBaseType operator()(AST::Call v) const;
  ExprBaseType operator()(AST::Interval v) const;
  ExprBaseType operator()(AST::MultiDimInter v) const;
  ExprBaseType operator()(AST::Set v) const;
  ExprBaseType operator()(AST::LinearExp v) const;
  ExprBaseType operator()(AST::MDLExp v) const;
  ExprBaseType operator()(AST::LinearMap v) const;
  ExprBaseType operator()(AST::PWLMap v) const;
  ExprBaseType operator()(AST::SBG v) const;
  ExprBaseType operator()(AST::BipartiteSBG v) const;
  ExprBaseType operator()(AST::DSBG v) const;
  ExprBaseType operator()(AST::ParenExpr v) const;

private:
  EvalContext& _eval_context;
};

} // namespace detail

} // namespace Eval

} // namespace SBG

#endif // SBGRAPH_EVAL_VISITORS_EXPR_EVALUATOR_HPP_
