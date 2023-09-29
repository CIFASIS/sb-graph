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

#include "eval/visitors/eval_base_pwmap.hpp"

namespace SBG {

namespace Eval {

EvalBasePWMap::EvalBasePWMap() : env_() {}
EvalBasePWMap::EvalBasePWMap(VarEnv env) : env_(env) {}

LIB::BasePWMap EvalBasePWMap::operator()(AST::Natural v) const
{
  Util::ERROR("EvalBasePWMap: trying to evaluate a Natural");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::MDNatural v) const
{
  Util::ERROR("EvalBasePWMap: trying to evaluate a MDNatural");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::Rational v) const
{
  Util::ERROR("EvalBasePWMap: trying to evaluate a Rational");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::Boolean v) const
{
  Util::ERROR("EvalBasePWMap: trying to evaluate a Boolean");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(Util::VariableName v) const
{
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (is<LIB::BasePWMap>(value))
      return boost::get<LIB::BasePWMap>(value);

    else {
      Util::ERROR("EvalBasePWMap: variable %s is not a PWMap", v.c_str());
      return LIB::BasePWMap(); 
    } 
  }

  Util::ERROR("EvalBasePWMap: variable %s not defined", v.c_str());
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::UnaryOp v) const 
{
  Util::ERROR("EvalBasePWMap: trying to evaluate an arithmetic BinOp");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::BinOp v) const 
{
  Util::ERROR("EvalBasePWMap: trying to evaluate an arithmetic BinOp");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::Call v) const
{
  Util::ERROR("EvalBasePWMap: trying to evaluate a Call");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::Interval v) const
{ 
  Util::ERROR("EvalBasePWMap: trying to evaluate an Interval");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalBasePWMap: trying to evaluate an InterUnaryOp");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalBasePWMap: trying to evaluate an InterBinOp");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::MultiDimInter v) const
{ 
  Util::ERROR("EvalBasePWMap: trying to evaluate an MultiDimInter");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::MDInterUnaryOp v) const
{
  Util::ERROR("EvalBasePWMap: trying to evaluate an MDInterUnaryOp");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::MDInterBinOp v) const
{
  Util::ERROR("EvalBasePWMap: trying to evaluate an MDInterBinOp");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::Set v) const 
{
  Util::ERROR("EvalBasePWMap: trying to evaluate a Set");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::SetUnaryOp v) const 
{
  Util::ERROR("EvalBasePWMap: trying to evaluate a SetUnaryOp");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::SetBinOp v) const 
{
  Util::ERROR("EvalBasePWMap: trying to evaluate a SetBinOp");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::LinearExp v) const
{ 
  Util::ERROR("EvalBasePWMap: trying to evaluate a LinearExp");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::LExpBinOp v) const 
{
  Util::ERROR("EvalBasePWMap: trying to evaluate a LExpBinOp");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::MDLExp v) const
{ 
  Util::ERROR("EvalBasePWMap: trying to evaluate a MDLExp");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::MDLExpBinOp v) const 
{
  Util::ERROR("EvalBasePWMap: trying to evaluate a MDLExpBinOp");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::LinearMap v) const 
{
  Util::ERROR("EvalBasePWMap: trying to evaluate a LinearMap");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::PWLMap v) const 
{
  LIB::BasePWMap res;

  EvalBaseMap visit_map(env_);
  BOOST_FOREACH (AST::Expr e, v.maps())
    res.emplace(Apply(visit_map, e));

  return LIB::BasePWMap(res); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::SBG v) const 
{
  Util::ERROR("EvalBasePWMap: trying to evaluate a SBG");
  return LIB::BasePWMap(); 
}

} // namespace Eval

} // namespace SBG
