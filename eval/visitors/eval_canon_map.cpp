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

#include "eval/visitors/eval_canon_map.hpp"

namespace SBG {

namespace Eval {

EvalCanonMap::EvalCanonMap() : env_() {}
EvalCanonMap::EvalCanonMap(VarEnv env) : env_(env) {}

LIB::CanonMap EvalCanonMap::operator()(AST::Natural v) const
{
  Util::ERROR("EvalCanonMap: trying to evaluate a Natural");
  return LIB::CanonMap(); 
}

LIB::CanonMap EvalCanonMap::operator()(AST::MDNatural v) const
{
  Util::ERROR("EvalCanonMap: trying to evaluate a MDNatural");
  return LIB::CanonMap(); 
}

LIB::CanonMap EvalCanonMap::operator()(AST::Rational v) const
{
  Util::ERROR("EvalCanonMap: trying to evaluate a Rational");
  return LIB::CanonMap(); 
}

LIB::CanonMap EvalCanonMap::operator()(AST::Boolean v) const
{
  Util::ERROR("EvalCanonMap: trying to evaluate a Boolean");
  return LIB::CanonMap(); 
}

LIB::CanonMap EvalCanonMap::operator()(Util::VariableName v) const
{
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (is<LIB::CanonMap>(value))
      return boost::get<LIB::CanonMap>(value);

    else {
      Util::ERROR("EvalCanonMap: variable %s is not a linear expression", v.c_str());
      return LIB::CanonMap(); 
    } 
  }

  Util::ERROR("EvalCanonMap: variable %s not defined", v.c_str());
  return LIB::CanonMap(); 
}

LIB::CanonMap EvalCanonMap::operator()(AST::UnaryOp v) const 
{
  Util::ERROR("EvalCanonMap: trying to evaluate an arithmetic UnaryOp");
  return LIB::CanonMap(); 
}

LIB::CanonMap EvalCanonMap::operator()(AST::BinOp v) const 
{
  Util::ERROR("EvalCanonMap: trying to evaluate an arithmetic BinOp");
  return LIB::CanonMap(); 
}

LIB::CanonMap EvalCanonMap::operator()(AST::Call v) const
{
  Util::ERROR("EvalCanonMap: trying to evaluate a Call");
  return LIB::CanonMap(); 
}

LIB::CanonMap EvalCanonMap::operator()(AST::Interval v) const
{ 
  Util::ERROR("EvalCanonMap: trying to evaluate an Interval");
  return LIB::CanonMap(); 
}

LIB::CanonMap EvalCanonMap::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalCanonMap: trying to evaluate an InterUnaryOp");
  return LIB::CanonMap(); 
}

LIB::CanonMap EvalCanonMap::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalCanonMap: trying to evaluate an InterBinOp");
  return LIB::CanonMap(); 
}

LIB::CanonMap EvalCanonMap::operator()(AST::MultiDimInter v) const
{ 
  Util::ERROR("EvalCanonMap: trying to evaluate an MultiDimInter");
  return LIB::CanonMap(); 
}

LIB::CanonMap EvalCanonMap::operator()(AST::MDInterUnaryOp v) const
{
  Util::ERROR("EvalCanonMap: trying to evaluate an MDInterUnaryOp");
  return LIB::CanonMap(); 
}

LIB::CanonMap EvalCanonMap::operator()(AST::MDInterBinOp v) const
{
  Util::ERROR("EvalCanonMap: trying to evaluate an MDInterBinOp");
  return LIB::CanonMap(); 
}

LIB::CanonMap EvalCanonMap::operator()(AST::Set v) const 
{
  Util::ERROR("EvalCanonMap: trying to evaluate a Set");
  return LIB::CanonMap(); 
}

LIB::CanonMap EvalCanonMap::operator()(AST::SetUnaryOp v) const 
{
  Util::ERROR("EvalCanonMap: trying to evaluate a SetUnaryOp");
  return LIB::CanonMap(); 
}

LIB::CanonMap EvalCanonMap::operator()(AST::SetBinOp v) const 
{
  Util::ERROR("EvalCanonMap: trying to evaluate a SetBinOp");
  return LIB::CanonMap(); 
}

LIB::CanonMap EvalCanonMap::operator()(AST::LinearExp v) const
{ 
  Util::ERROR("EvalCanonMap: trying to evaluate a LinearExp");
  return LIB::CanonMap(); 
}

LIB::CanonMap EvalCanonMap::operator()(AST::LExpBinOp v) const 
{
  Util::ERROR("EvalCanonMap: trying to evaluate a LExpBinOp");
  return LIB::CanonMap(); 
}

LIB::CanonMap EvalCanonMap::operator()(AST::MDLExp v) const
{ 
  Util::ERROR("EvalCanonMap: trying to evaluate a MDLExp");
  return LIB::CanonMap(); 
}

LIB::CanonMap EvalCanonMap::operator()(AST::MDLExpBinOp v) const 
{
  Util::ERROR("EvalCanonMap: trying to evaluate a MDLExpBinOp");
  return LIB::CanonMap(); 
}

LIB::CanonMap EvalCanonMap::operator()(AST::LinearMap v) const 
{
  EvalOrdSet visit_set(env_);
  EvalMDLE visit_mdle(env_);

  return LIB::CanonMap(Apply(visit_set, v.dom()), Apply(visit_mdle, v.lexp()));
}

LIB::CanonMap EvalCanonMap::operator()(AST::PWLMap v) const 
{
  Util::ERROR("EvalCanonMap: trying to evaluate a PWLMap");
  return LIB::CanonMap(); 
}

LIB::CanonMap EvalCanonMap::operator()(AST::SBG v) const 
{
  Util::ERROR("EvalCanonMap: trying to evaluate a SBG");
  return LIB::CanonMap(); 
}

} // namespace Eval

} // namespace SBG
