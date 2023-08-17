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

#include "eval/visitors/eval_pwmap.hpp"

namespace SBG {

namespace Eval {

EvalPWMap::EvalPWMap() : env_() {}
EvalPWMap::EvalPWMap(VarEnv env) : env_(env) {}

LIB::PWMap EvalPWMap::operator()(AST::Natural v) const { 
  Util::ERROR("EvalPWMap: trying to evaluate a Natural");
  return LIB::PWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::Rational v) const { 
  Util::ERROR("EvalPWMap: trying to evaluate a Rational");
  return LIB::PWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::Boolean v) const { 
  Util::ERROR("EvalPWMap: trying to evaluate a Boolean");
  return LIB::PWMap(); 
}

LIB::PWMap EvalPWMap::operator()(Util::VariableName v) const { 
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (std::holds_alternative<LIB::PWMap>(value))
      return std::get<LIB::PWMap>(value);

    else {
      Util::ERROR("EvalPWMap: variable %s is not a PWMap", v.c_str());
      return LIB::PWMap(); 
    } 
  }

  Util::ERROR("EvalPWMap: variable %s not defined", v.c_str());
  return LIB::PWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::UnaryOp v) const 
{
  Util::ERROR("EvalPWMap: trying to evaluate an arithmetic UnaryOp");
  return LIB::PWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::BinOp v) const 
{
  Util::ERROR("EvalPWMap: trying to evaluate an arithmetic BinOp");
  return LIB::PWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::Call v) const
{
  Util::ERROR("EvalPWMap: trying to evaluate a Call");
  return LIB::PWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::Interval v) const
{ 
  Util::ERROR("EvalPWMap: trying to evaluate an Interval");
  return LIB::PWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalPWMap: trying to evaluate an InterUnaryOp");
  return LIB::PWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalPWMap: trying to evaluate an InterBinOp");
  return LIB::PWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::Set v) const 
{
  Util::ERROR("EvalPWMap: trying to evaluate a Set");
  return LIB::PWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::SetUnaryOp v) const 
{
  Util::ERROR("EvalPWMap: trying to evaluate a SetUnaryOp");
  return LIB::PWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::SetBinOp v) const 
{
  Util::ERROR("EvalPWMap: trying to evaluate a SetBinOp");
  return LIB::PWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::LinearExp le) const
{ 
  Util::ERROR("EvalPWMap: trying to evaluate a LinearExp");
  return LIB::PWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::LExpBinOp v) const 
{
  Util::ERROR("EvalPWMap: trying to evaluate a LExpBinOp");
  return LIB::PWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::LinearMap v) const 
{
  Util::ERROR("EvalPWMap: trying to evaluate a LinearMap");
  return LIB::PWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::PWLMap v) const 
{
  LIB::MapSet res;

  EvalMap visit_map(env_);
  BOOST_FOREACH (AST::Expr e, v.maps())
    res.emplace(Apply(visit_map, e));

  return LIB::PWMap(res); 
}

} // namespace Eval

} // namespace SBG
