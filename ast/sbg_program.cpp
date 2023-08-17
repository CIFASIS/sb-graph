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

#include "ast/sbg_program.hpp"

namespace SBG {

namespace AST {

Program::Program() : stms_(), exprs_() {}
Program::Program(StatementList stms, ExprList exprs) : stms_(stms), exprs_(exprs) {}

member_imp(Program, StatementList, stms);
member_imp(Program, ExprList, exprs);

std::ostream &operator<<(std::ostream &out, const Program &prog)
{
  out << prog.stms() << "\n" << prog.exprs();  

  return out;
}

} // namespace AST

} // namespace SBG
