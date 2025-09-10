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

#include "eval/visitors/eval_set.hpp"

namespace SBG {

namespace Eval {

EvalSet::EvalSet(unsigned int nmbr_dims, const LIB::PWMapFact &fact, VarEnv &env)
  : nmbr_dims_(nmbr_dims), fact_(fact), env_(env) {}

LIB::Set EvalSet::operator()(AST::Natural v) const
{
  Util::ERROR("EvalSet: trying to evaluate Natural ", v, "\n");
  return fact_.createSet(); 
}

LIB::Set EvalSet::operator()(AST::Rational v) const
{
  Util::ERROR("EvalSet: trying to evaluate Rational ", v, "\n");
  return fact_.createSet(); 
}

LIB::Set EvalSet::operator()(AST::VariableName v) const
{
  Util::ERROR("EvalSet: variable ", v, " is not a Set\n");
  return fact_.createSet(); 
}

LIB::Set EvalSet::operator()(AST::UnaryOp v) const 
{
  Util::ERROR("EvalSet: trying to evaluate arithmetic UnaryOp ", v, "\n");
  return fact_.createSet(); 
}

LIB::Set EvalSet::operator()(AST::BinOp v) const 
{
  Util::ERROR("EvalSet: trying to evaluate arithmetic BinOp ", v, "\n");
  return fact_.createSet(); 
}

LIB::Set EvalSet::operator()(AST::Call v) const
{
  Util::ERROR("EvalSet: trying to evaluate Call ", v, "\n");
  return fact_.createSet(); 
}

LIB::Set EvalSet::operator()(AST::Interval v) const
{ 
  Util::ERROR("EvalSet: trying to evaluate Interval ", v, "\n");
  return fact_.createSet(); 
}

LIB::Set EvalSet::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalSet: trying to evaluate InterUnaryOp ", v, "\n");
  return fact_.createSet(); 
}

LIB::Set EvalSet::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalSet: trying to evaluate InterBinOp ", v, "\n");
  return fact_.createSet(); 
}

LIB::Set EvalSet::operator()(AST::MultiDimInter v) const
{ 
  Util::ERROR("EvalSet: trying to evaluate MultiDimInter ", v, "\n");
  return fact_.createSet(); 
}

LIB::Set EvalSet::operator()(AST::MDInterUnaryOp v) const
{
  Util::ERROR("EvalSet: trying to evaluate MDInterUnaryOp ", v, "\n");
  return fact_.createSet(); 
}

LIB::Set EvalSet::operator()(AST::MDInterBinOp v) const
{
  Util::ERROR("EvalSet: trying to evaluate MDInterBinOp ", v, "\n");
  return fact_.createSet(); 
}

LIB::Set EvalSet::operator()(AST::Set v) const 
{
  LIB::Set res = fact_.createSet();

  EvalMDI mdi_visit(nmbr_dims_, env_);
  for (AST::Expr e : v.pieces())
    res.emplaceBack(boost::apply_visitor(mdi_visit, e));

  Util::ERROR_UNLESS(res.arity() == nmbr_dims_ || res.arity() == 0
    , "EvalSet[nmbr_dims = ", nmbr_dims_, "]: arity(", res, ") = "
    , res.arity(), "\n");

  return res;
}

LIB::Set EvalSet::operator()(AST::SetUnaryOp v) const 
{
  AST::Expr e = v.e();
  switch (v.op()) {
    default:
      Util::ERROR("EvalSet: SetUnaryOp ", v.op(), " unsupported\n");
      return fact_.createSet(); 
  }
}

LIB::Set EvalSet::operator()(AST::SetBinOp v) const 
{
  LIB::Set l = boost::apply_visitor(*this, v.left());
  LIB::Set r = boost::apply_visitor(*this, v.right());
  switch (v.op()) {
    case AST::ContainerOp::cap:
      return l.intersection(r);

    case AST::ContainerOp::diff:
      return l.difference(r);

    default:
      Util::ERROR("EvalSet: SetBinOp ", v.op(), " unsupported\n");
      return fact_.createSet(); 
  }

  return fact_.createSet();
}

LIB::Set EvalSet::operator()(AST::LinearExp v) const
{ 
  Util::ERROR("EvalSet: trying to evaluate LinearExp ", v, "\n");
  return fact_.createSet(); 
}

LIB::Set EvalSet::operator()(AST::LExpBinOp v) const
{ 
  Util::ERROR("EvalSet: trying to evaluate LExpBinOp ", v, "\n");
  return fact_.createSet(); 
}

LIB::Set EvalSet::operator()(AST::MDLExp v) const
{ 
  Util::ERROR("EvalSet: trying to evaluate MDLExp ", v, "\n");
  return fact_.createSet(); 
}

LIB::Set EvalSet::operator()(AST::MDLExpBinOp v) const
{ 
  Util::ERROR("EvalSet: trying to evaluate MDLExpBinOp ", v, "\n");
  return fact_.createSet(); 
}

LIB::Set EvalSet::operator()(AST::LinearMap v) const
{ 
  Util::ERROR("EvalSet: trying to evaluate LinearMap ", v, "\n");
  return fact_.createSet(); 
}

LIB::Set EvalSet::operator()(AST::PWLMap v) const
{ 
  Util::ERROR("EvalSet: trying to evaluate PWLMap ", v, "\n");
  return fact_.createSet(); 
}

LIB::Set EvalSet::operator()(AST::SBG v) const
{ 
  Util::ERROR("EvalSet: trying to evaluate SBG ", v, "\n");
  return fact_.createSet(); 
}

LIB::Set EvalSet::operator()(AST::DSBG v) const
{ 
  Util::ERROR("EvalSet: trying to evaluate DSBG ", v, "\n");
  return fact_.createSet(); 
}

} // namespace Eval

} // namespace SBG
