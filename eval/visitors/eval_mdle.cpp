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

#include "eval/visitors/eval_mdle.hpp"

namespace SBG {

namespace Eval {

EvalMDLE::EvalMDLE(unsigned int nmbr_dims, VarEnv &env)
  : nmbr_dims_(nmbr_dims), env_(env) {}

LIB::Exp EvalMDLE::operator()(AST::Natural v) const
{
  Debug::ERROR("EvalMDLE: trying to evaluate Natural ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::Rational v) const
{
  Debug::ERROR("EvalMDLE: trying to evaluate Rational ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::VariableName v) const
{
  Debug::ERROR("EvalMDLE: variable ", v, " is not a MDLE\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::UnaryOp v) const
{
  Debug::ERROR("EvalMDLE: trying to evaluate arithmetic UnaryOp ", v , "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::BinOp v) const 
{
  Debug::ERROR("EvalMDLE: trying to evaluate arithmetic BinOp ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::Call v) const
{
  Debug::ERROR("EvalMDLE: trying to evaluate Call ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::Interval v) const
{ 
  Debug::ERROR("EvalMDLE: trying to evaluate Interval ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::InterUnaryOp v) const
{
  Debug::ERROR("EvalMDLE: trying to evaluate InterUnaryOp ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::InterBinOp v) const
{
  Debug::ERROR("EvalMDLE: trying to evaluate InterBinOp ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::MultiDimInter v) const
{ 
  Debug::ERROR("EvalMDLE: trying to evaluate MultiDimInter ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::MDInterUnaryOp v) const
{
  Debug::ERROR("EvalMDLE: trying to evaluate MDInterUnaryOp ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::MDInterBinOp v) const
{
  Debug::ERROR("EvalMDLE: trying to evaluate MDInterBinOp ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::Set v) const 
{
  Debug::ERROR("EvalMDLE: trying to evaluate Set ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::SetUnaryOp v) const 
{
  Debug::ERROR("EvalMDLE: trying to evaluate SetUnaryOp ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::SetBinOp v) const 
{
  Debug::ERROR("EvalMDLE: trying to evaluate SetBinOp ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::LinearExp v) const
{
  Debug::ERROR("EvalMDLE: trying to evaluate LinearExp ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::LExpBinOp v) const 
{
  Debug::ERROR("EvalMDLE: trying to evaluate LExpBinOp ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::MDLExp v) const
{ 
  LIB::Exp res;

  EvalLE visit_le(env_);
  for (AST::Expr le : v.exps())
    res.emplaceBack(boost::apply_visitor(visit_le, le));

  Debug::ERROR_UNLESS(res.arity() == nmbr_dims_ || res.arity() == 0
    , "EvalMDLE[nmbr_dims = ", nmbr_dims_, "]: arity(", res, ") = "
    , res.arity(), "\n");

  return res;
}

LIB::Exp EvalMDLE::operator()(AST::MDLExpBinOp v) const 
{
  LIB::Exp l = boost::apply_visitor(*this, v.left());
  LIB::Exp r = boost::apply_visitor(*this, v.right());
  switch (v.op()) {
    case AST::ExpOp::add:
      return l + r;

    case AST::ExpOp::sub:
      return l - r;

    default:
      Debug::ERROR("EvalMDLE: LExpBinOp ", v.op(), " unsupported\n");
      return LIB::Exp(); 
  }
}

LIB::Exp EvalMDLE::operator()(AST::LinearMap v) const 
{
  Debug::ERROR("EvalMDLE: trying to evaluate LinearMap ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::PWLMap v) const 
{
  Debug::ERROR("EvalMDLE: trying to evaluate PWLMap ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::SBG v) const 
{
  Debug::ERROR("EvalMDLE: trying to evaluate SBG ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::DSBG v) const 
{
  Debug::ERROR("EvalMDLE: trying to evaluate DSBG ", v, "\n");
  return LIB::Exp(); 
}

} // namespace Eval

} // namespace SBG
