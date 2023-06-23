/** @file eval_expr.hpp

 @brief <b>Expression evaluator</b>

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

#ifndef AST_VISITOR_EVALEXP
#define AST_VISITOR_EVALEXP

#include <boost/variant/static_visitor.hpp>

#include <parser/ast/expr.hpp>
#include <sbg/interval.hpp>
#include <util/table.hpp>

namespace Parser {

typedef boost::variant<SBG::INT, SBG::Rational, SBG::Interval> ExprType;

class EvalExpression : public boost::static_visitor<ExprType> {
  member_class(const VarSymbolTable, env);

  EvalExpression();
  EvalExpression(const VarSymbolTable env);

  ExprType operator()(Name v) const;
  SBG::INT operator()(Integer v) const;
  SBG::Rational operator()(Rational v) const;
  SBG::INT operator()(Boolean v) const;
  SBG::Rational operator()(BinOp v) const;
  SBG::Interval operator()(Interval i) const;
  SBG::Interval operator()(InterBinOp i) const;
};

} // namespace Parser

#endif
