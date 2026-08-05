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

#ifndef SBGRAPH_PARSER_EXPR_DEF_HPP_
#define SBGRAPH_PARSER_EXPR_DEF_HPP_

#include "ast/expr.hpp"
#include "sbg/rational.hpp"

#include <boost/phoenix/core.hpp>
#include <boost/phoenix/operator.hpp>
#include <boost/phoenix/object.hpp>
#include <boost/spirit/include/qi.hpp>

// Adapt structures ------------------------------------------------------------

BOOST_FUSION_ADAPT_STRUCT(SBG::LIB::MD_NAT, (SBG::LIB::MD_NAT::VNAT, value_))

BOOST_FUSION_ADAPT_STRUCT(
  SBG::LIB::RATIONAL, (boost::rational<SBG::LIB::INT>, value_)
)

BOOST_FUSION_ADAPT_STRUCT(
  SBG::AST::UnaryOp, (SBG::AST::UnOp, op_)(SBG::AST::Expr, expr_)
)

BOOST_FUSION_ADAPT_STRUCT(
  SBG::AST::BinOp
  , (SBG::AST::Expr, left_)(SBG::AST::Op, op_)(SBG::AST::Expr, right_)
)

BOOST_FUSION_ADAPT_STRUCT(
  SBG::AST::Call, (SBG::AST::Name, name_)(SBG::AST::ExprList, args_)
)

BOOST_FUSION_ADAPT_STRUCT(
  SBG::AST::Interval
  , (SBG::AST::Expr, begin_)(SBG::AST::Expr, step_)(SBG::AST::Expr, end_)
)

BOOST_FUSION_ADAPT_STRUCT(
  SBG::AST::MultiDimInter, (SBG::AST::ExprList, intervals_)
)

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::Set, (SBG::AST::ExprList, pieces_))

BOOST_FUSION_ADAPT_STRUCT(
  SBG::AST::LinearExp, (SBG::AST::Expr, slope_)(SBG::AST::Expr, offset_)
)

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::MDLExp, (SBG::AST::ExprList, exps_))

BOOST_FUSION_ADAPT_STRUCT(
  SBG::AST::LinearMap, (SBG::AST::Expr, dom_)(SBG::AST::Expr, lexp_)
)

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::PWLMap, (SBG::AST::ExprList, maps_))

BOOST_FUSION_ADAPT_STRUCT(
  SBG::AST::SBG
  , (SBG::AST::Expr, _V)(SBG::AST::Expr, _Vmap)
    (SBG::AST::Expr, _map1)(SBG::AST::Expr, _map2)(SBG::AST::Expr, _Emap)
)

BOOST_FUSION_ADAPT_STRUCT(
  SBG::AST::BipartiteSBG
  , (SBG::AST::Expr, _V)(SBG::AST::Expr, _Vmap)
    (SBG::AST::Expr, _map1)(SBG::AST::Expr, _map2)(SBG::AST::Expr, _Emap)
    (SBG::AST::Expr, _X)(SBG::AST::Expr, _Y)
)

BOOST_FUSION_ADAPT_STRUCT(
  SBG::AST::DSBG
  , (SBG::AST::Expr, V_)(SBG::AST::Expr, Vmap_)
    (SBG::AST::Expr, mapB_)(SBG::AST::Expr, mapD_)(SBG::AST::Expr, Emap_)
)

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::ParenExpr, (SBG::AST::Expr, e_))

namespace SBG {

namespace Parser {

namespace phx = boost::phoenix;

// Operators symbols -----------------------------------------------------------

struct add_symbols_struct : qi::symbols<char, AST::Op> {
  add_symbols_struct(){
    add("+", AST::Op::add)("-", AST::Op::sub);
  }
} add_symbols;

struct mult_symbol_struct : qi::symbols<char, AST::Op> {
  mult_symbol_struct() {
    add("*", AST::Op::mult);
  }
} mult_symbol;

struct unary_symbol_struct : qi::symbols<char, AST::UnOp> {
  unary_symbol_struct() {
    add("-", AST::UnOp::oppo)
       ("#", AST::UnOp::card);
  }
} unary_symbol;

struct binary_symbol_struct : qi::symbols<char, AST::Op> {
  binary_symbol_struct() {
    add("+", AST::Op::add)
       ("-", AST::Op::sub)
       ("/\\", AST::Op::cap)
       ("\\/", AST::Op::cup)
       ("\\", AST::Op::diff);
  }
} binary_symbol;

struct rel_symbol_struct : qi::symbols<char, AST::Op> {
  rel_symbol_struct() {
    add("==", AST::Op::eq)
       ("<", AST::Op::less);
  }
} rel_symbol;

// Expression parser -----------------------------------------------------------

template <typename Iterator>
ExprRule<Iterator>::ExprRule(Iterator &it) : 
  ExprRule::base_type(expr_list) 
  , it(it) 
  , OPAREN("(") 
  , CPAREN(")") 
  , OBRACKET("[") 
  , CBRACKET("]") 
  , OBRACE("{")
  , CBRACE("}")
  , COLON(":")
  , RAT("r") 
  , COMA(",") 
  , DIV("/")
  , ARROW("->")
  , OANGLE("<<")
  , CANGLE(">>")
  , CARTPROD("x")
  , ADD("+")
  , SUB("-")
  , PIPE("|")
  , SEMI(";")
  , V("V:")
  , VMAP("Vmap:")
  , MAP1("map1:")
  , MAP2("map2:")
  , EMAP("Emap:")
  , MAPB("mapB:")
  , MAPD("mapD:")
  , X("X:")
  , Y("Y:")
{
  identifier = qi::lexeme[qi::char_("a-zA-Z")
    >> *(qi::alnum | qi::char_('_'))];

  nat = qi::lexeme[qi::ulong_long][qi::_val = phx::construct<LIB::NAT>(qi::_1)];

  rational_legacy = (RAT 
    >> OPAREN 
    >> arithmetic_expr
    >> COMA
    >> arithmetic_expr
    >> CPAREN)[qi::_val = phx::construct<AST::Rational>(qi::_1, qi::_2)];

  primary = rational_legacy[qi::_val = qi::_1]
    | nat[qi::_val = qi::_1]
    | identifier[qi::_val = qi::_1]
    | (OPAREN >> arithmetic_expr >> CPAREN)
      [qi::_val = phx::construct<AST::ParenExpr>(qi::_1)];

  factor = (unary_symbol >> primary)
    [qi::_val = phx::construct<AST::UnaryOp>(qi::_1, qi::_2)]
    | primary[qi::_val = qi::_1];

  term = factor[qi::_val = qi::_1]
    >> *((mult_symbol >> factor)
      [qi::_val = phx::construct<AST::BinOp>(qi::_val, qi::_1, qi::_2)]
      | (DIV >> factor)
      [qi::_val = phx::construct<AST::Rational>(qi::_val, qi::_1)]);

  arithmetic_expr = term[qi::_val = qi::_1] >> *(add_symbols >> term)
    [qi::_val = phx::construct<AST::BinOp>(qi::_val, qi::_1, qi::_2)];

  // ------------ //

  interval = (OBRACKET 
    >> arithmetic_expr >> COLON 
    >> ((arithmetic_expr >> COLON) | qi::attr(phx::construct<AST::Expr>(1))) 
    >> arithmetic_expr >> CBRACKET)
    [qi::_val = phx::construct<AST::Interval>(qi::_1, qi::_2, qi::_3)];

  // ------------ //

  md_inter = (interval % CARTPROD)
    [qi::_val = phx::construct<AST::MultiDimInter>(qi::_1)];

  // ------------ //

  mdi_list = md_inter % COMA;

  set = (OBRACE >> mdi_list >> CBRACE)
      [qi::_val = phx::construct<AST::Set>(qi::_1)]
    | (OBRACE >> CBRACE)[qi::_val = phx::construct<AST::Set>()];

  // ------------ //

  lexp_list = arithmetic_expr % PIPE;

  mdlexp = (PIPE >> lexp_list >> PIPE)
    [qi::_val = phx::construct<AST::MDLExp>(qi::_1)];

  // ------------ //

  map = (set >> ARROW >> mdlexp)
    [qi::_val = phx::construct<AST::LinearMap>(qi::_1, qi::_2)];

  // ------------ //

  map_list = map % COMA;

  pwl = (OANGLE >> map_list >> CANGLE)
      [qi::_val = phx::construct<AST::PWLMap>(qi::_1)]
    | (OANGLE >> CANGLE)[qi::_val = phx::construct<AST::PWLMap>()];

  // ------------ //

  sbg_primary = call_expr
    | pwl
    | map
    | mdlexp
    | set
    | md_inter
    | arithmetic_expr
    | (OPAREN >> sbg_expr >> CPAREN);

  sbg_factor = (unary_symbol >> sbg_primary)
    [qi::_val = phx::construct<AST::UnaryOp>(qi::_1, qi::_2)]
    | sbg_primary[qi::_val = qi::_1];

  sbg_expr = sbg_factor[qi::_val = qi::_1] >> *(binary_symbol >> sbg_factor)
    [qi::_val = phx::construct<AST::BinOp>(qi::_val, qi::_1, qi::_2)];

  sbg_relation = sbg_expr[qi::_val = qi::_1] >> -(rel_symbol >> sbg_expr)
    [qi::_val = phx::construct<AST::BinOp>(qi::_val, qi::_1, qi::_2)];

  // ------------ //

  sbg = (V >> sbg_expr 
    >> VMAP >> sbg_expr
    >> MAP1 >> sbg_expr
    >> MAP2 >> sbg_expr
    >> EMAP >> sbg_expr)[qi::_val
      = phx::construct<AST::SBG>(qi::_1, qi::_2, qi::_3, qi::_4, qi::_5)];

  // ------------ //

  bipartite_sbg = (V >> sbg_expr 
    >> VMAP >> sbg_expr 
    >> MAP1 >> sbg_expr 
    >> MAP2 >> sbg_expr 
    >> EMAP >> sbg_expr
    >> X >> sbg_expr
    >> Y >> sbg_expr)[qi::_val
      = phx::construct<AST::BipartiteSBG>(qi::_1, qi::_2, qi::_3, qi::_4, qi::_5
        , qi::_6, qi::_7)];

  // ------------ //

  dsbg = (V >> sbg_expr 
    >> VMAP >> sbg_expr 
    >> MAPB >> sbg_expr 
    >> MAPD >> sbg_expr 
    >> EMAP >> sbg_expr)[qi::_val
      = phx::construct<AST::DSBG>(qi::_1, qi::_2, qi::_3, qi::_4, qi::_5)];

  // ------------ //

  arg_list = expr % COMA;

  function_call_arg = OPAREN >> arg_list >> CPAREN;

  // Take out "r" as function name, to preserve it for legacy rationals
  func_name = (qi::lexeme[qi::char_("r") >> +(qi::alnum | qi::char_('_'))])
    | (qi::lexeme[qi::char_("a-qs-zA-QS-Z")
        >> *(qi::alnum | qi::char_('_'))]);

  call_expr = (func_name >> function_call_arg)
    [qi::_val = phx::construct<AST::Call>(qi::_1, qi::_2)]; 

  // ------------ //

  expr = bipartite_sbg
    | sbg
    | dsbg
    | sbg_relation;

  expr_list = expr % COMA;
};

} // namespace Parser

} // namespace SBG

#endif // SBGRAPH_PARSER_EXPR_DEF_HPP_
