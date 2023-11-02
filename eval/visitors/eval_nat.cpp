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

#include "eval/visitors/eval_nat.hpp"

namespace SBG {

namespace Eval {

EvalNat::EvalNat() : env_() {}
EvalNat::EvalNat(VarEnv env) : env_(env) {}

Util::NAT EvalNat::operator()(AST::Natural v) const { return v; }

Util::NAT EvalNat::operator()(AST::MDNatural v) const 
{
  Util::ERROR("EvalNat: trying to evaluate a MDNatural");
  return 0;
}

Util::NAT EvalNat::operator()(AST::Rational v) const 
{ 
  if (ApplyThis(v.den()) == 1)
    return ApplyThis(v.num());

  Util::ERROR("EvalNat: trying to evaluate a Rational");
  return 0; 
}

Util::NAT EvalNat::operator()(AST::Boolean v) const 
{ 
  if (v)
    return 1;

  return 0; 
}

Util::NAT EvalNat::operator()(Util::VariableName v) const 
{ 
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (is<Util::NAT>(value)) return boost::get<Util::NAT>(value);

    else if (is<Util::RATIONAL>(value)) return Util::toNat(boost::get<Util::RATIONAL>(value)); 
  }

  Util::ERROR("EvalNat: variable %s not defined", v.c_str());
  return 0;
}

Util::NAT EvalNat::operator()(AST::UnaryOp v) const 
{
  Util::ERROR("EvalNat: trying to evaluate an arithmetic UnaryOp");
  return 0;
}

Util::NAT EvalNat::operator()(AST::BinOp v) const 
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
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalNat: BinOp %s not supported.", ss.str().c_str());
      return 0;
  }
}

Util::NAT EvalNat::operator()(AST::Call v) const
{
  Util::ERROR("EvalNat: trying to evaluate a Call");
  return 0;
}

Util::NAT EvalNat::operator()(AST::Interval v) const
{
  Util::ERROR("EvalNat: trying to evaluate an Interval");
  return 0;
}

Util::NAT EvalNat::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalNat: trying to evaluate an InterUnaryOp");
  return 0;
}

Util::NAT EvalNat::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalNat: trying to evaluate an InterBinOp");
  return 0;
}

Util::NAT EvalNat::operator()(AST::MultiDimInter v) const
{
  Util::ERROR("EvalNat: trying to evaluate an MultiDimInter");
  return 0;
}

Util::NAT EvalNat::operator()(AST::MDInterUnaryOp v) const
{
  Util::ERROR("EvalNat: trying to evaluate an MDInterUnaryOp");
  return 0;
}

Util::NAT EvalNat::operator()(AST::MDInterBinOp v) const
{
  Util::ERROR("EvalNat: trying to evaluate an MDInterBinOp");
  return 0;
}

Util::NAT EvalNat::operator()(AST::Set v) const
{
  Util::ERROR("EvalNat: trying to evaluate an Set");
  return 0;
}

Util::NAT EvalNat::operator()(AST::SetUnaryOp v) const
{
  Util::ERROR("EvalNat: trying to evaluate an SetUnaryOp");
  return 0;
}

Util::NAT EvalNat::operator()(AST::SetBinOp v) const
{
  Util::ERROR("EvalNat: trying to evaluate an SetBinOp");
  return 0;
}

Util::NAT EvalNat::operator()(AST::LinearExp v) const
{
  Util::ERROR("EvalNat: trying to evaluate a LinearExp");
  return 0;
}

Util::NAT EvalNat::operator()(AST::LExpBinOp v) const
{
  Util::ERROR("EvalNat: trying to evaluate a LExpBinOp");
  return 0;
}

Util::NAT EvalNat::operator()(AST::MDLExp v) const
{
  Util::ERROR("EvalNat: trying to evaluate a MDLExp");
  return 0;
}

Util::NAT EvalNat::operator()(AST::MDLExpBinOp v) const
{
  Util::ERROR("EvalNat: trying to evaluate a MDLExpBinOp");
  return 0;
}

Util::NAT EvalNat::operator()(AST::LinearMap v) const
{
  Util::ERROR("EvalNat: trying to evaluate a LinearMap");
  return 0;
}

Util::NAT EvalNat::operator()(AST::PWLMap v) const
{
  Util::ERROR("EvalNat: trying to evaluate a PWLMap");
  return 0;
}

Util::NAT EvalNat::operator()(AST::SBG v) const
{
  Util::ERROR("EvalNat: trying to evaluate a SBG");
  return 0;
}

Util::NAT EvalNat::operator()(AST::DSBG v) const
{
  Util::ERROR("EvalNat: trying to evaluate a DSBG");
  return 0;
}

} // namespace Eval

} // namespace SBG
