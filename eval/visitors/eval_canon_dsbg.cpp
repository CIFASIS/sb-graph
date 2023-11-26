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

#include "eval/visitors/eval_canon_dsbg.hpp"

namespace SBG {

namespace Eval {

EvalCanonDSBG::EvalCanonDSBG() : env_() {}
EvalCanonDSBG::EvalCanonDSBG(VarEnv env) : env_(env) {}

LIB::CanonDSBG EvalCanonDSBG::operator()(AST::Natural v) const
{
  Util::ERROR("EvalCanonDSBG: trying to evaluate a Natural");
  return LIB::CanonDSBG(); 
}

LIB::CanonDSBG EvalCanonDSBG::operator()(AST::MDNatural v) const
{
  Util::ERROR("EvalCanonDSBG: trying to evaluate a MDNatural");
  return LIB::CanonDSBG(); 
}

LIB::CanonDSBG EvalCanonDSBG::operator()(AST::Rational v) const
{
  Util::ERROR("EvalCanonDSBG: trying to evaluate a Rational");
  return LIB::CanonDSBG(); 
}

LIB::CanonDSBG EvalCanonDSBG::operator()(AST::Boolean v) const
{
  Util::ERROR("EvalCanonDSBG: trying to evaluate a Boolean");
  return LIB::CanonDSBG(); 
}

LIB::CanonDSBG EvalCanonDSBG::operator()(Util::VariableName v) const
{
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (is<LIB::CanonDSBG>(value))
      return boost::get<LIB::CanonDSBG>(value);

    else {
      Util::ERROR("EvalCanonDSBG: variable %s is not a PWMap", v.c_str());
      return LIB::CanonDSBG(); 
    } 
  }

  Util::ERROR("EvalCanonDSBG: variable %s not defined", v.c_str());
  return LIB::CanonDSBG(); 
}

LIB::CanonDSBG EvalCanonDSBG::operator()(AST::UnaryOp v) const 
{
  Util::ERROR("EvalCanonDSBG: trying to evaluate an arithmetic BinOp");
  return LIB::CanonDSBG(); 
}

LIB::CanonDSBG EvalCanonDSBG::operator()(AST::BinOp v) const 
{
  Util::ERROR("EvalCanonDSBG: trying to evaluate an arithmetic BinOp");
  return LIB::CanonDSBG(); 
}

LIB::CanonDSBG EvalCanonDSBG::operator()(AST::Call v) const
{
  Util::ERROR("EvalCanonDSBG: trying to evaluate a Call");
  return LIB::CanonDSBG(); 
}

LIB::CanonDSBG EvalCanonDSBG::operator()(AST::Interval v) const
{ 
  Util::ERROR("EvalCanonDSBG: trying to evaluate an Interval");
  return LIB::CanonDSBG(); 
}

LIB::CanonDSBG EvalCanonDSBG::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalCanonDSBG: trying to evaluate an InterUnaryOp");
  return LIB::CanonDSBG(); 
}

LIB::CanonDSBG EvalCanonDSBG::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalCanonDSBG: trying to evaluate an InterBinOp");
  return LIB::CanonDSBG(); 
}

LIB::CanonDSBG EvalCanonDSBG::operator()(AST::MultiDimInter v) const
{ 
  Util::ERROR("EvalCanonDSBG: trying to evaluate an MultiDimInter");
  return LIB::CanonDSBG(); 
}

LIB::CanonDSBG EvalCanonDSBG::operator()(AST::MDInterUnaryOp v) const
{
  Util::ERROR("EvalCanonDSBG: trying to evaluate an MDInterUnaryOp");
  return LIB::CanonDSBG(); 
}

LIB::CanonDSBG EvalCanonDSBG::operator()(AST::MDInterBinOp v) const
{
  Util::ERROR("EvalCanonDSBG: trying to evaluate an MDInterBinOp");
  return LIB::CanonDSBG(); 
}

LIB::CanonDSBG EvalCanonDSBG::operator()(AST::Set v) const 
{
  Util::ERROR("EvalCanonDSBG: trying to evaluate a Set");
  return LIB::CanonDSBG(); 
}

LIB::CanonDSBG EvalCanonDSBG::operator()(AST::SetUnaryOp v) const 
{
  Util::ERROR("EvalCanonDSBG: trying to evaluate a SetUnaryOp");
  return LIB::CanonDSBG(); 
}

LIB::CanonDSBG EvalCanonDSBG::operator()(AST::SetBinOp v) const 
{
  Util::ERROR("EvalCanonDSBG: trying to evaluate a SetBinOp");
  return LIB::CanonDSBG(); 
}

LIB::CanonDSBG EvalCanonDSBG::operator()(AST::LinearExp v) const
{ 
  Util::ERROR("EvalCanonDSBG: trying to evaluate a LinearExp");
  return LIB::CanonDSBG(); 
}

LIB::CanonDSBG EvalCanonDSBG::operator()(AST::LExpBinOp v) const 
{
  Util::ERROR("EvalCanonDSBG: trying to evaluate a LExpBinOp");
  return LIB::CanonDSBG(); 
}

LIB::CanonDSBG EvalCanonDSBG::operator()(AST::MDLExp v) const
{ 
  Util::ERROR("EvalCanonDSBG: trying to evaluate a MDLExp");
  return LIB::CanonDSBG(); 
}

LIB::CanonDSBG EvalCanonDSBG::operator()(AST::MDLExpBinOp v) const 
{
  Util::ERROR("EvalCanonDSBG: trying to evaluate a MDLExpBinOp");
  return LIB::CanonDSBG(); 
}

LIB::CanonDSBG EvalCanonDSBG::operator()(AST::LinearMap v) const 
{
  Util::ERROR("EvalCanonDSBG: trying to evaluate a LinearMap");
  return LIB::CanonDSBG(); 
}

LIB::CanonDSBG EvalCanonDSBG::operator()(AST::PWLMap v) const 
{
  Util::ERROR("EvalCanonDSBG: trying to evaluate a PWLMap");
  return LIB::CanonDSBG(); 
}

LIB::CanonDSBG EvalCanonDSBG::operator()(AST::SBG v) const 
{
  Util::ERROR("EvalCanonDSBG: trying to evaluate a SBG");
  return LIB::CanonDSBG(); 
}

LIB::CanonDSBG EvalCanonDSBG::operator()(AST::DSBG v) const 
{
  EvalOrdSet visit_set(env_);
  EvalCanonPWMap visit_pw(env_);

  LIB::OrdSet V = boost::apply_visitor(visit_set, v.V());
  LIB::CanonPWMap Vmap = boost::apply_visitor(visit_pw, v.Vmap());
  LIB::CanonPWMap mapB = boost::apply_visitor(visit_pw, v.mapB());
  LIB::CanonPWMap mapD = boost::apply_visitor(visit_pw, v.mapD());
  LIB::CanonPWMap Emap = boost::apply_visitor(visit_pw, v.Emap());

  return LIB::CanonDSBG(V, Vmap, mapB, mapD, Emap);
}

} // namespace Eval

} // namespace SBG
