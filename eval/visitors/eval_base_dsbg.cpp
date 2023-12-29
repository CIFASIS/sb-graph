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

#include "eval/visitors/eval_base_dsbg.hpp"

namespace SBG {

namespace Eval {

EvalBaseDSBG::EvalBaseDSBG() : env_() {}
EvalBaseDSBG::EvalBaseDSBG(VarEnv env) : env_(env) {}

LIB::BaseDSBG EvalBaseDSBG::operator()(AST::Natural v) const
{
  Util::ERROR("EvalBaseDSBG: trying to evaluate a Natural");
  return LIB::BaseDSBG(); 
}

LIB::BaseDSBG EvalBaseDSBG::operator()(AST::MDNatural v) const
{
  Util::ERROR("EvalBaseDSBG: trying to evaluate a MDNatural");
  return LIB::BaseDSBG(); 
}

LIB::BaseDSBG EvalBaseDSBG::operator()(AST::Rational v) const
{
  Util::ERROR("EvalBaseDSBG: trying to evaluate a Rational");
  return LIB::BaseDSBG(); 
}

LIB::BaseDSBG EvalBaseDSBG::operator()(AST::Boolean v) const
{
  Util::ERROR("EvalBaseDSBG: trying to evaluate a Boolean");
  return LIB::BaseDSBG(); 
}

LIB::BaseDSBG EvalBaseDSBG::operator()(Util::VariableName v) const
{
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (std::holds_alternative<SBGBaseType>(value)) {
      auto sbg = std::get<SBGBaseType>(value);
      if (std::holds_alternative<LIB::BaseDSBG>(sbg))
        return std::get<LIB::BaseDSBG>(sbg);
    }

    else {
      Util::ERROR("EvalBaseDSBG: variable %s is not a PWMap", v.c_str());
      return LIB::BaseDSBG(); 
    } 
  }

  Util::ERROR("EvalBaseDSBG: variable %s not defined", v.c_str());
  return LIB::BaseDSBG(); 
}

LIB::BaseDSBG EvalBaseDSBG::operator()(AST::UnaryOp v) const 
{
  Util::ERROR("EvalBaseDSBG: trying to evaluate an arithmetic BinOp");
  return LIB::BaseDSBG(); 
}

LIB::BaseDSBG EvalBaseDSBG::operator()(AST::BinOp v) const 
{
  Util::ERROR("EvalBaseDSBG: trying to evaluate an arithmetic BinOp");
  return LIB::BaseDSBG(); 
}

LIB::BaseDSBG EvalBaseDSBG::operator()(AST::Call v) const
{
  Util::ERROR("EvalBaseDSBG: trying to evaluate a Call");
  return LIB::BaseDSBG(); 
}

LIB::BaseDSBG EvalBaseDSBG::operator()(AST::Interval v) const
{ 
  Util::ERROR("EvalBaseDSBG: trying to evaluate an Interval");
  return LIB::BaseDSBG(); 
}

LIB::BaseDSBG EvalBaseDSBG::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalBaseDSBG: trying to evaluate an InterUnaryOp");
  return LIB::BaseDSBG(); 
}

LIB::BaseDSBG EvalBaseDSBG::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalBaseDSBG: trying to evaluate an InterBinOp");
  return LIB::BaseDSBG(); 
}

LIB::BaseDSBG EvalBaseDSBG::operator()(AST::MultiDimInter v) const
{ 
  Util::ERROR("EvalBaseDSBG: trying to evaluate an MultiDimInter");
  return LIB::BaseDSBG(); 
}

LIB::BaseDSBG EvalBaseDSBG::operator()(AST::MDInterUnaryOp v) const
{
  Util::ERROR("EvalBaseDSBG: trying to evaluate an MDInterUnaryOp");
  return LIB::BaseDSBG(); 
}

LIB::BaseDSBG EvalBaseDSBG::operator()(AST::MDInterBinOp v) const
{
  Util::ERROR("EvalBaseDSBG: trying to evaluate an MDInterBinOp");
  return LIB::BaseDSBG(); 
}

LIB::BaseDSBG EvalBaseDSBG::operator()(AST::Set v) const 
{
  Util::ERROR("EvalBaseDSBG: trying to evaluate a Set");
  return LIB::BaseDSBG(); 
}

LIB::BaseDSBG EvalBaseDSBG::operator()(AST::SetUnaryOp v) const 
{
  Util::ERROR("EvalBaseDSBG: trying to evaluate a SetUnaryOp");
  return LIB::BaseDSBG(); 
}

LIB::BaseDSBG EvalBaseDSBG::operator()(AST::SetBinOp v) const 
{
  Util::ERROR("EvalBaseDSBG: trying to evaluate a SetBinOp");
  return LIB::BaseDSBG(); 
}

LIB::BaseDSBG EvalBaseDSBG::operator()(AST::LinearExp v) const
{ 
  Util::ERROR("EvalBaseDSBG: trying to evaluate a LinearExp");
  return LIB::BaseDSBG(); 
}

LIB::BaseDSBG EvalBaseDSBG::operator()(AST::LExpBinOp v) const 
{
  Util::ERROR("EvalBaseDSBG: trying to evaluate a LExpBinOp");
  return LIB::BaseDSBG(); 
}

LIB::BaseDSBG EvalBaseDSBG::operator()(AST::MDLExp v) const
{ 
  Util::ERROR("EvalBaseDSBG: trying to evaluate a MDLExp");
  return LIB::BaseDSBG(); 
}

LIB::BaseDSBG EvalBaseDSBG::operator()(AST::MDLExpBinOp v) const 
{
  Util::ERROR("EvalBaseDSBG: trying to evaluate a MDLExpBinOp");
  return LIB::BaseDSBG(); 
}

LIB::BaseDSBG EvalBaseDSBG::operator()(AST::LinearMap v) const 
{
  Util::ERROR("EvalBaseDSBG: trying to evaluate a LinearMap");
  return LIB::BaseDSBG(); 
}

LIB::BaseDSBG EvalBaseDSBG::operator()(AST::PWLMap v) const 
{
  Util::ERROR("EvalBaseDSBG: trying to evaluate a PWLMap");
  return LIB::BaseDSBG(); 
}

LIB::BaseDSBG EvalBaseDSBG::operator()(AST::SBG v) const 
{
  Util::ERROR("EvalBaseDSBG: trying to evaluate a SBG");
  return LIB::BaseDSBG(); 
}

LIB::BaseDSBG EvalBaseDSBG::operator()(AST::DSBG v) const 
{
  EvalUnordSet visit_set(env_);
  EvalBasePWMap visit_pw(env_);

  LIB::UnordSet V = boost::apply_visitor(visit_set, v.V());
  LIB::BasePWMap Vmap = boost::apply_visitor(visit_pw, v.Vmap());
  LIB::BasePWMap mapB = boost::apply_visitor(visit_pw, v.mapB());
  LIB::BasePWMap mapD = boost::apply_visitor(visit_pw, v.mapD());
  LIB::BasePWMap Emap = boost::apply_visitor(visit_pw, v.Emap());

  return LIB::BaseDSBG(V, Vmap, mapB, mapD, Emap);
}

} // namespace Eval

} // namespace SBG
