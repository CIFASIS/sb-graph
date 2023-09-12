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

#define BOOST_SPIRIT_DEBUG

#include <boost/phoenix.hpp>
#include <boost/rational.hpp>

#include "ast/expr.hpp"
#include "parser/skipper.hpp"

namespace SBG {

namespace Parser {

namespace phx = boost::phoenix;

template <typename Iterator>
struct ExprRule : qi::grammar<Iterator, Skipper<Iterator>, AST::ExprList()> {
  ExprRule(Iterator &it);

  // Rules with no skip
  qi::rule<Iterator> comment, TRUE, FALSE;
  qi::rule<Iterator, AST::Name()> ident;
  qi::rule<Iterator, AST::Boolean()> boolean;

  // Operators tokens
  qi::rule<Iterator> OPAREN, CPAREN, OBRACKET, CBRACKET, OBRACE, CBRACE, COLON, RAT, COMA, ARROW, OANGLE, CANGLE, DIMS, PIPE;

  // Other rules
  qi::rule<Iterator, Skipper<Iterator>, Util::RATIONAL()> rational;
  qi::rule<Iterator, Skipper<Iterator>, AST::Call()> call_exp;
  qi::rule<Iterator, Skipper<Iterator>, AST::ExprList()> function_call_args;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> primary;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> factor;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> term;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> arithmetic_expr;

  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> interval;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> interval_unary;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> interval_binary;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> interval_expr;
  qi::rule<Iterator, Skipper<Iterator>, AST::ExprList()> inter_list;

  qi::rule<Iterator, Skipper<Iterator>, AST::ExprList()> inter_times;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> md_inter;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> mdi_unary;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> mdi_binary;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> mdi_expr;
  qi::rule<Iterator, Skipper<Iterator>, AST::ExprList()> mdi_list;

  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> set;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> set_unary;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> set_binary;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> set_expr;

  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> numeric;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> lexp;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> lexp_binary;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> lexp_expr;

  qi::rule<Iterator, Skipper<Iterator>, AST::ExprList()> lexp_pipe;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> mdlexp;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> mdlexp_binary;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> mdlexp_expr;


  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> sbgmap;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> map_expr;
  qi::rule<Iterator, Skipper<Iterator>, AST::ExprList()> map_list;

  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> pwl;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> pwl_expr;

  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> expr;
  qi::rule<Iterator, Skipper<Iterator>, AST::ExprList()> expr_list;
  qi::rule<Iterator, Skipper<Iterator>, AST::ExprList()> exprs_comments;

  Iterator &it;
};

} // namespace Parser

} // namespace SBG

#endif
