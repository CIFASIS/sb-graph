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

BOOST_FUSION_ADAPT_STRUCT(Parser::BinOp, (Parser::Expr, left_)(Parser::Op, op_)(Parser::Expr, right_))

BOOST_FUSION_ADAPT_STRUCT(Parser::Call, (Parser::Name, name_)(Parser::ExprList, args_))

BOOST_FUSION_ADAPT_STRUCT(Parser::Interval, (Parser::Expr, begin_)(Parser::Expr, step_)(Parser::Expr, end_))

// Expression parser -----------------------------------------------------------

namespace Parser {

struct add_symbols_struct : qi::symbols<char, Parser::Op> {
  add_symbols_struct(){
    add("+", Parser::Op::add)("-", Parser::Op::sub);
  }
} add_symbols;

struct mult_symbol_struct : qi::symbols<char, Parser::Op> {
  mult_symbol_struct(){
    add("*", Parser::Op::mult);
  }
} mult_symbol;

struct expo_symbol_struct : qi::symbols<char, Parser::Op> {
  expo_symbol_struct() {
    add("^", Parser::Op::expo);
  }
} expo_symbol;

struct cap_symbol_struct : qi::symbols<char, Parser::InterOp> {
  cap_symbol_struct(){
    add("/\\", Parser::InterOp::cap);
  }
} cap_symbol;

template <typename Iterator>
ExprRule<Iterator>::ExprRule(Iterator &it) : 
  ExprRule::base_type(exprs_comments), 
  it(it), 
  OPAREN("("), 
  CPAREN(")"), 
  RAT("r"), 
  COMA(","), 
  TRUE("true"), 
  FALSE("false") 
{
  ident = qi::lexeme[(qi::char_('_') | qi::alpha) >> *(qi::alnum | qi::char_('_'))] 
    | qi::lexeme[qi::char_('\'') >> *(qi::alnum | qi::char_('_')) > qi::char_('\'')];

  boolean = TRUE[qi::_val = true] | FALSE[qi::_val = false];

  rational = (RAT >> OPAREN >> qi::lexeme[qi::int_] >> COMA >> qi::char_(' ') >> qi::lexeme[qi::int_] >> CPAREN)[qi::_val = phx::construct<SBG::RATIONAL>(qi::_1, qi::_3)];

  call_exp = (ident >> function_call_args)[qi::_val = phx::construct<Call>(qi::_1, qi::_2)];

  function_call_args = OPAREN 
    >> expr[phx::push_back(qi::_val, qi::_1)] >> *(COMA >> expr)[phx::push_back(qi::_val, qi::_1)] 
    >> CPAREN;
 
  primary = qi::lexeme[qi::int_] | rational | boolean | call_exp | ident;

  factor = primary[qi::_val = qi::_1] >> -(expo_symbol > primary)[qi::_val = phx::construct<BinOp>(qi::_val, qi::_1, qi::_2)];

  term = factor[qi::_val = qi::_1] >> *(mult_symbol >> factor)[qi::_val = phx::construct<BinOp>(qi::_val, qi::_1, qi::_2)];

  arithmetic_expr = term[qi::_val = qi::_1] >> *(add_symbols > term)[qi::_val = phx::construct<BinOp>(qi::_val, qi::_1, qi::_2)];

  interval = (qi::char_('[') 
    >> arithmetic_expr >> qi::char_(':') 
    >> arithmetic_expr >> qi::char_(':') 
    >> arithmetic_expr >> qi::char_(']'))[qi::_val = phx::construct<Interval>(qi::_2, qi::_4, qi::_6)]; 

  interval_expr = interval[qi::_val = qi::_1] >> *(cap_symbol > interval)[qi::_val = phx::construct<InterBinOp>(qi::_val, qi::_1, qi::_2)];

  expr = arithmetic_expr | interval_expr;

  comment = qi::char_('/') >> qi::char_('/') >> *(qi::char_ - qi::char_('\n')) >> qi::char_('\n');

  exprs_comments = *(comment | expr);
};

template struct ExprRule<str_it>;

} // namespace Parser
