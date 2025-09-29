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

#include "eval/user_impl_map.hpp"
#include "eval/visitors/autom_impl_visitor.hpp"
#include "eval/visitors/stm_evaluator.hpp"
#include "eval/visitors/set_impl_visitor.hpp"
#include "sbg/af_pwmap.hpp"

namespace SBG {

namespace Eval {

////////////////////////////////////////////////////////////////////////////////
// Automatic Implementation Visitor --------------------------------------------
////////////////////////////////////////////////////////////////////////////////

AutomImplVisitor::AutomImplVisitor() {}

void AutomImplVisitor::visit(AST::SBGProgram p) const
{ 
  // Statement inspection ------------------------------------------------------ 

  LIB::NAT dims = 1;
  AST::IsConfig cfg_visit;
  if (!p.stms().empty()) {
    AST::Statement first = p.stms()[0];
    if (boost::apply_visitor(cfg_visit, first))
      dims = boost::get<AST::ConfigDims>(first).nmbr_dims();
  }

  EvalContext eval_ctx;
  StmEvaluator stm_eval(eval_ctx);
  for (AST::Statement stm : p.stms()) {
    if (!boost::apply_visitor(cfg_visit, stm))
      StmResult se = boost::apply_visitor(stm_eval, stm);
  }

  // Set Implementation --------------------------------------------------------

  int auto_set_impl = 2;
  if (dims < 2) {
    SetImplExprVisitor set_impl_visit(stm_eval.eval_ctx().venv());
    for (AST::Expr expr : p.exprs()) {
      int ith_set_impl = boost::apply_visitor(set_impl_visit, expr);
      auto_set_impl = std::min(auto_set_impl, ith_set_impl);
    }
  }

  LIB::SetFactPtr set_fact = std::get<LIB::SetFactPtr>(IMPL_MAP.getFactory("set"
    , auto_set_impl));
  LIB::SetFactory::instance().set_set_fact(std::move(set_fact));

  return;
}

} // namespace Eval

} // namespace SBG
