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

#include "eval/visitors/eval_map.hpp"

namespace SBG {

namespace Eval {

EvalMap::EvalMap() : env_() {}
EvalMap::EvalMap(VarEnv env) : env_(env) {}

LIB::SBGMap EvalMap::operator()(AST::Natural v) const { 
  Util::ERROR("EvalMap: trying to evaluate a Natural");
  return LIB::SBGMap(); 
}

LIB::SBGMap EvalMap::operator()(AST::Rational v) const { 
  Util::ERROR("EvalMap: trying to evaluate a Rational");
  return LIB::SBGMap(); 
}

LIB::SBGMap EvalMap::operator()(AST::Boolean v) const { 
  Util::ERROR("EvalMap: trying to evaluate a Boolean");
  return LIB::SBGMap(); 
}

LIB::SBGMap EvalMap::operator()(Util::VariableName v) const { 
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (std::holds_alternative<LIB::SBGMap>(value))
      return std::get<LIB::SBGMap>(value);

    else {
      Util::ERROR("EvalMap: variable %s is not a linear expression", v.c_str());
      return LIB::SBGMap(); 
    } 
  }

  Util::ERROR("EvalMap: variable %s not defined", v.c_str());
  return LIB::SBGMap(); 
}

LIB::SBGMap EvalMap::operator()(AST::UnaryOp v) const 
{
  Util::ERROR("EvalMap: trying to evaluate an arithmetic UnaryOp");
  return LIB::SBGMap(); 
}

LIB::SBGMap EvalMap::operator()(AST::BinOp v) const 
{
  Util::ERROR("EvalMap: trying to evaluate an arithmetic BinOp");
  return LIB::SBGMap(); 
}

LIB::SBGMap EvalMap::operator()(AST::Call v) const
{
  Util::ERROR("EvalMap: trying to evaluate a Call");
  return LIB::SBGMap(); 
}

LIB::SBGMap EvalMap::operator()(AST::Interval v) const
{ 
  Util::ERROR("EvalMap: trying to evaluate an Interval");
  return LIB::SBGMap(); 
}

LIB::SBGMap EvalMap::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalMap: trying to evaluate an InterUnaryOp");
  return LIB::SBGMap(); 
}

LIB::SBGMap EvalMap::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalMap: trying to evaluate an InterBinOp");
  return LIB::SBGMap(); 
}

LIB::SBGMap EvalMap::operator()(AST::Set v) const 
{
  Util::ERROR("EvalMap: trying to evaluate a Set");
  return LIB::SBGMap(); 
}

LIB::SBGMap EvalMap::operator()(AST::SetUnaryOp v) const 
{
  Util::ERROR("EvalMap: trying to evaluate a SetUnaryOp");
  return LIB::SBGMap(); 
}

LIB::SBGMap EvalMap::operator()(AST::SetBinOp v) const 
{
  Util::ERROR("EvalMap: trying to evaluate a SetBinOp");
  return LIB::SBGMap(); 
}

LIB::SBGMap EvalMap::operator()(AST::LinearExp le) const
{ 
  Util::ERROR("EvalMap: trying to evaluate a LinearExp");
  return LIB::SBGMap(); 
}

LIB::SBGMap EvalMap::operator()(AST::LExpBinOp v) const 
{
  Util::ERROR("EvalMap: trying to evaluate a LExpBinOp");
  return LIB::SBGMap(); 
}

LIB::SBGMap EvalMap::operator()(AST::LinearMap v) const 
{
  EvalSet visit_set(env_);
  EvalLE visit_le(env_);

  return LIB::SBGMap(Apply(visit_set, v.dom()), Apply(visit_le, v.lexp()));
}

LIB::SBGMap EvalMap::operator()(AST::PWLMap v) const 
{
  Util::ERROR("EvalMap: trying to evaluate a PWLMap");
  return LIB::SBGMap(); 
}

} // namespace Eval

} // namespace SBG
