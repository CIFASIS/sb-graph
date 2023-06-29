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

#include <boost/variant/get.hpp>

#include <eval/visitors/eval_rat.hpp>
#include <eval/visitors/eval_interval.hpp>
#include <eval/visitors/operation_visitor.hpp>

namespace SBG {

namespace Eval {

class EvalExpression : public boost::static_visitor<ExprBaseType> {
  public:
  EvalExpression();
  EvalExpression(VarEnv env);

  ExprBaseType operator()(AST::Integer v) const;
  ExprBaseType operator()(AST::Rational v) const;
  ExprBaseType operator()(AST::Boolean v) const;
  ExprBaseType operator()(Util::VariableName v) const;
  ExprBaseType operator()(AST::BinOp v) const;
  ExprBaseType operator()(AST::Call v) const;
  ExprBaseType operator()(AST::Interval i) const;
  ExprBaseType operator()(AST::InterBinOp i) const;

  private:
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
