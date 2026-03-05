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

#ifndef SBGRAPH_PARSER_EXPR_HPP_
#define SBGRAPH_PARSER_EXPR_HPP_

#include "ast/expr.hpp"
#include "parser/skipper.hpp"

namespace SBG {

namespace Parser {

namespace qi = boost::spirit::qi;

template <typename Iterator>
class ExprRule : qi::grammar<Iterator, Skipper<Iterator>, AST::ExprList()> {
public:
  ExprRule(Iterator &it);

  // Rules with no skip
  qi::rule<Iterator> comment;
  qi::rule<Iterator, AST::Name()> identifier;
  qi::rule<Iterator, AST::Name()> func_name;

  // Operators tokens
  qi::rule<Iterator> OPAREN, CPAREN, OBRACKET, CBRACKET, OBRACE, CBRACE, COLON
    , RAT, COMA, DIV, ARROW, OANGLE, CANGLE, CARTPROD, SLO, VAR, ADD, SUB, PIPE
    , SEMI, V, VMAP, MAP1, MAP2, EMAP, MAPB, MAPD, X, Y;

  // Other rules
  qi::rule<Iterator, Skipper<Iterator>, LIB::NAT()> nat;
  qi::rule<Iterator, Skipper<Iterator>, AST::Rational> rational_legacy;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> primary;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> factor;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> term;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> arithmetic_expr;

  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> nat_primary;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> nat_factor;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> nat_term;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> nat_expr;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> interval;

  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> md_inter;

  qi::rule<Iterator, Skipper<Iterator>, AST::ExprList()> mdi_list;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> set;

  qi::rule<Iterator, Skipper<Iterator>, AST::ExprList()> lexp_list;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> mdlexp;

  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> map;

  qi::rule<Iterator, Skipper<Iterator>, AST::ExprList()> map_list;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> pwl;

  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> sbg_primary;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> sbg_factor;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> sbg_term;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> sbg_expr;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> sbg_relation;

  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> sbg;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> bipartite_sbg;
  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> dsbg;

  qi::rule<Iterator, Skipper<Iterator>, AST::ExprList()> arg_list;
  qi::rule<Iterator, Skipper<Iterator>, AST::ExprList()> function_call_arg;
  qi::rule<Iterator, Skipper<Iterator>, AST::Call()> call_expr;

  qi::rule<Iterator, Skipper<Iterator>, AST::Expr()> expr;
  qi::rule<Iterator, Skipper<Iterator>, AST::ExprList()> expr_list;

  Iterator &it;
};

} // namespace Parser

} // namespace SBG

#endif // SBGRAPH_PARSER_EXPR_HPP_
