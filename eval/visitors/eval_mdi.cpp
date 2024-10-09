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

EvalMDI::EvalMDI() : env_() {}
EvalMDI::EvalMDI(VarEnv env) : env_(env) {}

LIB::MultiDimInter EvalMDI::operator()(AST::Natural v) const 
{ 
  Util::ERROR("EvalMDI: trying to evaluate Natural ", v, "\n");
  return LIB::MultiDimInter();
}

LIB::MultiDimInter EvalMDI::operator()(AST::MDNatural v) const 
{
  Util::ERROR("EvalMDI: trying to evaluate MDNatural ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::Rational v) const 
{ 
  Util::ERROR("EvalMDI: trying to evaluate Rational ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::Boolean v) const 
{ 
  Util::ERROR("EvalMDI: trying to evaluate Boolean ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(Util::VariableName v) const 
{ 
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (std::holds_alternative<ContainerBaseType>(value)) {
      auto c = std::get<ContainerBaseType>(value);
      if (std::holds_alternative<LIB::MultiDimInter>(c))
        return std::get<LIB::MultiDimInter>(c);
    }

    else {
      Util::ERROR("EvalMDI: variable ", v, " is not an interval\n");
      return LIB::MultiDimInter(); 
    } 
  }

  Util::ERROR("EvalMDI: variable ", v, " undefined\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::UnaryOp v) const 
{
  Util::ERROR("EvalMDI: trying to evaluate arithmetic UnaryOp ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::BinOp v) const 
{
  Util::ERROR("EvalMDI: trying to evaluate arithmetic BinOp ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::Call v) const
{
  Util::ERROR("EvalMDI: trying to evaluate Call ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::Interval v) const 
{ 
  Util::ERROR("EvalMDI: trying to evaluate Interval ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalMDI: trying to evaluate InterUnaryOp ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalMDI: trying to evaluate InterBinOp ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::MultiDimInter v) const 
{
  LIB::MultiDimInter res;

  EvalInterval visit_inter(env_);
  for (AST::Expr ith : v.intervals())
    res.emplaceBack(boost::apply_visitor(visit_inter, ith));

  return res;
}

LIB::MultiDimInter EvalMDI::operator()(AST::MDInterUnaryOp v) const 
{
  AST::Expr exp = v.e();
  switch (v.op()) {
    default:
      Util::ERROR("EvalMDI: MDInterUnaryOp ", v.op(), " unsupported\n");
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
      Util::ERROR("EvalMD: MDInterBinOp ", v.op(), " unsupported\n");
      return LIB::MultiDimInter(); 
  }
}

LIB::MultiDimInter EvalMDI::operator()(AST::Set v) const 
{
  Util::ERROR("EvalMDI: trying to evaluate Set ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::SetUnaryOp v) const 
{
  Util::ERROR("EvalMDI: trying to evaluate SetUnaryOp ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::SetBinOp v) const 
{
  Util::ERROR("EvalMDI: trying to evaluate SetBinOp ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::LinearExp v) const 
{
  Util::ERROR("EvalMDI: trying to evaluate LinearExp ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::LExpBinOp v) const 
{
  Util::ERROR("EvalMDI: trying to evaluate LExpBinOp ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::MDLExp v) const 
{
  Util::ERROR("EvalMDI: trying to evaluate MDLExp ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::MDLExpBinOp v) const 
{
  Util::ERROR("EvalMDI: trying to evaluate MDLExpBinOp ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::LinearMap v) const 
{
  Util::ERROR("EvalMDI: trying to evaluate LinearMap ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::PWLMap v) const 
{
  Util::ERROR("EvalMDI: trying to evaluate PWLMap ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::SBG v) const 
{
  Util::ERROR("EvalMDI: trying to evaluate SBG ", v, "\n");
  return LIB::MultiDimInter(); 
}

LIB::MultiDimInter EvalMDI::operator()(AST::DSBG v) const 
{
  Util::ERROR("EvalMDI: trying to evaluate DSBG ", v, "\n");
  return LIB::MultiDimInter(); 
}

} // namespace Eval

} // namespace SBG
