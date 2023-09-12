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

#include "eval/visitors/eval_mdle.hpp"

namespace SBG {

namespace Eval {

EvalMDLE::EvalMDLE() : env_() {}
EvalMDLE::EvalMDLE(VarEnv env) : env_(env) {}

LIB::MDLExp EvalMDLE::operator()(AST::Natural v) const
{
  Util::ERROR("EvalMDLE: trying to evaluate a Natural");
  return LIB::MDLExp(); 
}

LIB::MDLExp EvalMDLE::operator()(AST::MDNatural v) const
{
  Util::ERROR("EvalMDLE: trying to evaluate a MDNatural");
  return LIB::MDLExp(); 
}

LIB::MDLExp EvalMDLE::operator()(AST::Rational v) const
{
  Util::ERROR("EvalMDLE: trying to evaluate a Rational");
  return LIB::MDLExp(); 
}

LIB::MDLExp EvalMDLE::operator()(AST::Boolean v) const
{
  Util::ERROR("EvalMDLE: trying to evaluate a Boolean");
  return LIB::MDLExp(); 
}

LIB::MDLExp EvalMDLE::operator()(Util::VariableName v) const
{
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (std::holds_alternative<LIB::MDLExp>(value))
      return std::get<LIB::MDLExp>(value);

    else {
      Util::ERROR("EvalMDLE: variable %s is not a linear expression", v.c_str());
      return LIB::MDLExp(); 
    } 
  }

  Util::ERROR("EvalMDLE: variable %s not defined", v.c_str());
  return LIB::MDLExp(); 
}

LIB::MDLExp EvalMDLE::operator()(AST::UnaryOp v) const
{
  Util::ERROR("EvalMDLE: trying to evaluate an arithmetic UnaryOp");
  return LIB::MDLExp(); 
}

LIB::MDLExp EvalMDLE::operator()(AST::BinOp v) const 
{
  Util::ERROR("EvalMDLE: trying to evaluate an arithmetic BinOp");
  return LIB::MDLExp(); 
}

LIB::MDLExp EvalMDLE::operator()(AST::Call v) const
{
  Util::ERROR("EvalMDLE: trying to evaluate a Call");
  return LIB::MDLExp(); 
}

LIB::MDLExp EvalMDLE::operator()(AST::Interval v) const
{ 
  Util::ERROR("EvalMDLE: trying to evaluate an Interval");
  return LIB::MDLExp(); 
}

LIB::MDLExp EvalMDLE::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalMDLE: trying to evaluate an InterUnaryOp");
  return LIB::MDLExp(); 
}

LIB::MDLExp EvalMDLE::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalMDLE: trying to evaluate an InterBinOp");
  return LIB::MDLExp(); 
}

LIB::MDLExp EvalMDLE::operator()(AST::MultiDimInter v) const
{ 
  Util::ERROR("EvalMDLE: trying to evaluate an MultiDimInter");
  return LIB::MDLExp(); 
}

LIB::MDLExp EvalMDLE::operator()(AST::MDInterUnaryOp v) const
{
  Util::ERROR("EvalMDLE: trying to evaluate an MDInterUnaryOp");
  return LIB::MDLExp(); 
}

LIB::MDLExp EvalMDLE::operator()(AST::MDInterBinOp v) const
{
  Util::ERROR("EvalMDLE: trying to evaluate an MDInterBinOp");
  return LIB::MDLExp(); 
}

LIB::MDLExp EvalMDLE::operator()(AST::Set v) const 
{
  Util::ERROR("EvalMDLE: trying to evaluate a Set");
  return LIB::MDLExp(); 
}

LIB::MDLExp EvalMDLE::operator()(AST::SetUnaryOp v) const 
{
  Util::ERROR("EvalMDLE: trying to evaluate a SetUnaryOp");
  return LIB::MDLExp(); 
}

LIB::MDLExp EvalMDLE::operator()(AST::SetBinOp v) const 
{
  Util::ERROR("EvalMDLE: trying to evaluate a SetBinOp");
  return LIB::MDLExp(); 
}

LIB::MDLExp EvalMDLE::operator()(AST::LinearExp v) const
{
  Util::ERROR("EvalMDLE: trying to evaluate a LinearExp");
  return LIB::MDLExp(); 
}

LIB::MDLExp EvalMDLE::operator()(AST::LExpBinOp v) const 
{
  Util::ERROR("EvalMDLE: trying to evaluate a LExpBinOp");
  return LIB::MDLExp(); 
}

LIB::MDLExp EvalMDLE::operator()(AST::MDLExp v) const
{ 
  LIB::MDLExp res;

  EvalLE visit_le(env_);
  BOOST_FOREACH (AST::Expr le, v.exps())
    res.emplaceBack(Apply(visit_le, le));

  return LIB::MDLExp(res);
}

LIB::MDLExp EvalMDLE::operator()(AST::MDLExpBinOp v) const 
{
  AST::Expr l = v.left(), r = v.right();
  switch (v.op()) {
    case AST::ExpOp::add:
      return ApplyThis(l) + ApplyThis(r);

    case AST::ExpOp::sub:
      return ApplyThis(l) - ApplyThis(r);

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalMDLE: LExpBinOp %s not supported.", ss.str().c_str());
      return LIB::MDLExp(); 
  }
}

LIB::MDLExp EvalMDLE::operator()(AST::LinearMap v) const 
{
  Util::ERROR("EvalMDLE: trying to evaluate a LinearMap");
  return LIB::MDLExp(); 
}

LIB::MDLExp EvalMDLE::operator()(AST::PWLMap v) const 
{
  Util::ERROR("EvalMDLE: trying to evaluate a PWLMap");
  return LIB::MDLExp(); 
}

} // namespace Eval

} // namespace SBG
