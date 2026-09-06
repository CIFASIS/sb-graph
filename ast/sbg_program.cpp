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

SBGProgram::SBGProgram() : arity_(1), stms_(), exprs_() {}
SBGProgram::SBGProgram(StatementList stms, ExprList exprs) : arity_(1)
  , stms_(stms), exprs_(exprs) {}
SBGProgram::SBGProgram(std::size_t arity, StatementList stms, ExprList exprs) 
  : arity_(arity), stms_(stms), exprs_(exprs) {}

member_imp(SBGProgram, std::size_t, arity);
member_imp(SBGProgram, StatementList, stms);
member_imp(SBGProgram, ExprList, exprs);

std::ostream &operator<<(std::ostream &out, const SBGProgram &prog)
{
  if (!prog.stms().empty())
    out << prog.stms() << "\n";
  out<< prog.exprs();  

  return out;
}

} // namespace AST

} // namespace SBG
