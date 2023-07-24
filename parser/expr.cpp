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

#include <parser/expr.hpp>

// Adapt structures ------------------------------------------------------------

BOOST_FUSION_ADAPT_STRUCT(SBG::Util::RATIONAL, (boost::rational<SBG::Util::INT>, value_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::BinOp, (SBG::AST::Expr, left_)(SBG::AST::Op, op_)(SBG::AST::Expr, right_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::Call, (SBG::AST::Name, name_)(SBG::AST::ExprList, args_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::Interval, (SBG::AST::Expr, begin_)(SBG::AST::Expr, step_)(SBG::AST::Expr, end_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::InterUnaryOp, (SBG::AST::ContainerUOp, op_)(SBG::AST::Expr, e_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::InterBinOp, (SBG::AST::Expr, left_)(SBG::AST::ContainerOp, op_)(SBG::AST::Expr, right_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::Set, (SBG::AST::ExprList, pieces_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::SetUnaryOp, (SBG::AST::ContainerUOp, op_)(SBG::AST::Expr, e_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::SetBinOp, (SBG::AST::Expr, left_)(SBG::AST::ContainerOp, op_)(SBG::AST::Expr, right_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::LinearExp, (SBG::AST::Expr, slope_)(SBG::AST::Expr, offset_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::LExpBinOp, (SBG::AST::Expr, left_)(SBG::AST::Op, op_)(SBG::AST::Expr, right_))

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

struct lexp_bin_struct : qi::symbols<char, AST::Op> {
  lexp_bin_struct(){
    add("+", AST::Op::add)("-", AST::Op::sub);
  }
} lexp_bin;

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
  FALSE("false") 
{
  ident = qi::lexeme[(qi::char_('_') | qi::alpha) >> *(qi::alnum | qi::char_('_'))] 
    | qi::lexeme[qi::char_('\'') >> *(qi::alnum | qi::char_('_')) > qi::char_('\'')];

  boolean = TRUE[qi::_val = true] | FALSE[qi::_val = false];

  rational = (RAT >> OPAREN >> qi::lexeme[qi::long_long] 
    >> COMA >> qi::lexeme[qi::long_long] >> CPAREN)[qi::_val = phx::construct<Util::RATIONAL>(qi::_1, qi::_2)];

  call_exp = (ident >> function_call_args)[qi::_val = phx::construct<AST::Call>(qi::_1, qi::_2)];

  function_call_args = OPAREN >> expr_list >> CPAREN;

  primary = rational[qi::_val = qi::_1] 
    | qi::lexeme[qi::ulong_long][qi::_val = phx::construct<Util::NAT>(qi::_1)]
    | boolean[qi::_val = qi::_1] 
    | call_exp[qi::_val = qi::_1]
    | ident[qi::_val = qi::_1];

  factor = primary[qi::_val = qi::_1] >> -(expo_symbol > primary)[qi::_val = phx::construct<AST::BinOp>(qi::_val, qi::_1, qi::_2)];

  term = factor[qi::_val = qi::_1] >> *(mult_symbol >> factor)[qi::_val = phx::construct<AST::BinOp>(qi::_val, qi::_1, qi::_2)];

  arithmetic_expr = term[qi::_val = qi::_1] >> *(add_symbols > term)[qi::_val = phx::construct<AST::BinOp>(qi::_val, qi::_1, qi::_2)];

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

  set = (OBRACE >> inter_list >> CBRACE)[qi::_val = phx::construct<AST::Set>(qi::_1)]
    | (OBRACE >> CBRACE)[qi::_val = phx::construct<AST::Set>()]
    | ident[qi::_val = phx::construct<Util::VariableName>(qi::_1)];

  set_unary = (set_un >> set_expr)[qi::_val = phx::construct<AST::SetUnaryOp>(qi::_1, qi::_2)]; 
 
  set_binary = (OPAREN >> set
    >> set_bin >> set_expr 
    > CPAREN)[qi::_val = phx::construct<AST::SetBinOp>(qi::_1, qi::_2, qi::_3)];
  
  set_expr = set_unary[qi::_val = qi::_1]
    | set_binary[qi::_val = qi::_1]
    | set[qi::_val = qi::_1];

  numeric = rational[qi::_val = qi::_1] | qi::lexeme[qi::ulong_long][qi::_val = phx::construct<Util::NAT>(qi::_1)];

  lexp = (numeric >> qi::char_('*') >> qi::char_('x') 
    >> qi::char_('+') >> arithmetic_expr)[qi::_val = phx::construct<AST::LinearExp>(qi::_1, qi::_5)];

  lexp_binary = OPAREN >> lexp[qi::_val = qi::_1] >> CPAREN 
    >> *(lexp_bin > OPAREN >> lexp >> CPAREN)[qi::_val = phx::construct<AST::LExpBinOp>(qi::_val, qi::_1, qi::_2)];

  lexp_expr = lexp[qi::_val = qi::_1]
    | lexp_binary[qi::_val = qi::_1];

  expr = lexp_expr | arithmetic_expr | interval_expr | set_expr;
  
  expr_list = expr[phx::push_back(qi::_val, qi::_1)] >> *(COMA >> expr)[phx::push_back(qi::_val, qi::_1)];

  exprs_comments = *(comment | expr);
};

template struct ExprRule<StrIt>;

} // namespace Parser

} // namespace SBG
