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

#include "eval/visitors/eval_ord_set.hpp"

namespace SBG {

namespace Eval {

EvalOrdSet::EvalOrdSet() : env_() {}
EvalOrdSet::EvalOrdSet(VarEnv env) : env_(env) {}

LIB::OrdSet EvalOrdSet::operator()(AST::Natural v) const
{
  Util::ERROR("EvalOrdSet: trying to evaluate Natural ", v, "\n");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::MDNatural v) const
{
  Util::ERROR("EvalOrdSet: trying to evaluate MDNatural ", v, "\n");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::Rational v) const
{
  Util::ERROR("EvalOrdSet: trying to evaluate Rational ", v, "\n");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::Boolean v) const
{
  Util::ERROR("EvalOrdSet: trying to evaluate Boolean ", v, "\n");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(Util::VariableName v) const
{
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (std::holds_alternative<ContainerBaseType>(value)) {
      auto c = std::get<ContainerBaseType>(value);
      if (std::holds_alternative<LIB::OrdSet>(c))
        return std::get<LIB::OrdSet>(c);
    }

    else {
      Util::ERROR("EvalOrdSet: variable ", v, " is not a set\n");
      return LIB::OrdSet(); 
    } 
  }

  Util::ERROR("EvalOrdSet: variable ", v, " undefined\n");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::UnaryOp v) const 
{
  Util::ERROR("EvalOrdSet: trying to evaluate arithmetic UnaryOp ", v, "\n");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::BinOp v) const 
{
  Util::ERROR("EvalOrdSet: trying to evaluate arithmetic BinOp ", v, "\n");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::Call v) const
{
  Util::ERROR("EvalOrdSet: trying to evaluate Call ", v, "\n");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::Interval v) const
{ 
  Util::ERROR("EvalOrdSet: trying to evaluate Interval ", v, "\n");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalOrdSet: trying to evaluate InterUnaryOp ", v, "\n");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalOrdSet: trying to evaluate InterBinOp ", v, "\n");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::MultiDimInter v) const
{ 
  Util::ERROR("EvalOrdSet: trying to evaluate MultiDimInter ", v, "\n");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::MDInterUnaryOp v) const
{
  Util::ERROR("EvalOrdSet: trying to evaluate MDInterUnaryOp ", v, "\n");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::MDInterBinOp v) const
{
  Util::ERROR("EvalOrdSet: trying to evaluate MDInterBinOp ", v, "\n");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::Set v) const 
{
  LIB::OrdSet res;

  EvalMDI mdi_visit(env_);
  for (AST::Expr e : v.pieces())
    res.emplaceBack(boost::apply_visitor(mdi_visit, e));

  return res;
}

LIB::OrdSet EvalOrdSet::operator()(AST::SetUnaryOp v) const 
{
  AST::Expr e = v.e();
  switch (v.op()) {
    default:
      Util::ERROR("EvalOrdSet: SetUnaryOp ", v.op(), " unsupported\n");
      return LIB::OrdSet(); 
  }
}

LIB::OrdSet EvalOrdSet::operator()(AST::SetBinOp v) const 
{
  LIB::OrdSet l = boost::apply_visitor(*this, v.left());
  LIB::OrdSet r = boost::apply_visitor(*this, v.right());
  switch (v.op()) {
    case AST::ContainerOp::cap:
      return l.intersection(r);

    case AST::ContainerOp::diff:
      return l.difference(r);

    default:
      Util::ERROR("EvalOrdSet: SetBinOp ", v.op(), " unsupported\n");
      return LIB::OrdSet(); 
  }
}

LIB::OrdSet EvalOrdSet::operator()(AST::LinearExp v) const
{ 
  Util::ERROR("EvalOrdSet: trying to evaluate LinearExp ", v, "\n");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::LExpBinOp v) const
{ 
  Util::ERROR("EvalOrdSet: trying to evaluate LExpBinOp ", v, "\n");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::MDLExp v) const
{ 
  Util::ERROR("EvalOrdSet: trying to evaluate MDLExp ", v, "\n");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::MDLExpBinOp v) const
{ 
  Util::ERROR("EvalOrdSet: trying to evaluate MDLExpBinOp ", v, "\n");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::LinearMap v) const
{ 
  Util::ERROR("EvalOrdSet: trying to evaluate LinearMap ", v, "\n");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::PWLMap v) const
{ 
  Util::ERROR("EvalOrdSet: trying to evaluate PWLMap ", v, "\n");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::SBG v) const
{ 
  Util::ERROR("EvalOrdSet: trying to evaluate SBG ", v, "\n");
  return LIB::OrdSet(); 
}

LIB::OrdSet EvalOrdSet::operator()(AST::DSBG v) const
{ 
  Util::ERROR("EvalOrdSet: trying to evaluate DSBG ", v, "\n");
  return LIB::OrdSet(); 
}

} // namespace Eval

} // namespace SBG
