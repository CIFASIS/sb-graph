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

#include <eval/visitors/eval_le.hpp>

namespace SBG {

namespace Eval {

EvalLE::EvalLE() : env_() {}
EvalLE::EvalLE(VarEnv env) : env_(env) {}

LExp EvalLE::operator()(AST::Natural v) const { 
  Util::ERROR("EvalLE: trying to evaluate a Natural");
  return LExp(); 
}

LExp EvalLE::operator()(AST::Rational v) const { 
  Util::ERROR("EvalLE: trying to evaluate a Rational");
  return LExp(); 
}

LExp EvalLE::operator()(AST::Boolean v) const { 
  Util::ERROR("EvalLE: trying to evaluate a Boolean");
  return LExp(); 
}

LExp EvalLE::operator()(Util::VariableName v) const { 
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (std::holds_alternative<LExp>(value))
      return std::get<LExp>(value);

    else {
      Util::ERROR("EvalLE: variable %s is not a linear expression", v.c_str());
      return LExp(); 
    } 
  }

  Util::ERROR("EvalLE: variable %s not defined", v.c_str());
  return LExp(); 
}

LExp EvalLE::operator()(AST::BinOp v) const 
{
  Util::ERROR("EvalLE: trying to evaluate an arithmetic BinOp");
  return LExp(); 
}

LExp EvalLE::operator()(AST::Call v) const
{
  Util::ERROR("EvalLE: trying to evaluate a Call");
  return LExp(); 
}

LExp EvalLE::operator()(AST::Interval v) const
{ 
  Util::ERROR("EvalLE: trying to evaluate an Interval");
  return LExp(); 
}

LExp EvalLE::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalLE: trying to evaluate an InterUnaryOp");
  return LExp(); 
}

LExp EvalLE::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalLE: trying to evaluate an InterBinOp");
  return LExp(); 
}

LExp EvalLE::operator()(AST::Set v) const 
{
  Util::ERROR("EvalLE: trying to evaluate a Set");
  return LExp(); 
}

LExp EvalLE::operator()(AST::SetUnaryOp v) const 
{
  Util::ERROR("EvalLE: trying to evaluate a SetUnaryOp");
  return LExp(); 
}

LExp EvalLE::operator()(AST::SetBinOp v) const 
{
  Util::ERROR("EvalLE: trying to evaluate a SetBinOp");
  return LExp(); 
}

LExp EvalLE::operator()(AST::LinearExp le) const
{ 
  EvalRat visit_rat(env_);

  return LExp(Apply(visit_rat, le.slope()), Apply(visit_rat, le.offset()));
}

LExp EvalLE::operator()(AST::LExpBinOp v) const 
{
  AST::Expr l = v.left(), r = v.right();
  switch (v.op()) {
    case AST::Op::add:
      return ApplyThis(l) + ApplyThis(r);

    case AST::Op::sub:
      return ApplyThis(l) + ApplyThis(r);

    default:
      Util::ERROR("EvalLE: LExpBinOp %s not supported.", AST::OpNames[v.op()]);
      return SBG::LExp(); 
  }
}

} // namespace Eval

} // namespace SBG
