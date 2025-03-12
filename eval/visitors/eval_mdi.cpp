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

#include "eval/visitors/eval_mdi.hpp"

namespace SBG {

namespace Eval {

EvalMDI::EvalMDI(unsigned int nmbr_dims, VarEnv &env)
  : nmbr_dims_(nmbr_dims), env_(env) {}

LIB::MultiDimInter EvalMDI::operator()(AST::Natural v) const 
{ 
  Debug::ERROR("EvalMDI: trying to evaluate Natural ", v, "\n");
  return LIB::MultiDimInter();
}

LIB::MultiDimInter EvalMDI::operator()(AST::Rational v) const 
{ 
  Debug::ERROR("EvalMDI: trying to evaluate Rational ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::VariableName v) const 
{ 
  Debug::ERROR("EvalMDI: variable ", v, " is not a MDI\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::UnaryOp v) const 
{
  Debug::ERROR("EvalMDI: trying to evaluate arithmetic UnaryOp ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::BinOp v) const 
{
  Debug::ERROR("EvalMDI: trying to evaluate arithmetic BinOp ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::Call v) const
{
  Debug::ERROR("EvalMDI: trying to evaluate Call ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::Interval v) const 
{ 
  Debug::ERROR("EvalMDI: trying to evaluate Interval ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::InterUnaryOp v) const
{
  Debug::ERROR("EvalMDI: trying to evaluate InterUnaryOp ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::InterBinOp v) const
{
  Debug::ERROR("EvalMDI: trying to evaluate InterBinOp ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::MultiDimInter v) const 
{
  LIB::MultiDimInter res;

  EvalInterval visit_inter(env_);
  for (AST::Expr ith : v.intervals())
    res.emplaceBack(boost::apply_visitor(visit_inter, ith));

  Debug::ERROR_UNLESS(res.arity() == nmbr_dims_ || res.arity() == 0
    , "EvalMDI[nmbr_dims = ", nmbr_dims_, "]: arity(", res, ") = "
    , res.arity(), "\n");

  return res;
}

LIB::MultiDimInter EvalMDI::operator()(AST::MDInterUnaryOp v) const 
{
  AST::Expr exp = v.e();
  switch (v.op()) {
    default:
      Debug::ERROR("EvalMDI: MDInterUnaryOp ", v.op(), " unsupported\n");
      return LIB::MultiDimInter(); 
  }
}

LIB::MultiDimInter EvalMDI::operator()(AST::MDInterBinOp v) const 
{
  LIB::MultiDimInter l = boost::apply_visitor(*this, v.left());
  LIB::MultiDimInter r = boost::apply_visitor(*this, v.right());
  switch (v.op()) {
    case AST::ContainerOp::cap:
      return l.intersection(r);

    default:
      Debug::ERROR("EvalMD: MDInterBinOp ", v.op(), " unsupported\n");
      return LIB::MultiDimInter(); 
  }
}

LIB::MultiDimInter EvalMDI::operator()(AST::Set v) const 
{
  Debug::ERROR("EvalMDI: trying to evaluate Set ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::SetUnaryOp v) const 
{
  Debug::ERROR("EvalMDI: trying to evaluate SetUnaryOp ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::SetBinOp v) const 
{
  Debug::ERROR("EvalMDI: trying to evaluate SetBinOp ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::LinearExp v) const 
{
  Debug::ERROR("EvalMDI: trying to evaluate LinearExp ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::LExpBinOp v) const 
{
  Debug::ERROR("EvalMDI: trying to evaluate LExpBinOp ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::MDLExp v) const 
{
  Debug::ERROR("EvalMDI: trying to evaluate MDLExp ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::MDLExpBinOp v) const 
{
  Debug::ERROR("EvalMDI: trying to evaluate MDLExpBinOp ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::LinearMap v) const 
{
  Debug::ERROR("EvalMDI: trying to evaluate LinearMap ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::PWLMap v) const 
{
  Debug::ERROR("EvalMDI: trying to evaluate PWLMap ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::SBG v) const 
{
  Debug::ERROR("EvalMDI: trying to evaluate SBG ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::DSBG v) const 
{
  Debug::ERROR("EvalMDI: trying to evaluate DSBG ", v, "\n");
  return LIB::MultiDimInter(); 
}

} // namespace Eval

} // namespace SBG
