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

#include "eval/visitors/eval_ord_set.hpp"

namespace SBG {

namespace Eval {

EvalOrdSet::EvalOrdSet() : env_() {}
EvalOrdSet::EvalOrdSet(VarEnv env) : env_(env) {}

LIB::OrdSet EvalOrdSet::operator()(AST::Natural v) const
{
  Util::ERROR("EvalOrdSet: trying to evaluate a Natural");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::MDNatural v) const
{
  Util::ERROR("EvalOrdSet: trying to evaluate a MDNatural");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::Rational v) const
{
  Util::ERROR("EvalOrdSet: trying to evaluate a Rational");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::Boolean v) const
{
  Util::ERROR("EvalOrdSet: trying to evaluate a Boolean");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(Util::VariableName v) const
{
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (is<LIB::OrdSet>(value))
      return boost::get<LIB::OrdSet>(value);

    else {
      Util::ERROR("EvalOrdSet: variable %s is not a set", v.c_str());
      return LIB::OrdSet(); 
    } 
  }

  Util::ERROR("EvalOrdSet: variable %s not defined", v.c_str());
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::UnaryOp v) const 
{
  Util::ERROR("EvalOrdSet: trying to evaluate an arithmetic UnaryOp");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::BinOp v) const 
{
  Util::ERROR("EvalOrdSet: trying to evaluate an arithmetic BinOp");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::Call v) const
{
  Util::ERROR("EvalOrdSet: trying to evaluate a Call");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::Interval v) const
{ 
  Util::ERROR("EvalOrdSet: trying to evaluate an Interval");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalOrdSet: trying to evaluate an InterUnaryOp");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalOrdSet: trying to evaluate an InterBinOp");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::MultiDimInter v) const
{ 
  Util::ERROR("EvalOrdSet: trying to evaluate an MultiDimInter");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::MDInterUnaryOp v) const
{
  Util::ERROR("EvalOrdSet: trying to evaluate an MDInterUnaryOp");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::MDInterBinOp v) const
{
  Util::ERROR("EvalOrdSet: trying to evaluate an MDInterBinOp");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::Set v) const 
{
  LIB::OrdSet res;

  EvalMDI mdi_visit(env_);
  BOOST_FOREACH (AST::Expr e, v.pieces()) res.emplace_hint(res.end(), Apply(mdi_visit, e));     

  return res;
}

LIB::OrdSet EvalOrdSet::operator()(AST::SetUnaryOp v) const 
{
  AST::Expr e = v.e();
  switch (v.op()) {
    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalOrdSet: SetUnaryOp %s not supported.", ss.str().c_str());
      return LIB::OrdSet(); 
  }
}

LIB::OrdSet EvalOrdSet::operator()(AST::SetBinOp v) const 
{
  AST::Expr l = v.left(), r = v.right();
  switch (v.op()) {
    case AST::ContainerOp::cap:
      return intersection(ApplyThis(l), ApplyThis(r));

    case AST::ContainerOp::diff:
      return difference(ApplyThis(l), ApplyThis(r));

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalOrdSet: SetBinOp %s not supported.", ss.str().c_str());
      return LIB::OrdSet(); 
  }
}

LIB::OrdSet EvalOrdSet::operator()(AST::LinearExp v) const
{ 
  Util::ERROR("EvalOrdSet: trying to evaluate a LinearExp");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::LExpBinOp v) const
{ 
  Util::ERROR("EvalOrdSet: trying to evaluate a LExpBinOp");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::MDLExp v) const
{ 
  Util::ERROR("EvalOrdSet: trying to evaluate a MDLExp");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::MDLExpBinOp v) const
{ 
  Util::ERROR("EvalOrdSet: trying to evaluate a MDLExpBinOp");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::LinearMap v) const
{ 
  Util::ERROR("EvalOrdSet: trying to evaluate a LinearMap");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::PWLMap v) const
{ 
  Util::ERROR("EvalOrdSet: trying to evaluate a PWLMap");
  return LIB::OrdSet(); 
}

} // namespace Eval

} // namespace SBG
