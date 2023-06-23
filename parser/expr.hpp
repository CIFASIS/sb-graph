/** @file expr.hpp

 @brief <b>Expression parser definition</b>

 Tiny parser of expressions according to the following syntax:

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

#ifndef EXPR_PARSER_HPP
#define EXPR_PARSER_HPP

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/repository/include/qi_kwd.hpp>

#include <parser/ast/expr.hpp>
#include <parser/defs.hpp>

namespace asc = boost::spirit::ascii;
namespace phx = boost::phoenix;
namespace qi = boost::spirit::qi;

namespace Parser {

template <typename Iterator>
struct ExprRule : qi::grammar<Iterator, asc::space_type, ExprList()> {
  ExprRule(Iterator &it);

  // Rules with no skip
  qi::rule<Iterator> comment, TRUE, FALSE;
  qi::rule<Iterator, Name()> ident;
  qi::rule<Iterator, Boolean()> boolean;

  // Operators tokens
  qi::rule<Iterator> OPAREN, CPAREN, RAT, COMA;

  // Other rules
  qi::rule<Iterator, asc::space_type, SBG::RATIONAL()> rational;
  qi::rule<Iterator, asc::space_type, Call()> call_exp;
  qi::rule<Iterator, asc::space_type, ExprList()> function_call_args;
  qi::rule<Iterator, asc::space_type, Expr()> primary;
  qi::rule<Iterator, asc::space_type, Expr()> factor;
  qi::rule<Iterator, asc::space_type, Expr()> term;
  qi::rule<Iterator, asc::space_type, Expr()> arithmetic_expr;

  qi::rule<Iterator, asc::space_type, Expr()> interval;
  qi::rule<Iterator, asc::space_type, Expr()> interval_expr;

  qi::rule<Iterator, asc::space_type, Expr()> expr;
  qi::rule<Iterator, asc::space_type, ExprList()> exprs_comments;

  Iterator &it;
};

} // namespace Parser

#endif
