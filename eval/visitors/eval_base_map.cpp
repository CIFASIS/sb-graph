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

#include "eval/visitors/eval_base_map.hpp"

namespace SBG {

namespace Eval {

EvalBaseMap::EvalBaseMap() : env_() {}
EvalBaseMap::EvalBaseMap(VarEnv env) : env_(env) {}

LIB::BaseMap EvalBaseMap::operator()(AST::Natural v) const
{ 
  Util::ERROR("EvalBaseMap: trying to evaluate a Natural");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::MDNatural v) const
{ 
  Util::ERROR("EvalBaseMap: trying to evaluate a MDNatural");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::Rational v) const
{ 
  Util::ERROR("EvalBaseMap: trying to evaluate a Rational");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::Boolean v) const
{ 
  Util::ERROR("EvalBaseMap: trying to evaluate a Boolean");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(Util::VariableName v) const
{
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (is<LIB::BaseMap>(value))
      return boost::get<LIB::BaseMap>(value);

    else {
      Util::ERROR("EvalBaseMap: variable %s is not a linear expression", v.c_str());
      return LIB::BaseMap(); 
    } 
  }

  Util::ERROR("EvalBaseMap: variable %s not defined", v.c_str());
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::UnaryOp v) const 
{
  Util::ERROR("EvalBaseMap: trying to evaluate an arithmetic UnaryOp");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::BinOp v) const 
{
  Util::ERROR("EvalBaseMap: trying to evaluate an arithmetic BinOp");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::Call v) const
{
  Util::ERROR("EvalBaseMap: trying to evaluate a Call");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::Interval v) const
{ 
  Util::ERROR("EvalBaseMap: trying to evaluate an Interval");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalBaseMap: trying to evaluate an InterUnaryOp");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalBaseMap: trying to evaluate an InterBinOp");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::MultiDimInter v) const
{ 
  Util::ERROR("EvalBaseMap: trying to evaluate an MultiDimInter");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::MDInterUnaryOp v) const
{
  Util::ERROR("EvalBaseMap: trying to evaluate an MDInterUnaryOp");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::MDInterBinOp v) const
{
  Util::ERROR("EvalBaseMap: trying to evaluate an MDInterBinOp");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::Set v) const 
{
  Util::ERROR("EvalBaseMap: trying to evaluate a Set");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::SetUnaryOp v) const 
{
  Util::ERROR("EvalBaseMap: trying to evaluate a SetUnaryOp");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::SetBinOp v) const 
{
  Util::ERROR("EvalBaseMap: trying to evaluate a SetBinOp");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::LinearExp v) const
{ 
  Util::ERROR("EvalBaseMap: trying to evaluate a LinearExp");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::LExpBinOp v) const 
{
  Util::ERROR("EvalBaseMap: trying to evaluate a LExpBinOp");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::MDLExp le) const
{ 
  Util::ERROR("EvalBaseMap: trying to evaluate a MDLExp");
  return LIB::BaseMap();
}

LIB::BaseMap EvalBaseMap::operator()(AST::MDLExpBinOp v) const 
{
  Util::ERROR("EvalBaseMap: trying to evaluate a MDLExpBinOp");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::LinearMap v) const 
{
  EvalUnordSet visit_set(env_);
  EvalMDLE visit_mdle(env_);

  return LIB::BaseMap(Apply(visit_set, v.dom()), Apply(visit_mdle, v.lexp()));
}

LIB::BaseMap EvalBaseMap::operator()(AST::PWLMap v) const 
{
  Util::ERROR("EvalBaseMap: trying to evaluate a PWLMap");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::SBG v) const 
{
  Util::ERROR("EvalBaseMap: trying to evaluate a SBG");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::DSBG v) const 
{
  Util::ERROR("EvalBaseMap: trying to evaluate a DSBG");
  return LIB::BaseMap(); 
}

} // namespace Eval

} // namespace SBG
