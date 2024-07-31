/** @file sbg_program.hpp

 @brief <b>SBG program parser definition</b>

 Tiny parser of SBG program.

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

#ifndef PROGRAM_PARSER_HPP
#define PROGRAM_PARSER_HPP

#include "ast/sbg_program.hpp"
#include "parser/statement.hpp"

namespace SBG {

namespace Parser {

template <typename Iterator>
struct SBGProgramRule : qi::grammar<Iterator, Skipper<Iterator>
  , AST::SBGProgram()> {
  SBGProgramRule(Iterator &it);

  // Other rules
  ExprRule<Iterator> expr;
  StmRule<Iterator> stm;
  qi::rule<Iterator, Skipper<Iterator>, AST::SBGProgram()> program_comments;

  Iterator &it;
};

} // namespace Parser

} // namespace SBG

#endif
