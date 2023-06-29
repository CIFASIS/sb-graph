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

#include <eval/visitors/program_visitor.hpp>

namespace SBG {

namespace Eval {

ProgramVisitor::ProgramVisitor() : env_() {}

AST::Program ProgramVisitor::operator()(AST::Program p) const { 
  AST::Program result;

  StmVisitor stm_visit;
  BOOST_FOREACH (AST::Statement s, p.stms()) {
    Apply(stm_visit, s);
    result.stms_ref().push_back(s);
  }

  EvalExpression eval_expr(stm_visit.env());
  BOOST_FOREACH (AST::Expr e, p.exprs()) {
    ExprBaseType expr_res = Apply(eval_expr, e);
    result.exprs_ref().push_back(toExpr(expr_res));
  }

  return result;
}

} // namespace Eval

} // namespace SBG
