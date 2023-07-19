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

#include <eval/visitors/eval_set.hpp>

namespace SBG {

namespace Eval {

EvalSet::EvalSet() : env_() {}
EvalSet::EvalSet(VarEnv env) : env_(env) {}

SBG::Set EvalSet::operator()(AST::Integer v) const { 
  Util::ERROR("EvalSet: trying to evaluate an Integer");
  return SBG::Set(); 
}

SBG::Set EvalSet::operator()(AST::Rational v) const { 
  Util::ERROR("EvalSet: trying to evaluate a Rational");
  return SBG::Set(); 
}

SBG::Set EvalSet::operator()(AST::Boolean v) const { 
  Util::ERROR("EvalSet: trying to evaluate a Boolean");
  return SBG::Set(); 
}

SBG::Set EvalSet::operator()(Util::VariableName v) const { 
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (std::holds_alternative<SBG::Set>(value))
      return std::get<SBG::Set>(value);

    else {
      Util::ERROR("EvalSet: variable %s is not a set", v);
      return SBG::Set(); 
    } 
  }

  Util::ERROR("EvalSet: variable %s not defined", v);
  return SBG::Set(); 
}

SBG::Set EvalSet::operator()(AST::BinOp v) const 
{
  Util::ERROR("EvalSet: trying to evaluate an arithmetic BinOp");
  return SBG::Set(); 
}

SBG::Set EvalSet::operator()(AST::Call v) const
{
  Util::ERROR("EvalSet: trying to evaluate a Call");
  return SBG::Set(); 
}

SBG::Set EvalSet::operator()(AST::Interval v) const
{ 
  Util::ERROR("EvalSet: trying to evaluate an Interval");
  return SBG::Set(); 
}

SBG::Set EvalSet::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalSet: trying to evaluate an InterUnaryOp");
  return SBG::Set(); 
}

SBG::Set EvalSet::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalSet: trying to evaluate an InterBinOp");
  return SBG::Set(); 
}

SBG::Set EvalSet::operator()(AST::Set v) const 
{
  SBG::InterSet res;

  EvalInterval inter_visit(env_);
  BOOST_FOREACH (AST::Expr e, v.pieces())
    res.emplace_hint(res.cend(), Apply(inter_visit, e));     

  return SBG::Set(res);
}

SBG::Set EvalSet::operator()(AST::SetUnaryOp v) const 
{
  AST::Expr e = v.e();
  switch (v.op()) {
    default:
      Util::ERROR("EvalSet: SetUnaryOp %s not supported.", AST::ContUOpNames[v.op()]);
      return SBG::Set(); 
  }
}

SBG::Set EvalSet::operator()(AST::SetBinOp v) const 
{
  AST::Expr l = v.left(), r = v.right();
  switch (v.op()) {
    case AST::ContainerOp::cap:
      return intersection(ApplyThis(l), ApplyThis(r));

    case AST::ContainerOp::diff:
      return difference(ApplyThis(l), ApplyThis(r));

    default:
      Util::ERROR("EvalSet: SetBinOp %s not supported.", AST::ContOpNames[v.op()]);
      return SBG::Set(); 
  }
}

SBG::Set EvalSet::operator()(AST::LinearExp v) const
{ 
  Util::ERROR("EvalSet: trying to evaluate a LinearExp");
  return SBG::Set(); 
}

} // namespace Eval

} // namespace SBG
