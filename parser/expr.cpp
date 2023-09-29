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

#include "parser/expr.hpp"

// Adapt structures ------------------------------------------------------------

BOOST_FUSION_ADAPT_STRUCT(SBG::Util::MD_NAT, (SBG::Util::VNAT, value_))

BOOST_FUSION_ADAPT_STRUCT(SBG::Util::RATIONAL, (boost::rational<SBG::Util::INT>, value_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::UnaryOp, (SBG::AST::UnOp, op_)(SBG::AST::Expr, expr_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::BinOp, (SBG::AST::Expr, left_)(SBG::AST::Op, op_)(SBG::AST::Expr, right_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::Call, (SBG::AST::Name, name_)(SBG::AST::ExprList, args_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::Interval, (SBG::AST::Expr, begin_)(SBG::AST::Expr, step_)(SBG::AST::Expr, end_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::InterUnaryOp, (SBG::AST::ContainerUOp, op_)(SBG::AST::Expr, e_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::InterBinOp, (SBG::AST::Expr, left_)(SBG::AST::ContainerOp, op_)(SBG::AST::Expr, right_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::MultiDimInter, (SBG::AST::ExprList, intervals_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::MDInterUnaryOp, (SBG::AST::ContainerUOp, op_)(SBG::AST::Expr, e_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::MDInterBinOp, (SBG::AST::Expr, left_)(SBG::AST::ContainerOp, op_)(SBG::AST::Expr, right_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::Set, (SBG::AST::ExprList, pieces_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::SetUnaryOp, (SBG::AST::ContainerUOp, op_)(SBG::AST::Expr, e_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::SetBinOp, (SBG::AST::Expr, left_)(SBG::AST::ContainerOp, op_)(SBG::AST::Expr, right_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::LinearExp, (SBG::AST::Expr, slope_)(SBG::AST::Expr, offset_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::LExpBinOp, (SBG::AST::Expr, left_)(SBG::AST::ExpOp, op_)(SBG::AST::Expr, right_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::MDLExp, (SBG::AST::ExprList, exps_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::MDLExpBinOp, (SBG::AST::Expr, left_)(SBG::AST::ExpOp, op_)(SBG::AST::Expr, right_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::LinearMap, (SBG::AST::Expr, dom_)(SBG::AST::Expr, lexp_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::PWLMap, (SBG::AST::ExprList, maps_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::SBG, (SBG::AST::Expr, V_)(SBG::AST::Expr, Vmap_)(SBG::AST::Expr, map1_)(SBG::AST::Expr, map2_)(SBG::AST::Expr, Emap_))

// Expression parser -----------------------------------------------------------

namespace SBG {

namespace Parser {

struct add_symbols_struct : qi::symbols<char, AST::Op> {
  add_symbols_struct(){
    add("+", AST::Op::add)("-", AST::Op::sub);
  }
} add_symbols;

struct mult_symbol_struct : qi::symbols<char, AST::Op> {
  mult_symbol_struct(){
    add("*", AST::Op::mult);
  }
} mult_symbol;

struct expo_symbol_struct : qi::symbols<char, AST::Op> {
  expo_symbol_struct() {
    add("^", AST::Op::expo);
  }
} expo_symbol;

struct unary_symbol_struct : qi::symbols<char, AST::UnOp> {
  unary_symbol_struct() {
    add("-", AST::UnOp::neg);
  }
} unary_symbol;

// ------------ //

struct inter_unary_struct : qi::symbols<char, AST::ContainerUOp> {
  inter_unary_struct(){
    add("#", AST::ContainerUOp::card);
  }
} inter_un;

struct inter_bin_struct : qi::symbols<char, AST::ContainerOp> {
  inter_bin_struct(){
    add("/\\", AST::ContainerOp::cap)("<", AST::ContainerOp::less)("==", AST::ContainerOp::eq);
  }
} inter_bin;

// ------------ //

struct mdi_unary_struct : qi::symbols<char, AST::ContainerUOp> {
  mdi_unary_struct(){
    add("#", AST::ContainerUOp::card);
  }
} mdi_un;

struct mdi_bin_struct : qi::symbols<char, AST::ContainerOp> {
  mdi_bin_struct(){
    add("/\\", AST::ContainerOp::cap)("==", AST::ContainerOp::eq);
  }
} mdi_bin;

// ------------ //

struct set_unary_struct : qi::symbols<char, AST::ContainerUOp> {
  set_unary_struct(){
    add("#", AST::ContainerUOp::card)("-", AST::ContainerUOp::comp);
  }
} set_un;

struct set_bin_struct : qi::symbols<char, AST::ContainerOp> {
  set_bin_struct(){
    add("/\\", AST::ContainerOp::cap)("\\", AST::ContainerOp::diff)("==", AST::ContainerOp::eq)("\\/", AST::ContainerOp::cup);
  }
} set_bin;

// ------------ //

struct lexp_bin_struct : qi::symbols<char, AST::ExpOp> {
  lexp_bin_struct(){
    add("==", AST::ExpOp::eq)("+", AST::ExpOp::add)("-", AST::ExpOp::sub);
  }
} lexp_bin;

// ------------ //

struct mdlexp_bin_struct : qi::symbols<char, AST::ExpOp> {
  mdlexp_bin_struct(){
    add("==", AST::ExpOp::eq)("+", AST::ExpOp::add)("-", AST::ExpOp::sub);
  }
} mdlexp_bin;

template <typename Iterator>
ExprRule<Iterator>::ExprRule(Iterator &it) : 
  ExprRule::base_type(exprs_comments), 
  it(it), 
  OPAREN("("), 
  CPAREN(")"), 
  OBRACKET("["), 
  CBRACKET("]"), 
  OBRACE("{"),
  CBRACE("}"),
  COLON(":"),
  RAT("r"), 
  COMA(","), 
  TRUE("true"), 
  FALSE("false"), 
  ARROW("->"),
  OANGLE("<<"),
  CANGLE(">>"),
  DIMS("x"),
  PIPE("|"),
  SEMI(";"),
  V("V %="),
  VMAP("Vmap %="),
  MAP1("map1 %="),
  MAP2("map2 %="),
  EMAP("Emap %=")
{
  ident = qi::lexeme[(qi::char_('_') | qi::alpha) >> *(qi::alnum | qi::char_('_'))] 
    | qi::lexeme[qi::char_('\'') >> *(qi::alnum | qi::char_('_')) > qi::char_('\'')];

  boolean = TRUE[qi::_val = true] | FALSE[qi::_val = false];

  md_nat = OPAREN >> qi::lexeme[qi::ulong_long][phx::push_back(qi::_val, qi::_1)] 
    >> *(COMA >> qi::lexeme[qi::ulong_long])[phx::push_back(qi::_val, qi::_1)] >> CPAREN;

  rational = (RAT >> OPAREN >> qi::lexeme[qi::long_long] 
    >> COMA >> qi::lexeme[qi::long_long] >> CPAREN)[qi::_val = phx::construct<Util::RATIONAL>(qi::_1, qi::_2)];

  call_exp = (ident >> function_call_args)[qi::_val = phx::construct<AST::Call>(qi::_1, qi::_2)];

  function_call_args = OPAREN >> expr_list >> CPAREN;

  primary = rational[qi::_val = qi::_1] 
    | qi::lexeme[qi::ulong_long][qi::_val = phx::construct<Util::NAT>(qi::_1)]
    | md_nat[qi::_val = qi::_1]
    | boolean[qi::_val = qi::_1] 
    | call_exp[qi::_val = qi::_1]
    | ident[qi::_val = qi::_1];

  factor = primary[qi::_val = qi::_1] >> -(expo_symbol > primary)[qi::_val = phx::construct<AST::BinOp>(qi::_val, qi::_1, qi::_2)];

  term = factor[qi::_val = qi::_1] >> *(mult_symbol >> factor)[qi::_val = phx::construct<AST::BinOp>(qi::_val, qi::_1, qi::_2)];

  arithmetic_expr = (term[qi::_val = qi::_1] >> *(add_symbols > term)[qi::_val = phx::construct<AST::BinOp>(qi::_val, qi::_1, qi::_2)])
    | (unary_symbol >> term)[qi::_val = phx::construct<AST::UnaryOp>(qi::_1, qi::_2)];

  // ------------ //

  interval = (OBRACKET 
    >> arithmetic_expr >> COLON 
    >> arithmetic_expr >> COLON 
    >> arithmetic_expr >> CBRACKET)[qi::_val = phx::construct<AST::Interval>(qi::_1, qi::_2, qi::_3)]
    | ident[qi::_val = qi::_1];

  interval_unary = (inter_un >> interval_expr)[qi::_val = phx::construct<AST::InterUnaryOp>(qi::_1, qi::_2)];

  interval_binary = (OPAREN >> interval_expr 
    >> inter_bin >> interval_expr
    > CPAREN)[qi::_val = phx::construct<AST::InterBinOp>(qi::_1, qi::_2, qi::_3)];

  interval_expr = interval_unary[qi::_val = qi::_1]
    | interval_binary[qi::_val = qi::_1]
    | interval[qi::_val = qi::_1];

  inter_list = interval[phx::push_back(qi::_val, qi::_1)] >> *(COMA >> interval)[phx::push_back(qi::_val, qi::_1)];

  // ------------ //

  inter_times = interval[phx::push_back(qi::_val, qi::_1)] >> *(DIMS >> interval)[phx::push_back(qi::_val, qi::_1)];

  md_inter = inter_times[qi::_val = phx::construct<AST::MultiDimInter>(qi::_1)];

  mdi_unary = (mdi_un >> mdi_expr)[qi::_val = phx::construct<AST::MDInterUnaryOp>(qi::_1, qi::_2)];

  mdi_binary = (OPAREN >> mdi_expr 
    >> mdi_bin >> mdi_expr
    > CPAREN)[qi::_val = phx::construct<AST::MDInterBinOp>(qi::_1, qi::_2, qi::_3)];

  mdi_expr = mdi_unary[qi::_val = qi::_1]
    | mdi_binary[qi::_val = qi::_1]
    | md_inter[qi::_val = qi::_1];

  mdi_list = md_inter[phx::push_back(qi::_val, qi::_1)] >> *(COMA >> md_inter)[phx::push_back(qi::_val, qi::_1)];

  // ------------ //

  set = (OBRACE >> mdi_list >> CBRACE)[qi::_val = phx::construct<AST::Set>(qi::_1)]
    | (OBRACE >> CBRACE)[qi::_val = phx::construct<AST::Set>()]
    | ident[qi::_val = qi::_1];

  set_unary = (set_un >> set_expr)[qi::_val = phx::construct<AST::SetUnaryOp>(qi::_1, qi::_2)]; 
 
  set_binary = (OPAREN >> set_expr
    >> set_bin >> set_expr 
    > CPAREN)[qi::_val = phx::construct<AST::SetBinOp>(qi::_1, qi::_2, qi::_3)];
  
  set_expr = set_unary[qi::_val = qi::_1]
    | set_binary[qi::_val = qi::_1]
    | set[qi::_val = qi::_1];

  // ------------ //

  numeric = rational[qi::_val = qi::_1] | qi::lexeme[qi::ulong_long][qi::_val = phx::construct<Util::NAT>(qi::_1)];

  lexp = (numeric >> qi::char_('*') >> qi::char_('x') 
    >> qi::char_('+') >> arithmetic_expr)[qi::_val = phx::construct<AST::LinearExp>(qi::_1, qi::_5)]
    | (numeric >> qi::char_('*') >> qi::char_('x') 
      >> arithmetic_expr)[qi::_val = phx::construct<AST::LinearExp>(qi::_1, qi::_4)];

  lexp_binary = OPAREN >> lexp[qi::_val = qi::_1] >> CPAREN 
    >> *(lexp_bin > OPAREN >> lexp >> CPAREN)[qi::_val = phx::construct<AST::LExpBinOp>(qi::_val, qi::_1, qi::_2)];

  lexp_expr = lexp[qi::_val = qi::_1]
    | lexp_binary[qi::_val = qi::_1];

  // ------------ //

  lexp_pipe = lexp[phx::push_back(qi::_val, qi::_1)] >> *(PIPE >> lexp)[phx::push_back(qi::_val, qi::_1)];

  mdlexp = lexp_pipe[qi::_val = phx::construct<AST::MDLExp>(qi::_1)];

  mdlexp_binary = OPAREN >> mdlexp[qi::_val = qi::_1] >> CPAREN 
    >> *(mdlexp_bin > OPAREN >> mdlexp >> CPAREN)[qi::_val = phx::construct<AST::MDLExpBinOp>(qi::_val, qi::_1, qi::_2)];

  mdlexp_expr = mdlexp[qi::_val = qi::_1]
    | mdlexp_binary[qi::_val = qi::_1];


  // ------------ //

  sbgmap = (set_expr >> ARROW >> mdlexp_expr)[qi::_val = phx::construct<AST::LinearMap>(qi::_1, qi::_2)];

  map_expr = sbgmap;

  map_list = sbgmap[phx::push_back(qi::_val, qi::_1)] >> *(COMA >> sbgmap)[phx::push_back(qi::_val, qi::_1)];

  // ------------ //

  pwl = (OANGLE >> map_list >> CANGLE)[qi::_val = phx::construct<AST::PWLMap>(qi::_1)]
    | (OANGLE >> CANGLE)[qi::_val = phx::construct<AST::PWLMap>()];

  pwl_expr = pwl;

  // ------------ //

  sbg = (V >> set >> SEMI 
     >> VMAP >> pwl >> SEMI 
     >> MAP1 >> pwl >> SEMI 
     >> MAP2 >> pwl >> SEMI 
     >> EMAP >> pwl >> SEMI)[qi::_val = phx::construct<AST::SBG>(qi::_1, qi::_2, qi::_3, qi::_4, qi::_5)];

  // ------------ //

  expr = sbg | pwl_expr | map_expr | mdlexp_expr | lexp_expr | arithmetic_expr | set_expr | mdi_expr | interval_expr;
  
  expr_list = expr[phx::push_back(qi::_val, qi::_1)] >> *(COMA >> expr)[phx::push_back(qi::_val, qi::_1)];

  exprs_comments = *(comment | expr);
};

template struct ExprRule<StrIt>;

} // namespace Parser

} // namespace SBG
