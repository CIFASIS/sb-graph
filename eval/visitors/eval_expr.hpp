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

#include "eval/visitors/eval_base_pwmap.hpp"
#include "eval/visitors/eval_canon_pwmap.hpp"
#include "eval/visitors/eval_natbt.hpp"
#include "eval/visitors/eval_container.hpp"
#include "eval/visitors/eval_linear.hpp"
#include "eval/visitors/eval_map.hpp"

namespace SBG {

namespace Eval {

struct EvalExpression : public boost::static_visitor<ExprBaseType> {
  public:
  EvalExpression();
  EvalExpression(VarEnv env);
  EvalExpression(unsigned int nmbr_dims, VarEnv env);

  ExprBaseType operator()(AST::Natural v) const;
  ExprBaseType operator()(AST::MDNatural v) const;
  ExprBaseType operator()(AST::Rational v) const;
  ExprBaseType operator()(AST::Boolean v) const;
  ExprBaseType operator()(Util::VariableName v) const;
  ExprBaseType operator()(AST::UnaryOp v) const;
  ExprBaseType operator()(AST::BinOp v) const;
  ExprBaseType operator()(AST::Call v) const;
  ExprBaseType operator()(AST::Interval v) const;
  ExprBaseType operator()(AST::InterUnaryOp v) const;
  ExprBaseType operator()(AST::InterBinOp v) const;
  ExprBaseType operator()(AST::MultiDimInter v) const;
  ExprBaseType operator()(AST::MDInterUnaryOp v) const;
  ExprBaseType operator()(AST::MDInterBinOp v) const;
  ExprBaseType operator()(AST::Set v) const;
  ExprBaseType operator()(AST::SetUnaryOp v) const;
  ExprBaseType operator()(AST::SetBinOp v) const;
  ExprBaseType operator()(AST::LinearExp v) const;
  ExprBaseType operator()(AST::LExpBinOp v) const;
  ExprBaseType operator()(AST::MDLExp v) const;
  ExprBaseType operator()(AST::MDLExpBinOp v) const;
  ExprBaseType operator()(AST::LinearMap v) const;
  ExprBaseType operator()(AST::PWLMap v) const;

  private:
  unsigned int nmbr_dims_;
  mutable VarEnv env_;
  const FuncEnv fenv_;
};

} // namespace Eval

} // namespace SBG

#endif
