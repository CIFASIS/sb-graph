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

ExprBaseType EvalExpression::operator()(Util::VariableName v) const 
{
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
      Util::ERROR("EvalExpression: BinOp %s not supported.", AST::OpNames[v.op()]);
      return 0;
  }

  Util::ERROR("EvalExpression: BinOp %s not supported.", AST::OpNames[v.op()]);
  return 0;
}

ExprBaseType EvalExpression::operator()(AST::Call v) const
{
  auto venv = FUNC_ENV[v.name()];
  if (venv) { 
    std::vector<ExprBaseType> eval_args;
    BOOST_FOREACH (AST::Expr a, v.args()) 
      eval_args.push_back(ApplyThis(a));

    switch (*venv) {
      //case Eval::Func::empty: 
      //  return Apply(EmptyVisitor(), eval_args[0]);

      case Eval::Func::member: { 
        ExprBaseType x = eval_args[0], i = eval_args[1];
        bool result = std::visit(Overload{
          [](Util::INT a, SBG::Interval b) { return isMember(a, b); },
          [](auto a, auto b) { return false; }, // << default!
        }, x, i);
        return result;
      }

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

ExprBaseType EvalExpression::operator()(AST::InterUnaryOp i) const
{
  AST::Expr exp = i.e();
  EvalInterval eval_interval(env_);
  switch (i.op()){
    case AST::InterUOp::card:
      return cardinal(Apply(eval_interval, exp));

    case AST::InterUOp::comp:
      return complement(Apply(eval_interval, exp));

    default:
      Util::ERROR("EvalExpression: InterUnaryOp %s not supported.", AST::InterUOpNames[i.op()]);
      return 0;
  }
}

ExprBaseType EvalExpression::operator()(AST::InterBinOp i) const
{
  AST::Expr l = i.left(), r = i.right();
  EvalInterval eval_interval(env_);
  switch (i.op()){
    case AST::InterOp::cap:
      return intersection(Apply(eval_interval, l), Apply(eval_interval, r));

    default:
      Util::ERROR("EvalExpression: InterBinOp %s not supported.", AST::InterOpNames[i.op()]);
      return 0;
  }
}

} // namespace Eval

} // namespace SBG
