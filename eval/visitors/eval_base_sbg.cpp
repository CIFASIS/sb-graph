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

#include "eval/visitors/eval_base_sbg.hpp"

namespace SBG {

namespace Eval {

EvalBaseSBG::EvalBaseSBG() : env_() {}
EvalBaseSBG::EvalBaseSBG(VarEnv env) : env_(env) {}

LIB::BaseSBG EvalBaseSBG::operator()(AST::Natural v) const
{
  Util::ERROR("EvalBaseSBG: trying to evaluate a Natural");
  return LIB::BaseSBG(); 
}

LIB::BaseSBG EvalBaseSBG::operator()(AST::MDNatural v) const
{
  Util::ERROR("EvalBaseSBG: trying to evaluate a MDNatural");
  return LIB::BaseSBG(); 
}

LIB::BaseSBG EvalBaseSBG::operator()(AST::Rational v) const
{
  Util::ERROR("EvalBaseSBG: trying to evaluate a Rational");
  return LIB::BaseSBG(); 
}

LIB::BaseSBG EvalBaseSBG::operator()(AST::Boolean v) const
{
  Util::ERROR("EvalBaseSBG: trying to evaluate a Boolean");
  return LIB::BaseSBG(); 
}

LIB::BaseSBG EvalBaseSBG::operator()(Util::VariableName v) const
{
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (is<LIB::BaseSBG>(value))
      return boost::get<LIB::BaseSBG>(value);

    else {
      Util::ERROR("EvalBaseSBG: variable %s is not a PWMap", v.c_str());
      return LIB::BaseSBG(); 
    } 
  }

  Util::ERROR("EvalBaseSBG: variable %s not defined", v.c_str());
  return LIB::BaseSBG(); 
}

LIB::BaseSBG EvalBaseSBG::operator()(AST::UnaryOp v) const 
{
  Util::ERROR("EvalBaseSBG: trying to evaluate an arithmetic BinOp");
  return LIB::BaseSBG(); 
}

LIB::BaseSBG EvalBaseSBG::operator()(AST::BinOp v) const 
{
  Util::ERROR("EvalBaseSBG: trying to evaluate an arithmetic BinOp");
  return LIB::BaseSBG(); 
}

LIB::BaseSBG EvalBaseSBG::operator()(AST::Call v) const
{
  Util::ERROR("EvalBaseSBG: trying to evaluate a Call");
  return LIB::BaseSBG(); 
}

LIB::BaseSBG EvalBaseSBG::operator()(AST::Interval v) const
{ 
  Util::ERROR("EvalBaseSBG: trying to evaluate an Interval");
  return LIB::BaseSBG(); 
}

LIB::BaseSBG EvalBaseSBG::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalBaseSBG: trying to evaluate an InterUnaryOp");
  return LIB::BaseSBG(); 
}

LIB::BaseSBG EvalBaseSBG::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalBaseSBG: trying to evaluate an InterBinOp");
  return LIB::BaseSBG(); 
}

LIB::BaseSBG EvalBaseSBG::operator()(AST::MultiDimInter v) const
{ 
  Util::ERROR("EvalBaseSBG: trying to evaluate an MultiDimInter");
  return LIB::BaseSBG(); 
}

LIB::BaseSBG EvalBaseSBG::operator()(AST::MDInterUnaryOp v) const
{
  Util::ERROR("EvalBaseSBG: trying to evaluate an MDInterUnaryOp");
  return LIB::BaseSBG(); 
}

LIB::BaseSBG EvalBaseSBG::operator()(AST::MDInterBinOp v) const
{
  Util::ERROR("EvalBaseSBG: trying to evaluate an MDInterBinOp");
  return LIB::BaseSBG(); 
}

LIB::BaseSBG EvalBaseSBG::operator()(AST::Set v) const 
{
  Util::ERROR("EvalBaseSBG: trying to evaluate a Set");
  return LIB::BaseSBG(); 
}

LIB::BaseSBG EvalBaseSBG::operator()(AST::SetUnaryOp v) const 
{
  Util::ERROR("EvalBaseSBG: trying to evaluate a SetUnaryOp");
  return LIB::BaseSBG(); 
}

LIB::BaseSBG EvalBaseSBG::operator()(AST::SetBinOp v) const 
{
  Util::ERROR("EvalBaseSBG: trying to evaluate a SetBinOp");
  return LIB::BaseSBG(); 
}

LIB::BaseSBG EvalBaseSBG::operator()(AST::LinearExp v) const
{ 
  Util::ERROR("EvalBaseSBG: trying to evaluate a LinearExp");
  return LIB::BaseSBG(); 
}

LIB::BaseSBG EvalBaseSBG::operator()(AST::LExpBinOp v) const 
{
  Util::ERROR("EvalBaseSBG: trying to evaluate a LExpBinOp");
  return LIB::BaseSBG(); 
}

LIB::BaseSBG EvalBaseSBG::operator()(AST::MDLExp v) const
{ 
  Util::ERROR("EvalBaseSBG: trying to evaluate a MDLExp");
  return LIB::BaseSBG(); 
}

LIB::BaseSBG EvalBaseSBG::operator()(AST::MDLExpBinOp v) const 
{
  Util::ERROR("EvalBaseSBG: trying to evaluate a MDLExpBinOp");
  return LIB::BaseSBG(); 
}

LIB::BaseSBG EvalBaseSBG::operator()(AST::LinearMap v) const 
{
  Util::ERROR("EvalBaseSBG: trying to evaluate a LinearMap");
  return LIB::BaseSBG(); 
}

LIB::BaseSBG EvalBaseSBG::operator()(AST::PWLMap v) const 
{
  Util::ERROR("EvalBaseSBG: trying to evaluate a PWLMap");
  return LIB::BaseSBG(); 
}

LIB::BaseSBG EvalBaseSBG::operator()(AST::SBG v) const 
{
  EvalUnordSet visit_set(env_);
  EvalBasePWMap visit_pw(env_);

  LIB::UnordSet V = Apply(visit_set, v.V());
  LIB::BasePWMap Vmap = Apply(visit_pw, v.Vmap());
  LIB::BasePWMap map1 = Apply(visit_pw, v.map1());
  LIB::BasePWMap map2 = Apply(visit_pw, v.map2());
  LIB::BasePWMap Emap = Apply(visit_pw, v.Emap());

  return LIB::BaseSBG(V, Vmap, map1, map2, Emap);
}

} // namespace Eval

} // namespace SBG
