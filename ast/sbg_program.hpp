/** @file sbg_program.hpp

 @brief <b>SBG program AST definition</b>

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

#ifndef PARSER_PROGRAM_AST_HPP
#define PARSER_PROGRAM_AST_HPP

#include "ast/statement.hpp"

namespace SBG {

namespace AST {

struct Program {
  member_class(Util::NAT, nmbr_dims);
  member_class(StatementList, stms);
  member_class(ExprList, exprs);

  Program();
  Program(StatementList stms, ExprList exprs);
  Program(Util::NAT nmbr_dims, StatementList stms, ExprList exprs);
};
std::ostream &operator<<(std::ostream &out, const Program &prog);

typedef boost::variant<Program> SBGProgram;

} // namespace AST

} // namespace SBG

#endif
