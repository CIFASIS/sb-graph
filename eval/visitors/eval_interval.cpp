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

#include "eval/visitors/eval_interval.hpp"

namespace SBG {

namespace Eval {

EvalInterval::EvalInterval() : env_() {}
EvalInterval::EvalInterval(VarEnv env) : env_(env) {}

LIB::Interval EvalInterval::operator()(AST::Natural v) const 
{ 
  Util::ERROR("EvalInterval: trying to evaluate Natural ", v, "\n");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::MDNatural v) const 
{ 
  Util::ERROR("EvalInterval: trying to evaluate MDNatural ", v, "\n");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::Rational v) const 
{ 
  Util::ERROR("EvalInterval: trying to evaluate Rational ", v, "\n");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::Boolean v) const 
{ 
  Util::ERROR("EvalInterval: trying to evaluate Boolean ", v, "\n");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(Util::VariableName v) const 
{ 
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (std::holds_alternative<ContainerBaseType>(value)) {
      auto c = std::get<ContainerBaseType>(value);
      if (std::holds_alternative<LIB::Interval>(c)) 
        return std::get<LIB::Interval>(c); 
    }

    else {
      Util::ERROR("EvalInterval: variable ", v, " is not an interval\n");
      return LIB::Interval(); 
    } 
  }

  Util::ERROR("EvalInterval: variable ", v, " undefined\n");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::UnaryOp v) const 
{
  Util::ERROR("EvalInterval: trying to evaluate arithmetic UnaryOp ", v, "\n");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::BinOp v) const 
{
  Util::ERROR("EvalInterval: trying to evaluate arithmetic BinOp ", v, "\n");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::Call v) const
{
  Util::ERROR("EvalInterval: trying to evaluate Call ", v, "\n");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::Interval v) const 
{ 
  EvalNat visit_nat(env_);
  Util::NAT b = boost::apply_visitor(visit_nat, v.begin());
  Util::NAT s = boost::apply_visitor(visit_nat, v.step());
  Util::NAT e = boost::apply_visitor(visit_nat, v.end());

  return LIB::Interval(b, s, e);
}

LIB::Interval EvalInterval::operator()(AST::InterUnaryOp v) const
{
  AST::Expr exp = v.e();
  switch (v.op()) {
    default:
      Util::ERROR("EvalInterval: InterUnaryOp ", v.op(), " unsupported\n");
      return LIB::Interval(); 
  }
}

LIB::Interval EvalInterval::operator()(AST::InterBinOp v) const
{
  LIB::Interval l = boost::apply_visitor(*this, v.left());
  LIB::Interval r = boost::apply_visitor(*this, v.right());
  switch (v.op()) {
    case AST::ContainerOp::cap:
      return l.intersection(r);

    default:
      Util::ERROR("EvalInterval: InterBinOp ", v.op(), " unsupported\n");
      return LIB::Interval(); 
  }
}

LIB::Interval EvalInterval::operator()(AST::MultiDimInter v) const 
{
  Util::ERROR("EvalInterval: trying to evaluate MultiDimInter ", v, "\n");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::MDInterUnaryOp v) const 
{
  Util::ERROR("EvalInterval: trying to evaluate MDInterUnaryOp ", v, "\n");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::MDInterBinOp v) const 
{
  Util::ERROR("EvalInterval: trying to evaluate MDInterBinOp ", v, "\n");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::Set v) const 
{
  Util::ERROR("EvalInterval: trying to evaluate Set ", v, "\n");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::SetUnaryOp v) const 
{
  Util::ERROR("EvalInterval: trying to evaluate SetUnaryOp ", v, "\n");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::SetBinOp v) const 
{
  Util::ERROR("EvalInterval: trying to evaluate SetBinOp ", v, "\n");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::LinearExp v) const 
{
  Util::ERROR("EvalInterval: trying to evaluate LinearExp ", v, "\n");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::LExpBinOp v) const 
{
  Util::ERROR("EvalInterval: trying to evaluate LExpBinOp ", v, "\n");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::MDLExp v) const 
{
  Util::ERROR("EvalInterval: trying to evaluate MDLExp ", v, "\n");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::MDLExpBinOp v) const 
{
  Util::ERROR("EvalInterval: trying to evaluate MDLExpBinOp ", v, "\n");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::LinearMap v) const 
{
  Util::ERROR("EvalInterval: trying to evaluate LinearMap ", v, "\n");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::PWLMap v) const 
{
  Util::ERROR("EvalInterval: trying to evaluate PWLMap ", v, "\n");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::SBG v) const 
{
  Util::ERROR("EvalInterval: trying to evaluate SBG ", v, "\n");
  return LIB::Interval(); 
}

LIB::Interval EvalInterval::operator()(AST::DSBG v) const 
{
  Util::ERROR("EvalInterval: trying to evaluate DSBG ", v, "\n");
  return LIB::Interval(); 
}

} // namespace Eval

} // namespace SBG
