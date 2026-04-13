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
#include "sbg/pwmap_fact.hpp"

namespace SBG {

namespace Eval {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Automatic Implementation Visitor --------------------------------------------
////////////////////////////////////////////////////////////////////////////////

AutomImplVisitor::AutomImplVisitor() {}

EvalUserInput AutomImplVisitor::visit(AST::SBGProgram p) const
{ 
  // Statement inspection ------------------------------------------------------ 

  AST::IsConfig cfg_visit;
  EvalContext eval_context;
  if (!p.stms().empty()) {
    AST::Statement first = p.stms()[0];
    if (boost::apply_visitor(cfg_visit, first)) {
      eval_context.setArity(boost::get<AST::ConfigDims>(first).nmbr_dims());
    }
  }

  StmEvaluator stm_eval{eval_context};
  for (AST::Statement stm : p.stms()) {
    if (!boost::apply_visitor(cfg_visit, stm)) {
      StmResult se = boost::apply_visitor(stm_eval, stm);
    }
  }

  // Set Implementation --------------------------------------------------------

  int auto_set_impl = 2;
  SetImplExprVisitor set_impl_visit{stm_eval.eval_context().venv()};
  for (AST::Expr expr : p.exprs()) {
    int ith_set_impl = boost::apply_visitor(set_impl_visit, expr);
    auto_set_impl = std::min(auto_set_impl, ith_set_impl);
  }

  EvalUserInput result;
  result.set_set_impl(auto_set_impl);

  // PW Implementation ---------------------------------------------------------

  result.set_pw_impl(1);
  if (auto_set_impl > 0) {
    result.set_pw_impl(2);
  }

  return result;
}

} // namespace detail

} // namespace Eval

} // namespace SBG
