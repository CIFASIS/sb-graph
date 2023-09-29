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

#include "eval/visitors/eval_canon_sbg.hpp"

namespace SBG {

namespace Eval {

EvalCanonSBG::EvalCanonSBG() : env_() {}
EvalCanonSBG::EvalCanonSBG(VarEnv env) : env_(env) {}

LIB::CanonSBG EvalCanonSBG::operator()(AST::Natural v) const
{
  Util::ERROR("EvalCanonSBG: trying to evaluate a Natural");
  return LIB::CanonSBG(); 
}

LIB::CanonSBG EvalCanonSBG::operator()(AST::MDNatural v) const
{
  Util::ERROR("EvalCanonSBG: trying to evaluate a MDNatural");
  return LIB::CanonSBG(); 
}

LIB::CanonSBG EvalCanonSBG::operator()(AST::Rational v) const
{
  Util::ERROR("EvalCanonSBG: trying to evaluate a Rational");
  return LIB::CanonSBG(); 
}

LIB::CanonSBG EvalCanonSBG::operator()(AST::Boolean v) const
{
  Util::ERROR("EvalCanonSBG: trying to evaluate a Boolean");
  return LIB::CanonSBG(); 
}

LIB::CanonSBG EvalCanonSBG::operator()(Util::VariableName v) const
{
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (is<LIB::CanonSBG>(value))
      return boost::get<LIB::CanonSBG>(value);

    else {
      Util::ERROR("EvalCanonSBG: variable %s is not a PWMap", v.c_str());
      return LIB::CanonSBG(); 
    } 
  }

  Util::ERROR("EvalCanonSBG: variable %s not defined", v.c_str());
  return LIB::CanonSBG(); 
}

LIB::CanonSBG EvalCanonSBG::operator()(AST::UnaryOp v) const 
{
  Util::ERROR("EvalCanonSBG: trying to evaluate an arithmetic BinOp");
  return LIB::CanonSBG(); 
}

LIB::CanonSBG EvalCanonSBG::operator()(AST::BinOp v) const 
{
  Util::ERROR("EvalCanonSBG: trying to evaluate an arithmetic BinOp");
  return LIB::CanonSBG(); 
}

LIB::CanonSBG EvalCanonSBG::operator()(AST::Call v) const
{
  Util::ERROR("EvalCanonSBG: trying to evaluate a Call");
  return LIB::CanonSBG(); 
}

LIB::CanonSBG EvalCanonSBG::operator()(AST::Interval v) const
{ 
  Util::ERROR("EvalCanonSBG: trying to evaluate an Interval");
  return LIB::CanonSBG(); 
}

LIB::CanonSBG EvalCanonSBG::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalCanonSBG: trying to evaluate an InterUnaryOp");
  return LIB::CanonSBG(); 
}

LIB::CanonSBG EvalCanonSBG::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalCanonSBG: trying to evaluate an InterBinOp");
  return LIB::CanonSBG(); 
}

LIB::CanonSBG EvalCanonSBG::operator()(AST::MultiDimInter v) const
{ 
  Util::ERROR("EvalCanonSBG: trying to evaluate an MultiDimInter");
  return LIB::CanonSBG(); 
}

LIB::CanonSBG EvalCanonSBG::operator()(AST::MDInterUnaryOp v) const
{
  Util::ERROR("EvalCanonSBG: trying to evaluate an MDInterUnaryOp");
  return LIB::CanonSBG(); 
}

LIB::CanonSBG EvalCanonSBG::operator()(AST::MDInterBinOp v) const
{
  Util::ERROR("EvalCanonSBG: trying to evaluate an MDInterBinOp");
  return LIB::CanonSBG(); 
}

LIB::CanonSBG EvalCanonSBG::operator()(AST::Set v) const 
{
  Util::ERROR("EvalCanonSBG: trying to evaluate a Set");
  return LIB::CanonSBG(); 
}

LIB::CanonSBG EvalCanonSBG::operator()(AST::SetUnaryOp v) const 
{
  Util::ERROR("EvalCanonSBG: trying to evaluate a SetUnaryOp");
  return LIB::CanonSBG(); 
}

LIB::CanonSBG EvalCanonSBG::operator()(AST::SetBinOp v) const 
{
  Util::ERROR("EvalCanonSBG: trying to evaluate a SetBinOp");
  return LIB::CanonSBG(); 
}

LIB::CanonSBG EvalCanonSBG::operator()(AST::LinearExp v) const
{ 
  Util::ERROR("EvalCanonSBG: trying to evaluate a LinearExp");
  return LIB::CanonSBG(); 
}

LIB::CanonSBG EvalCanonSBG::operator()(AST::LExpBinOp v) const 
{
  Util::ERROR("EvalCanonSBG: trying to evaluate a LExpBinOp");
  return LIB::CanonSBG(); 
}

LIB::CanonSBG EvalCanonSBG::operator()(AST::MDLExp v) const
{ 
  Util::ERROR("EvalCanonSBG: trying to evaluate a MDLExp");
  return LIB::CanonSBG(); 
}

LIB::CanonSBG EvalCanonSBG::operator()(AST::MDLExpBinOp v) const 
{
  Util::ERROR("EvalCanonSBG: trying to evaluate a MDLExpBinOp");
  return LIB::CanonSBG(); 
}

LIB::CanonSBG EvalCanonSBG::operator()(AST::LinearMap v) const 
{
  Util::ERROR("EvalCanonSBG: trying to evaluate a LinearMap");
  return LIB::CanonSBG(); 
}

LIB::CanonSBG EvalCanonSBG::operator()(AST::PWLMap v) const 
{
  Util::ERROR("EvalCanonSBG: trying to evaluate a PWLMap");
  return LIB::CanonSBG(); 
}

LIB::CanonSBG EvalCanonSBG::operator()(AST::SBG v) const 
{
  EvalOrdSet visit_set(env_);
  EvalCanonPWMap visit_pw(env_);

  LIB::OrdSet V = Apply(visit_set, v.V());
  LIB::CanonPWMap Vmap = Apply(visit_pw, v.Vmap());
  LIB::CanonPWMap map1 = Apply(visit_pw, v.map1());
  LIB::CanonPWMap map2 = Apply(visit_pw, v.map2());
  LIB::CanonPWMap Emap = Apply(visit_pw, v.Emap());

  return LIB::CanonSBG(V, Vmap, map1, map2, Emap);
}

} // namespace Eval

} // namespace SBG
