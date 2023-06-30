/** @file stm_visitor.hpp

 @brief <b>Statement visitor</b>

 Statement visitor designed to create a variable environment.

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

#ifndef AST_VISITOR_STATEMENT 
#define AST_VISITOR_STATEMENT 

#include <ast/statement.hpp>
#include <eval/visitors/eval_expr.hpp>

namespace SBG {

namespace Eval {

struct StmVisitor : public boost::static_visitor<void> {
  public:
  StmVisitor();
 
  VarEnv env();
  void operator()(AST::Assign asgn) const; 

  private:
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
