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

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::BinOp, (SBG::AST::Expr, left_)(SBG::AST::Op, op_)(SBG::AST::Expr, right_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::Call, (SBG::AST::Name, name_)(SBG::AST::ExprList, args_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::Interval, (SBG::AST::Expr, begin_)(SBG::AST::Expr, step_)(SBG::AST::Expr, end_))

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

struct complement_symbol_struct : qi::symbols<char, AST::InterUOp> {
  complement_symbol_struct(){
    add("-", AST::InterUOp::comp);
  }
} complement_symbol;

struct card_symbol_struct : qi::symbols<char, AST::InterUOp> {
  card_symbol_struct(){
    add("#", AST::InterUOp::card);
  }
} card_symbol;

struct cap_symbol_struct : qi::symbols<char, AST::InterOp> {
  cap_symbol_struct(){
    add("/\\", AST::InterOp::cap);
  }
} cap_symbol;

struct diff_symbol_struct : qi::symbols<char, AST::InterOp> {
  diff_symbol_struct(){
    add("\\", AST::InterOp::diff);
  }
} diff_symbol;

template <typename Iterator>
ExprRule<Iterator>::ExprRule(Iterator &it) : 
  ExprRule::base_type(exprs_comments), 
  it(it), 
  OPAREN("("), 
  CPAREN(")"), 
  OBRACKET("["), 
  CBRACKET("]"), 
  COLON(":"),
  RAT("r"), 
  COMA(","), 
  TRUE("true"), 
  FALSE("false") 
{
  ident = qi::lexeme[(qi::char_('_') | qi::alpha) >> *(qi::alnum | qi::char_('_'))] 
    | qi::lexeme[qi::char_('\'') >> *(qi::alnum | qi::char_('_')) > qi::char_('\'')];

  boolean = TRUE[qi::_val = true] | FALSE[qi::_val = false];

  rational = (RAT >> OPAREN >> qi::lexeme[qi::int_] >> COMA >> qi::lexeme[qi::int_] >> CPAREN)[qi::_val = phx::construct<Util::RATIONAL>(qi::_1, qi::_2)];

  call_exp = (ident >> function_call_args)[qi::_val = phx::construct<AST::Call>(qi::_1, qi::_2)];

  function_call_args = OPAREN 
    >> expr[phx::push_back(qi::_val, qi::_1)] >> *(COMA >> expr)[phx::push_back(qi::_val, qi::_1)] 
    >> CPAREN;

  primary = rational[qi::_val = phx::construct<Util::RATIONAL>(qi::_1)] 
    | qi::lexeme[qi::ulong_long][qi::_val = phx::construct<Util::INT>(qi::_1)]
    | boolean[qi::_val = phx::construct<AST::Boolean>(qi::_1)] 
    | call_exp[qi::_val = phx::construct<AST::Call>(qi::_1)]
    | ident[qi::_val = phx::construct<Util::VariableName>(qi::_1)];

  factor = primary[qi::_val = qi::_1] >> -(expo_symbol > primary)[qi::_val = phx::construct<AST::BinOp>(qi::_val, qi::_1, qi::_2)];

  term = factor[qi::_val = qi::_1] >> *(mult_symbol >> factor)[qi::_val = phx::construct<AST::BinOp>(qi::_val, qi::_1, qi::_2)];

  arithmetic_expr = term[qi::_val = qi::_1] >> *(add_symbols > term)[qi::_val = phx::construct<AST::BinOp>(qi::_val, qi::_1, qi::_2)];

  interval = (OBRACKET 
    >> arithmetic_expr >> COLON 
    >> arithmetic_expr >> COLON 
    >> arithmetic_expr >> CBRACKET)[qi::_val = phx::construct<AST::Interval>(qi::_1, qi::_2, qi::_3)];

  interval_unary = ((complement_symbol | card_symbol) >> interval)[qi::_val = phx::construct<AST::InterUnaryOp>(qi::_1, qi::_2)];

  interval_binary = OPAREN >> interval[qi::_val = qi::_1] 
    >> +((cap_symbol | diff_symbol) >> interval > CPAREN)[qi::_val = phx::construct<AST::InterBinOp>(qi::_val, qi::_1, qi::_2)];

  interval_expr = interval[qi::_val = qi::_1]
    | interval_unary[qi::_val = qi::_1]
    | interval_binary[qi::_val = qi::_1];

  expr = arithmetic_expr | interval_expr;

  exprs_comments = *(comment | expr);
};

template struct ExprRule<StrIt>;

} // namespace Parser

} // namespace SBG
