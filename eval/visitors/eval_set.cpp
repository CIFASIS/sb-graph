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

#include "eval/visitors/eval_set.hpp"

namespace SBG {

namespace Eval {

EvalSet::EvalSet() : nmbr_dims_(), env_() {}
EvalSet::EvalSet(unsigned int nmbr_dims, VarEnv env) : nmbr_dims_(nmbr_dims), env_(env) {}

LIB::Set EvalSet::operator()(AST::Natural v) const
{
  Util::ERROR("EvalSet: trying to evaluate a Natural");
  return LIB::Set();
}

LIB::Set EvalSet::operator()(AST::MDNatural v) const
{
  Util::ERROR("EvalSet: trying to evaluate a MDNatural");
  return LIB::Set();
}

LIB::Set EvalSet::operator()(AST::Rational v) const
{
  Util::ERROR("EvalSet: trying to evaluate a Rational");
  return LIB::Set();
}

LIB::Set EvalSet::operator()(AST::Boolean v) const
{
  Util::ERROR("EvalSet: trying to evaluate a Boolean");
  return LIB::Set();
}

LIB::Set EvalSet::operator()(Util::VariableName v) const
{
  MaybeEBT v_opt = env_[v];
  if (v_opt) {
    ExprBaseType value = *v_opt;
    if (is<LIB::Set>(value))
      return boost::get<LIB::Set>(value);

    else {
      Util::ERROR("EvalSet: variable %s is not a set", v.c_str());
      return LIB::Set();
    }
  }

  Util::ERROR("EvalSet: variable %s not defined", v.c_str());
  return LIB::Set();
}

LIB::Set EvalSet::operator()(AST::UnaryOp v) const
{
  Util::ERROR("EvalSet: trying to evaluate an arithmetic UnaryOp");
  return LIB::Set();
}

LIB::Set EvalSet::operator()(AST::BinOp v) const
{
  Util::ERROR("EvalSet: trying to evaluate an arithmetic BinOp");
  return LIB::Set();
}

LIB::Set EvalSet::operator()(AST::Call v) const
{
  Util::ERROR("EvalSet: trying to evaluate a Call");
  return LIB::Set();
}

LIB::Set EvalSet::operator()(AST::Interval v) const
{
  Util::ERROR("EvalSet: trying to evaluate an Interval");
  return LIB::Set();
}

LIB::Set EvalSet::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalSet: trying to evaluate an InterUnaryOp");
  return LIB::Set();
}

LIB::Set EvalSet::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalSet: trying to evaluate an InterBinOp");
  return LIB::Set();
}

LIB::Set EvalSet::operator()(AST::MultiDimInter v) const
{
  Util::ERROR("EvalSet: trying to evaluate an MultiDimInter");
  return LIB::Set();
}

LIB::Set EvalSet::operator()(AST::MDInterUnaryOp v) const
{
  Util::ERROR("EvalSet: trying to evaluate an MDInterUnaryOp");
  return LIB::Set();
}

LIB::Set EvalSet::operator()(AST::MDInterBinOp v) const
{
  Util::ERROR("EvalSet: trying to evaluate an MDInterBinOp");
  return LIB::Set();
}

LIB::Set EvalSet::operator()(AST::Set v) const
{
  EvalMDI mdi_visit(env_);

  if (nmbr_dims_ == 1) {
    LIB::OrdSet ord_res;
    for (AST::Expr e : v.pieces()) ord_res.emplace_hint(ord_res.end(), Apply(mdi_visit, e));

    return ord_res;
  }

  LIB::UnordSet unord_res;
  for (AST::Expr e : v.pieces()) unord_res.emplace_hint(unord_res.end(), Apply(mdi_visit, e));

  return unord_res;
}

LIB::Set EvalSet::operator()(AST::SetUnaryOp v) const
{
  AST::Expr e = v.e();
  switch (v.op()) {
    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalSet: SetUnaryOp %s not supported.", ss.str().c_str());
      return LIB::Set();
  }
}

LIB::Set EvalSet::operator()(AST::SetBinOp v) const
{
  LIB::Set l = ApplyThis(v.left()), r = ApplyThis(v.right());
  switch (v.op()) {
    case AST::ContainerOp::cap:
      return intersection(l, r);

    case AST::ContainerOp::cup:
      return cup(l, r);

    case AST::ContainerOp::diff:
      return difference(l, r);

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalSet: SetBinOp %s not supported.", ss.str().c_str());
      return LIB::Set();
  }
}

LIB::Set EvalSet::operator()(AST::LinearExp v) const
{
  Util::ERROR("EvalSet: trying to evaluate a LinearExp");
  return LIB::Set();
}

LIB::Set EvalSet::operator()(AST::LExpBinOp v) const
{
  Util::ERROR("EvalSet: trying to evaluate a LExpBinOp");
  return LIB::Set();
}

LIB::Set EvalSet::operator()(AST::MDLExp v) const
{
  Util::ERROR("EvalSet: trying to evaluate a MDLExp");
  return LIB::Set();
}

LIB::Set EvalSet::operator()(AST::MDLExpBinOp v) const
{
  Util::ERROR("EvalSet: trying to evaluate a MDLExpBinOp");
  return LIB::Set();
}

LIB::Set EvalSet::operator()(AST::LinearMap v) const
{
  Util::ERROR("EvalSet: trying to evaluate a LinearMap");
  return LIB::Set();
}

LIB::Set EvalSet::operator()(AST::PWLMap v) const
{
  Util::ERROR("EvalSet: trying to evaluate a PWLMap");
  return LIB::Set();
}

LIB::Set EvalSet::operator()(AST::SBG v) const
{
  Util::ERROR("EvalSet: trying to evaluate a SBG");
  return LIB::Set();
}

LIB::Set EvalSet::operator()(AST::DSBG v) const
{
  Util::ERROR("EvalSet: trying to evaluate a DSBG");
  return LIB::Set();
}

} // namespace Eval

} // namespace SBG
