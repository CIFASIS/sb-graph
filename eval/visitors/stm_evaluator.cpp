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

#include "eval/visitors/expr_evaluator.hpp"
#include "eval/visitors/stm_evaluator.hpp"

namespace SBG {

namespace Eval {

namespace detail {

StmEvaluator::StmEvaluator(EvalContext& eval_ctx) : _eval_context(eval_ctx) {}

EvalContext& StmEvaluator::eval_context() const
{
  return _eval_context;
}

StmResult StmEvaluator::operator()(AST::Assign assgn) const 
{
  ExprEvaluator eval_expr(_eval_context);
  ExprBaseType e = boost::apply_visitor(eval_expr, assgn.r());
  _eval_context.insertVariable(assgn.l(), e);

  return StmResult(assgn.l(), e);
}

StmResult StmEvaluator::operator()(AST::ConfigDims cfg) const
{
  return StmResult("", cfg.nmbr_dims());
}

} // namespace detail

} // namespace Eval

} // namespace SBG
