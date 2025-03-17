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

#include "eval/visitors/program_visitor.hpp"

namespace SBG {

namespace Eval {

ProgramVisitor::ProgramVisitor(const LIB::PWMapAF &fact, bool debug)
  : fact_(fact), env_(), debug_(debug) {}

ProgramIO ProgramVisitor::operator()(AST::Program p) const 
{ 
  LIB::NAT dims = 1;
  AST::StatementList stms;
  ExprEvalList exprs;

  AST::IsConfig cfg_visit;
  StmVisitor stm_visit;
  for (AST::Statement s : p.stms()) {
    if (boost::apply_visitor(cfg_visit, s))
      dims = boost::get<AST::ConfigDims>(s).nmbr_dims();

    else {
      boost::apply_visitor(stm_visit, s);
      stms.push_back(s);
    }
  }

  EvalExpression eval_expr(dims, fact_, stm_visit.env(), debug_);
  for (AST::Expr e : p.exprs()) {
    ExprBaseType expr_res = boost::apply_visitor(eval_expr, e);
    exprs.push_back(ExprEval(e, expr_res));
  }

  return ProgramIO(dims, stms, exprs);
}

} // namespace Eval

} // namespace SBG
