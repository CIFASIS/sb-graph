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

EvalMap::EvalMap(unsigned int nmbr_dims, const LIB::PWMapAF &fact, VarEnv &env)
  : nmbr_dims_(nmbr_dims), fact_(fact), env_(env) {}

LIB::Map EvalMap::operator()(AST::Natural v) const
{
  Debug::ERROR("EvalMap: trying to evaluate Natural ", v, "\n");
  return fact_.createMap(); 
}

LIB::Map EvalMap::operator()(AST::Rational v) const
{
  Debug::ERROR("EvalMap: trying to evaluate Rational ", v, "\n");
  return fact_.createMap(); 
}

LIB::Map EvalMap::operator()(AST::VariableName v) const
{
  Debug::ERROR("EvalMap: variable ", v, " is not a Map\n");
  return fact_.createMap(); 
}

LIB::Map EvalMap::operator()(AST::UnaryOp v) const 
{
  Debug::ERROR("EvalMap: trying to evaluate arithmetic UnaryOp ", v, "\n");
  return fact_.createMap(); 
}

LIB::Map EvalMap::operator()(AST::BinOp v) const 
{
  Debug::ERROR("EvalMap: trying to evaluate arithmetic BinOp ", v, "\n");
  return fact_.createMap(); 
}

LIB::Map EvalMap::operator()(AST::Call v) const
{
  Debug::ERROR("EvalMap: trying to evaluate Call ", v, "\n");
  return fact_.createMap(); 
}

LIB::Map EvalMap::operator()(AST::Interval v) const
{ 
  Debug::ERROR("EvalMap: trying to evaluate Interval ", v, "\n");
  return fact_.createMap(); 
}

LIB::Map EvalMap::operator()(AST::InterUnaryOp v) const
{
  Debug::ERROR("EvalMap: trying to evaluate InterUnaryOp ", v, "\n");
  return fact_.createMap(); 
}

LIB::Map EvalMap::operator()(AST::InterBinOp v) const
{
  Debug::ERROR("EvalMap: trying to evaluate InterBinOp ", v, "\n");
  return fact_.createMap(); 
}

LIB::Map EvalMap::operator()(AST::MultiDimInter v) const
{ 
  Debug::ERROR("EvalMap: trying to evaluate MultiDimInter ", v, "\n");
  return fact_.createMap(); 
}

LIB::Map EvalMap::operator()(AST::MDInterUnaryOp v) const
{
  Debug::ERROR("EvalMap: trying to evaluate MDInterUnaryOp ", v, "\n");
  return fact_.createMap(); 
}

LIB::Map EvalMap::operator()(AST::MDInterBinOp v) const
{
  Debug::ERROR("EvalMap: trying to evaluate MDInterBinOp ", v, "\n");
  return fact_.createMap(); 
}

LIB::Map EvalMap::operator()(AST::Set v) const 
{
  Debug::ERROR("EvalMap: trying to evaluate Set ", v, "\n");
  return fact_.createMap(); 
}

LIB::Map EvalMap::operator()(AST::SetUnaryOp v) const 
{
  Debug::ERROR("EvalMap: trying to evaluate SetUnaryOp ", v, "\n");
  return fact_.createMap(); 
}

LIB::Map EvalMap::operator()(AST::SetBinOp v) const 
{
  Debug::ERROR("EvalMap: trying to evaluate SetBinOp ", v, "\n");
  return fact_.createMap(); 
}

LIB::Map EvalMap::operator()(AST::LinearExp v) const
{ 
  Debug::ERROR("EvalMap: trying to evaluate LinearExp ", v, "\n");
  return fact_.createMap(); 
}

LIB::Map EvalMap::operator()(AST::LExpBinOp v) const
{ 
  Debug::ERROR("EvalMap: trying to evaluate LExpBinOp ", v, "\n");
  return fact_.createMap(); 
}

LIB::Map EvalMap::operator()(AST::MDLExp v) const
{ 
  Debug::ERROR("EvalMap: trying to evaluate MDLExp ", v, "\n");
  return fact_.createMap(); 
}

LIB::Map EvalMap::operator()(AST::MDLExpBinOp v) const
{ 
  Debug::ERROR("EvalMap: trying to evaluate MDLExpBinOp ", v, "\n");
  return fact_.createMap(); 
}

LIB::Map EvalMap::operator()(AST::LinearMap v) const
{ 
  EvalSet visit_set(nmbr_dims_, fact_, env_);
  LIB::Set d = boost::apply_visitor(visit_set, v.dom());
  EvalMDLE visit_mdle(nmbr_dims_, env_);
  LIB::Exp e = boost::apply_visitor(visit_mdle, v.lexp());

  return fact_.createMap(d, e);
}

LIB::Map EvalMap::operator()(AST::PWLMap v) const
{ 
  Debug::ERROR("EvalMap: trying to evaluate PWLMap ", v, "\n");
  return fact_.createMap(); 
}

LIB::Map EvalMap::operator()(AST::SBG v) const
{ 
  Debug::ERROR("EvalMap: trying to evaluate SBG ", v, "\n");
  return fact_.createMap(); 
}

LIB::Map EvalMap::operator()(AST::DSBG v) const
{ 
  Debug::ERROR("EvalMap: trying to evaluate DSBG ", v, "\n");
  return fact_.createMap(); 
}

} // namespace Eval

} // namespace SBG
