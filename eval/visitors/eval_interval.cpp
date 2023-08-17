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

#include "eval/visitors/eval_interval.hpp"

namespace SBG {

namespace Eval {

EvalInterval::EvalInterval() : env_() {}
EvalInterval::EvalInterval(VarEnv env) : env_(env) {}

LIB::Interval EvalInterval::operator()(AST::Natural v) const 
{ 
  Util::ERROR("EvalInterval: trying to evaluate an Natural");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::Rational v) const 
{ 
  Util::ERROR("EvalInterval: trying to evaluate a Rational");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::Boolean v) const 
{ 
  Util::ERROR("EvalInterval: trying to evaluate a Boolean");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(Util::VariableName v) const 
{ 
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (std::holds_alternative<LIB::Interval>(value))
      return std::get<LIB::Interval>(value);

    else {
      Util::ERROR("EvalInterval: variable %s is not an interval", v.c_str());
      return LIB::Interval(); 
    } 
  }

  Util::ERROR("EvalInterval: variable %s not defined", v.c_str());
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::UnaryOp v) const 
{
  Util::ERROR("EvalInterval: trying to evaluate an arithmetic UnaryOp");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::BinOp v) const 
{
  Util::ERROR("EvalInterval: trying to evaluate an arithmetic BinOp");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::Call v) const
{
  Util::ERROR("EvalInterval: trying to evaluate a Call");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::Interval v) const 
{ 
  EvalNat visit_nat(env_);
  Util::NAT b = Apply(visit_nat, v.begin());
  Util::NAT s = Apply(visit_nat, v.step());
  Util::NAT e = Apply(visit_nat, v.end());

  return LIB::Interval(b, s, e);
}

LIB::Interval EvalInterval::operator()(AST::InterUnaryOp v) const
{
  AST::Expr exp = v.e();
  switch (v.op()) {
    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalInterval: InterUnaryOp %s not supported.", ss.str().c_str());
      return LIB::Interval(); 
  }
}

LIB::Interval EvalInterval::operator()(AST::InterBinOp v) const
{
  AST::Expr l = v.left(), r = v.right();
  switch (v.op()) {
    case AST::ContainerOp::cap:
      return intersection(ApplyThis(l), ApplyThis(r));

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalInterval: InterBinOp %s not supported.", ss.str().c_str());
      return LIB::Interval(); 
  }
}

LIB::Interval EvalInterval::operator()(AST::Set v) const 
{
  std::stringstream ss;
  ss << v;
  Util::ERROR("EvalInterval: trying to evaluate the Set %s", ss.str().c_str());
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::SetUnaryOp v) const 
{
  Util::ERROR("EvalInterval: trying to evaluate a SetUnaryOp");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::SetBinOp v) const 
{
  Util::ERROR("EvalInterval: trying to evaluate a SetBinOp");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::LinearExp v) const 
{
  Util::ERROR("EvalInterval: trying to evaluate a LinearExp");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::LExpBinOp v) const 
{
  Util::ERROR("EvalInterval: trying to evaluate a LExpBinOp");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::LinearMap v) const 
{
  Util::ERROR("EvalInterval: trying to evaluate a LinearMap");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::PWLMap v) const 
{
  Util::ERROR("EvalInterval: trying to evaluate a PWLMap");
  return LIB::Interval(); 
}

} // namespace Eval

} // namespace SBG
