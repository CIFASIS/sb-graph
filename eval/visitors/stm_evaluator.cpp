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

StmEvaluator::StmEvaluator(unsigned int nmbr_dims, const LIB::PWMapAF &fact)
  : nmbr_dims_(nmbr_dims), fact_(fact), env_() {}

VarEnv StmEvaluator::env() { return env_; }

StmResult StmEvaluator::operator()(AST::Assign assgn) const 
{
  ExprEvaluator eval_expr(nmbr_dims_, fact_, env_, false);
  ExprBaseType e = boost::apply_visitor(eval_expr, assgn.r());
  env_.insert(assgn.l(), e);

  return StmResult(assgn.l(), e);
}

StmResult StmEvaluator::operator()(AST::ConfigDims cfg) const
{
  return StmResult("", ExprBaseType());
}

} // namespace Eval

} // namespace SBG
