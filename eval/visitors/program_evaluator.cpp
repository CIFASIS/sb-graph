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
#include "eval/visitors/program_evaluator.hpp"
#include "util/logger.hpp"

namespace SBG {

namespace Eval {

ProgramEvaluator::ProgramEvaluator(const LIB::PWMapAF &fact, bool debug)
  : fact_(fact), env_(), debug_(debug) {}

ProgramIO ProgramEvaluator::operator()(AST::Program p) const 
{ 
  LIB::NAT dims = 1;
  StmResultList stms;
  ExprResultList exprs;

  if (debug_)
    Util::SBGLogger::instance().setLevel(Util::LogLevel::Debug);

  AST::IsConfig cfg_visit;
  if (!p.stms().empty()) {
    AST::Statement first = p.stms()[0];

    if (boost::apply_visitor(cfg_visit, first))
      dims = boost::get<AST::ConfigDims>(first).nmbr_dims();
  }

  StmEvaluator stm_visit(dims, fact_);
  for (AST::Statement s : p.stms()) {
    if (!boost::apply_visitor(cfg_visit, s)) {
      StmResult se = boost::apply_visitor(stm_visit, s);
      stms.push_back(se);
    }
  }

  ExprEvaluator eval_expr(dims, fact_, stm_visit.env(), debug_);
  for (AST::Expr e : p.exprs()) {
    ExprBaseType expr_res = boost::apply_visitor(eval_expr, e);
    exprs.push_back(ExprResult(e, expr_res));
  }

  return ProgramIO(dims, stms, exprs);
}

} // namespace Eval

} // namespace SBG
