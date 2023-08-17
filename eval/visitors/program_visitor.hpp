/** @file program_visitor.hpp

 @brief <b>Program visitor</b>

 Program visitor, that will create an environment using a StmVisitor, and
 then it will evaluate the following expressions with an EvalExpression.

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

#ifndef AST_VISITOR_PROGRAM 
#define AST_VISITOR_PROGRAM

#include "ast/sbg_program.hpp"
#include "eval/visitors/stm_visitor.hpp"

namespace SBG {

namespace Eval {

struct ProgramVisitor : public boost::static_visitor<ProgramIO> {
  public:
  ProgramVisitor();
 
  ProgramIO operator()(AST::Program p) const; 

  private:
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
