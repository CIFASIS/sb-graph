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

// Function visitors -----------------------------------------------------------

auto empty_visitor_ = Overload{
  [](SBG::Interval a) { return isEmpty(a); },
  [](SBG::Set a) { return isEmpty(a); },
  [](auto a) { return true; } // << default!
};

auto member_visitor_ = Overload{
  [](Util::INT a, SBG::Interval b) { return isMember(a, b); },
  [](Util::INT a, SBG::Set b) { return isMember(a, b); },
  [](auto a, auto b) { return false; } // << default!
};

auto min_visitor_ = Overload{
  [](SBG::Interval a) { return minElem(a); },
  [](SBG::Set a) { return minElem(a); },
  [](auto a) { return (Util::INT) 0; }
};

auto max_visitor_ = Overload{
  [](SBG::Interval a) { return maxElem(a); },
  [](SBG::Set a) { return maxElem(a); },
  [](auto a) { return (Util::INT) 0; }
};

auto lt_visitor_ = Overload{
  [](SBG::Interval a, SBG::Interval b) { return least(a, b); },
  [](auto a, auto b) { return SBG::Interval(1, 0, 0); }
};

// Expression evaluator --------------------------------------------------------

EvalExpression::EvalExpression() : env_() {}
EvalExpression::EvalExpression(VarEnv env) : env_(env) {}

ExprBaseType EvalExpression::operator()(AST::Integer v) const { return v; }

ExprBaseType EvalExpression::operator()(AST::Rational v) const { return v; }

ExprBaseType EvalExpression::operator()(AST::Boolean v) const { return v; }

ExprBaseType EvalExpression::operator()(Util::VariableName v) const 
{
  MaybeEBT v_opt = env_[v];
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
  std::string vname = v.name();

  auto venv = fenv_[vname];
  if (venv) { 
    std::vector<ExprBaseType> eval_args;
    BOOST_FOREACH (AST::Expr a, v.args()) 
      eval_args.push_back(ApplyThis(a));

    switch (*venv) {
      case Eval::Func::empty: {
        ExprBaseType container = eval_args[0];
        bool result = std::visit(empty_visitor_, container);
        return result;
      }

      case Eval::Func::member: { 
        ExprBaseType x = eval_args[0], container = eval_args[1];
        bool result = std::visit(member_visitor_, x, container);
        return result;
      }

      case Eval::Func::min: {
        ExprBaseType container = eval_args[0];
        Util::INT result = std::visit(min_visitor_, container);
        return result;
      }

      case Eval::Func::max: {
        ExprBaseType container = eval_args[0];
        Util::INT result = std::visit(max_visitor_, container);
        return result;
      }

      case Eval::Func::lt: {
        ExprBaseType cont1 = eval_args[0], cont2 = eval_args[1];
        SBG::Interval result = std::visit(lt_visitor_, cont1, cont2);
        return result;
      }

      default:
        Util::ERROR("EvalExpression: function %s not implemented", vname.c_str());
        return 0;
    }
  }

  Util::ERROR("EvalExpression: function %s does not exist", vname.c_str());
  return 0;
}

ExprBaseType EvalExpression::operator()(AST::Interval v) const
{
  EvalInt int_visit(env_);
  Util::INT b = Apply(int_visit, v.begin());
  Util::INT s = Apply(int_visit, v.step());
  Util::INT e = Apply(int_visit, v.end());

  return Interval(b, s, e);
}

ExprBaseType EvalExpression::operator()(AST::InterUnaryOp v) const
{
  AST::Expr exp = v.e();
  EvalInterval eval_interval(env_);
  switch (v.op()) {
    case AST::ContainerUOp::card:
      return cardinal(Apply(eval_interval, exp));

    default:
      Util::ERROR("EvalExpression: InterUnaryOp %s not supported.", AST::ContUOpNames[v.op()]);
      return 0;
  }
}

ExprBaseType EvalExpression::operator()(AST::InterBinOp v) const
{
  AST::Expr l = v.left(), r = v.right();
  EvalInterval eval_interval(env_);
  switch (v.op()) {
    case AST::ContainerOp::cap:
      return intersection(Apply(eval_interval, l), Apply(eval_interval, r));

    case AST::ContainerOp::less:
      return Apply(eval_interval, l) < Apply(eval_interval, r);

    case AST::ContainerOp::eq:
      return Apply(eval_interval, l) == Apply(eval_interval, r);

    default:
      Util::ERROR("EvalExpression: InterBinOp %s not supported.", AST::ContOpNames[v.op()]);
      return 0;
  }
}

ExprBaseType EvalExpression::operator()(AST::Set v) const
{
  SBG::InterSet res;
  EvalInterval inter_visit(env_);

  BOOST_FOREACH (auto i, v.pieces())
    res.emplace(Apply(inter_visit, i));
 
  return SBG::Set(res);
}

ExprBaseType EvalExpression::operator()(AST::SetUnaryOp v) const
{
  AST::Expr exp = v.e();
  EvalSet eval_set(env_);
  switch (v.op()) {
    case AST::ContainerUOp::card:
      return cardinal(Apply(eval_set, exp));

    case AST::ContainerUOp::comp:
      return complement(Apply(eval_set, exp));

    default:
      Util::ERROR("EvalExpression: SetUnaryOp %s not supported.", AST::ContUOpNames[v.op()]);
      return 0;
  }
}

ExprBaseType EvalExpression::operator()(AST::SetBinOp v) const
{
  AST::Expr l = v.left(), r = v.right();
  EvalSet eval_set(env_);
  switch (v.op()) {
    case AST::ContainerOp::cap:
      return intersection(Apply(eval_set, l), Apply(eval_set, r));

    case AST::ContainerOp::diff:
      return difference(Apply(eval_set, l), Apply(eval_set, r));

    case AST::ContainerOp::eq:
      return Apply(eval_set, l) == Apply(eval_set, r);

    case AST::ContainerOp::cup:
      return cup(Apply(eval_set, l), Apply(eval_set, r));

    default:
      Util::ERROR("EvalExpression: SetBinOp %s not supported.", AST::ContOpNames[v.op()]);
      return 0;
  }
}

ExprBaseType EvalExpression::operator()(AST::LinearExp v) const 
{
  EvalRat visit_rat(env_); 
 
  return SBG::LExp(Apply(visit_rat, v.slope()), Apply(visit_rat, v.offset())); 
}

} // namespace Eval

} // namespace SBG
