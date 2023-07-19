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

#include <eval/visitors/eval_int.hpp>

namespace SBG {

namespace Eval {

EvalInt::EvalInt() : env_() {}
EvalInt::EvalInt(VarEnv env) : env_(env) {}

Util::INT EvalInt::operator()(AST::Integer v) const { return v; }

Util::INT EvalInt::operator()(AST::Rational v) const 
{ 
  if (v.denominator() == 1)
    return v.numerator();

  Util::ERROR("EvalInt: trying to evaluate a Rational");
  return 0; 
}

Util::INT EvalInt::operator()(AST::Boolean v) const 
{ 
  if (v)
    return 1;

  return 0; 
}

Util::INT EvalInt::operator()(Util::VariableName v) const 
{ 
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    return toInt(value); 
  }

  Util::ERROR("EvalInt: variable %s not defined", v);
  return 0;
}

Util::INT EvalInt::operator()(AST::BinOp v) const 
{
  AST::Expr l = v.left(), r = v.right();
  switch (v.op()) {
    case AST::Op::add:
      return ApplyThis(l) + ApplyThis(r);

    case AST::Op::sub:
      return ApplyThis(l) - ApplyThis(r);

    case AST::Op::mult:
      return ApplyThis(l) * ApplyThis(r);

    case AST::Op::expo:
      return pow(ApplyThis(l), ApplyThis(r));

    default:
      Util::ERROR("EvalInt: BinOp %s not supported.", AST::OpNames[v.op()]);
      return 0;
  }
}

Util::INT EvalInt::operator()(AST::Call v) const
{
  Util::ERROR("EvalInt: trying to evaluate a Call");
  return 0;
}

Util::INT EvalInt::operator()(AST::Interval v) const
{
  Util::ERROR("EvalInt: trying to evaluate an Interval");
  return 0;
}

Util::INT EvalInt::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalInt: trying to evaluate an InterUnaryOp");
  return 0;
}

Util::INT EvalInt::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalInt: trying to evaluate an InterBinOp");
  return 0;
}

Util::INT EvalInt::operator()(AST::Set v) const
{
  Util::ERROR("EvalInt: trying to evaluate an Set");
  return 0;
}

Util::INT EvalInt::operator()(AST::SetUnaryOp v) const
{
  Util::ERROR("EvalInt: trying to evaluate an SetUnaryOp");
  return 0;
}

Util::INT EvalInt::operator()(AST::SetBinOp v) const
{
  Util::ERROR("EvalInt: trying to evaluate an SetBinOp");
  return 0;
}

Util::INT EvalInt::operator()(AST::LinearExp v) const
{
  Util::ERROR("EvalInt: trying to evaluate a LinearExp");
  return 0;
}

} // namespace Eval

} // namespace SBG
