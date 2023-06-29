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

#include <eval/visitors/stm_visitor.hpp>

namespace SBG {

namespace Eval {

StmVisitor::StmVisitor() : env_() {}

VarEnv StmVisitor::env() { return env_; }

void StmVisitor::operator()(AST::Assign asgn) const { 
  EvalExpression eval_expr(env_);
  env_.insert(asgn.l(), Apply(eval_expr, asgn.r()));

  return;
}

} // namespace Eval

} // namespace SBG
