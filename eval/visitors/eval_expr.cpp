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

#include <eval/visitors/eval_expr.hpp>

namespace SBG {

namespace Eval {

EvalExpression::EvalExpression() : env_() {}
EvalExpression::EvalExpression(VarEnv env) : env_(env) {}

ExprBaseType EvalExpression::operator()(AST::Integer v) const { return v; }

ExprBaseType EvalExpression::operator()(AST::Rational v) const { return v; }

ExprBaseType EvalExpression::operator()(AST::Boolean v) const { return v; }

ExprBaseType EvalExpression::operator()(Util::VariableName v) const {
  Util::Option<ExprBaseType> v_opt = env_[v];
  if (v_opt)
    return *v_opt;

  Util::ERROR("EvalExpression: variable %s not defined", v);
  return 0; 
}

ExprBaseType EvalExpression::operator()(AST::BinOp v) const 
{
  AST::Expr l = v.left(), r = v.right();

  EvalRat eval_rat(env_);    
  Util::RATIONAL l_result = Apply(eval_rat, l), r_result = Apply(eval_rat, r);
  switch (v.op()) {
    case AST::Op::add:
      return l_result + r_result;

    case AST::Op::sub:
      return l_result - r_result;

    case AST::Op::mult:
      return l_result * r_result;

    default:
      Util::ERROR("EvalExpression: BinOp %s not supported.", AST::OpNames[static_cast<int>(v.op())]);
      return 0;
  }

  Util::ERROR("EvalExpression: BinOp %s not supported.", AST::OpNames[static_cast<int>(v.op())]);
  return 0;
}

ExprBaseType EvalExpression::operator()(AST::Call v) const
{
  std::vector<ExprBaseType> eval_args;
  BOOST_FOREACH (AST::Expr a, v.args()) 
    eval_args.push_back(ApplyThis(a));

  std::cout << v.name() << "\n";
  auto venv = FUNC_ENV[v.name()];
  if (venv) { 
    switch (*venv) {
      case 0: // cardinal
        return Apply(CardinalVisitor(), eval_args[0]);

      default:
        Util::ERROR("EvalExpression: function %s not implemented", v.name());
        return 0;
    }
  }

  Util::ERROR("EvalExpression: function %s does not exist", v.name());
  return 0;
}

ExprBaseType EvalExpression::operator()(AST::Interval i) const
{
  AST::Expr b = i.begin(), s = i.step(), e = i.end();
  ExprBaseType bt = ApplyThis(b), st = ApplyThis(s), et = ApplyThis(e); 

  return Interval(toInt(bt), toInt(st), toInt(et));
}

ExprBaseType EvalExpression::operator()(AST::InterBinOp i) const
{
  AST::Expr l = i.left(), r = i.right();
  EvalInterval eval_interval(env_);
  switch (i.op()){
    case AST::InterOp::cap:
      return intersection(Apply(eval_interval, l), Apply(eval_interval, r));

    default:
      Util::ERROR("EvalExpression: InterBinOp %s not supported.", AST::InterOpNames[static_cast<int>(i.op())]);
      return 0;
  }
}

} // namespace Eval

} // namespace SBG
