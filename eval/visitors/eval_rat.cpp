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

#include <eval/visitors/eval_rat.hpp>

namespace SBG {

namespace Eval {

EvalRat::EvalRat() : env_() {}
EvalRat::EvalRat(VarEnv env) : env_(env) {}

Util::RATIONAL EvalRat::operator()(AST::Natural v) const { return Util::RATIONAL(v, 1); }

Util::RATIONAL EvalRat::operator()(AST::Rational v) const { return v; }

Util::RATIONAL EvalRat::operator()(AST::Boolean v) const 
{ 
  if (v)
    return Util::RATIONAL(1, 1);

  return Util::RATIONAL(0, 1); 
}

Util::RATIONAL EvalRat::operator()(Util::VariableName v) const 
{
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (std::holds_alternative<Util::RATIONAL>(value))
      return std::get<Util::RATIONAL>(value);

    else {
      Util::ERROR("EvalRat: variable %s is not rational", v.c_str());
      return Util::RATIONAL(0, 1);
    }
  }

  Util::ERROR("EvalRat: variable %s not defined", v.c_str());
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::BinOp v) const 
{
  AST::Expr l = v.left(), r = v.right();
  switch (v.op()) {
    case AST::Op::add:
      return ApplyThis(l) + ApplyThis(r);

    case AST::Op::sub:
      return ApplyThis(l) - ApplyThis(r);

    case AST::Op::mult:
      return ApplyThis(l) * ApplyThis(r);

    default:
      Util::ERROR("EvalRat: BinOp %s not supported.", AST::OpNames[v.op()]);
      return Util::RATIONAL(0, 1);
  }
}

Util::RATIONAL EvalRat::operator()(AST::Call v) const
{
  Util::ERROR("EvalRat: trying to evaluate a Call");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::Interval v) const
{
  Util::ERROR("EvalRat: trying to evaluate an Interval");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalRat: trying to evaluate an InterUnaryOp");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalRat: trying to evaluate an InterBinOp");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::Set v) const
{
  Util::ERROR("EvalRat: trying to evaluate an Set");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::SetUnaryOp v) const
{
  Util::ERROR("EvalRat: trying to evaluate an SetUnaryOp");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::SetBinOp v) const
{
  Util::ERROR("EvalRat: trying to evaluate an SetBinOp");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::LinearExp v) const
{
  Util::ERROR("EvalRat: trying to evaluate an LinearExp");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::LExpBinOp v) const
{
  Util::ERROR("EvalRat: trying to evaluate an LExpBinOp");
  return Util::RATIONAL(0, 1);
}

} // namespace Eval

} // namespace SBG
