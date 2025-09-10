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

#include "eval/visitors/eval_pw.hpp"

namespace SBG {

namespace Eval {

EvalPWMap::EvalPWMap(unsigned int nmbr_dims, const LIB::PWMapFact &fact
  , VarEnv &env)
  : nmbr_dims_(nmbr_dims), fact_(fact), env_(env) {}

LIB::PWMap EvalPWMap::operator()(AST::Natural v) const
{
  Util::ERROR("EvalPWMap: trying to evaluate Natural ", v, "\n");
  return fact_.createPWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::Rational v) const
{
  Util::ERROR("EvalPWMap: trying to evaluate Rational ", v, "\n");
  return fact_.createPWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::VariableName v) const
{
  Util::ERROR("EvalPWMap: variable ", v, " is not a PWMap\n");
  return fact_.createPWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::UnaryOp v) const 
{
  Util::ERROR("EvalPWMap: trying to evaluate arithmetic UnaryOp ", v, "\n");
  return fact_.createPWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::BinOp v) const 
{
  Util::ERROR("EvalPWMap: trying to evaluate arithmetic BinOp ", v, "\n");
  return fact_.createPWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::Call v) const
{
  Util::ERROR("EvalPWMap: trying to evaluate Call ", v, "\n");
  return fact_.createPWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::Interval v) const
{ 
  Util::ERROR("EvalPWMap: trying to evaluate Interval ", v, "\n");
  return fact_.createPWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalPWMap: trying to evaluate InterUnaryOp ", v, "\n");
  return fact_.createPWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalPWMap: trying to evaluate InterBinOp ", v, "\n");
  return fact_.createPWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::MultiDimInter v) const
{ 
  Util::ERROR("EvalPWMap: trying to evaluate MultiDimInter ", v, "\n");
  return fact_.createPWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::MDInterUnaryOp v) const
{
  Util::ERROR("EvalPWMap: trying to evaluate MDInterUnaryOp ", v, "\n");
  return fact_.createPWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::MDInterBinOp v) const
{
  Util::ERROR("EvalPWMap: trying to evaluate MDInterBinOp ", v, "\n");
  return fact_.createPWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::Set v) const 
{
  Util::ERROR("EvalPWMap: trying to evaluate Set ", v, "\n");
  return fact_.createPWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::SetUnaryOp v) const 
{
  Util::ERROR("EvalPWMap: trying to evaluate SetUnaryOp ", v, "\n");
  return fact_.createPWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::SetBinOp v) const 
{
  Util::ERROR("EvalPWMap: trying to evaluate SetBinOp ", v, "\n");
  return fact_.createPWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::LinearExp v) const
{ 
  Util::ERROR("EvalPWMap: trying to evaluate LinearExp ", v, "\n");
  return fact_.createPWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::LExpBinOp v) const
{ 
  Util::ERROR("EvalPWMap: trying to evaluate LExpBinOp ", v, "\n");
  return fact_.createPWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::MDLExp v) const
{ 
  Util::ERROR("EvalPWMap: trying to evaluate MDLExp ", v, "\n");
  return fact_.createPWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::MDLExpBinOp v) const
{ 
  Util::ERROR("EvalPWMap: trying to evaluate MDLExpBinOp ", v, "\n");
  return fact_.createPWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::LinearMap v) const
{ 
  Util::ERROR("EvalPWMap: trying to evaluate LinearMap ", v, "\n");
  return fact_.createPWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::PWLMap v) const
{ 
  LIB::PWMap res = fact_.createPWMap();

  EvalMap visit_map(nmbr_dims_, fact_, env_);
  for (AST::Expr e : v.maps())
    res.emplaceBack(boost::apply_visitor(visit_map, e));

  Util::ERROR_UNLESS(res.arity() == nmbr_dims_ || res.arity() == 0
    , "EvalPWMap[nmbr_dims = ", nmbr_dims_, "]: arity(", res, ") = "
    , res.arity(), "\n");

  return res; 
}

LIB::PWMap EvalPWMap::operator()(AST::SBG v) const
{ 
  Util::ERROR("EvalPWMap: trying to evaluate SBG ", v, "\n");
  return fact_.createPWMap(); 
}

LIB::PWMap EvalPWMap::operator()(AST::DSBG v) const
{ 
  Util::ERROR("EvalPWMap: trying to evaluate DSBG ", v, "\n");
  return fact_.createPWMap(); 
}

} // namespace Eval

} // namespace SBG
