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

#include "eval/visitors/eval_mdi.hpp"

namespace SBG {

namespace Eval {

EvalMDI::EvalMDI() : env_() {}
EvalMDI::EvalMDI(VarEnv env) : env_(env) {}

LIB::MultiDimInter EvalMDI::operator()(AST::Natural v) const 
{ 
  Util::ERROR("EvalMDI: trying to evaluate an Natural");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::MDNatural v) const 
{
  Util::ERROR("EvalMDI: trying to evaluate an MDNatural");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::Rational v) const 
{ 
  Util::ERROR("EvalMDI: trying to evaluate a Rational");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::Boolean v) const 
{ 
  Util::ERROR("EvalMDI: trying to evaluate a Boolean");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(Util::VariableName v) const 
{ 
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (std::holds_alternative<LIB::MultiDimInter>(value))
      return std::get<LIB::MultiDimInter>(value);

    else {
      Util::ERROR("EvalMDI: variable %s is not an interval", v.c_str());
      return LIB::MultiDimInter(); 
    } 
  }

  Util::ERROR("EvalMDI: variable %s not defined", v.c_str());
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::UnaryOp v) const 
{
  Util::ERROR("EvalMDI: trying to evaluate an arithmetic UnaryOp");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::BinOp v) const 
{
  Util::ERROR("EvalMDI: trying to evaluate an arithmetic BinOp");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::Call v) const
{
  Util::ERROR("EvalMDI: trying to evaluate a Call");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::Interval v) const 
{ 
  Util::ERROR("EvalMDI: trying to evaluate an Interval");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalMDI: trying to evaluate an InterUnaryOp");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalMDI: trying to evaluate an InterBinOp");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::MultiDimInter v) const 
{
  LIB::MultiDimInter res;

  EvalInterval visit_inter(env_);
  BOOST_FOREACH (AST::Expr ith, v.intervals()) res.emplaceBack(Apply(visit_inter, ith));

  return res;
}

LIB::MultiDimInter EvalMDI::operator()(AST::MDInterUnaryOp v) const 
{
  AST::Expr exp = v.e();
  switch (v.op()) {
    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalMDI: MDInterUnaryOp %s not supported.", ss.str().c_str());
      return LIB::MultiDimInter(); 
  }
}

LIB::MultiDimInter EvalMDI::operator()(AST::MDInterBinOp v) const 
{
  AST::Expr l = v.left(), r = v.right();
  switch (v.op()) {
    case AST::ContainerOp::cap:
      return intersection(ApplyThis(l), ApplyThis(r));

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalMD: MDInterBinOp %s not supported.", ss.str().c_str());
      return LIB::MultiDimInter(); 
  }
}

LIB::MultiDimInter EvalMDI::operator()(AST::Set v) const 
{
  std::stringstream ss;
  ss << v;
  Util::ERROR("EvalMDI: trying to evaluate the Set %s", ss.str().c_str());
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::SetUnaryOp v) const 
{
  Util::ERROR("EvalMDI: trying to evaluate a SetUnaryOp");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::SetBinOp v) const 
{
  Util::ERROR("EvalMDI: trying to evaluate a SetBinOp");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::LinearExp v) const 
{
  Util::ERROR("EvalMDI: trying to evaluate a LinearExp");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::LExpBinOp v) const 
{
  Util::ERROR("EvalMDI: trying to evaluate a LExpBinOp");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::MDLExp v) const 
{
  Util::ERROR("EvalMDI: trying to evaluate a LinearExp");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::MDLExpBinOp v) const 
{
  Util::ERROR("EvalMDI: trying to evaluate a LExpBinOp");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::LinearMap v) const 
{
  Util::ERROR("EvalMDI: trying to evaluate a LinearMap");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::PWLMap v) const 
{
  Util::ERROR("EvalMDI: trying to evaluate a PWLMap");
  return LIB::MultiDimInter(); 
}

} // namespace Eval

} // namespace SBG
