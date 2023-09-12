/** @file statement.hpp

 @brief <b>Statement parser definition</b>

 Tiny parser of statements.

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

#ifndef STATEMENT_PARSER_HPP
#define STATEMENT_PARSER_HPP

#include "ast/statement.hpp"
#include "parser/expr.hpp"

namespace SBG {

namespace Parser {

template <typename Iterator>
struct StmRule : qi::grammar<Iterator, Skipper<Iterator>, AST::StatementList()> {
  StmRule(Iterator &it);

  // Rules with no skip
  qi::rule<Iterator, Util::VariableName()> ident;

  // Operators tokens
  qi::rule<Iterator> ASSIGN, NMBR_DIMS;

  // Other rules
  ExprRule<Iterator> expr;
  qi::rule<Iterator, Skipper<Iterator>, AST::Statement()> cfg_dims;
  qi::rule<Iterator, Skipper<Iterator>, AST::Statement()> assign;
  qi::rule<Iterator, Skipper<Iterator>, AST::Statement()> stm;
  qi::rule<Iterator, Skipper<Iterator>, AST::StatementList()> stms_comments;

  Iterator &it;
};

} // namespace Parser

} // namespace SBG

#endif
