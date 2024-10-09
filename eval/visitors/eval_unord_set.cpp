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

#include "eval/visitors/eval_unord_set.hpp"

namespace SBG {

namespace Eval {

EvalUnordSet::EvalUnordSet() : env_() {}
EvalUnordSet::EvalUnordSet(VarEnv env) : env_(env) {}

LIB::UnordSet EvalUnordSet::operator()(AST::Natural v) const
{
  Util::ERROR("EvalUnordSet: trying to evaluate Natural");
  return LIB::UnordSet(); 
}

LIB::UnordSet EvalUnordSet::operator()(AST::MDNatural v) const
{
  Util::ERROR("EvalUnordSet: trying to evaluate MDNatural");
  return LIB::UnordSet(); 
}

LIB::UnordSet EvalUnordSet::operator()(AST::Rational v) const
{
  Util::ERROR("EvalUnordSet: trying to evaluate Rational");
  return LIB::UnordSet(); 
}

LIB::UnordSet EvalUnordSet::operator()(AST::Boolean v) const
{
  Util::ERROR("EvalUnordSet: trying to evaluate Boolean");
  return LIB::UnordSet(); 
}

LIB::UnordSet EvalUnordSet::operator()(Util::VariableName v) const
{
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (std::holds_alternative<ContainerBaseType>(value)) {
      auto c = std::get<ContainerBaseType>(value);
      if (std::holds_alternative<LIB::UnordSet>(c))
        return std::get<LIB::UnordSet>(c);
    }

    else {
      Util::ERROR("EvalUnordSet: variable", v, " is not a set\n");
      return LIB::UnordSet(); 
    } 
  }

  Util::ERROR("EvalUnordSet: variable", v, " undefined\n");
  return LIB::UnordSet(); 
}

LIB::UnordSet EvalUnordSet::operator()(AST::UnaryOp v) const 
{
  Util::ERROR("EvalUnordSet: trying to evaluate arithmetic UnaryOp");
  return LIB::UnordSet(); 
}

LIB::UnordSet EvalUnordSet::operator()(AST::BinOp v) const 
{
  Util::ERROR("EvalUnordSet: trying to evaluate arithmetic BinOp");
  return LIB::UnordSet(); 
}

LIB::UnordSet EvalUnordSet::operator()(AST::Call v) const
{
  Util::ERROR("EvalUnordSet: trying to evaluate Call");
  return LIB::UnordSet(); 
}

LIB::UnordSet EvalUnordSet::operator()(AST::Interval v) const
{ 
  Util::ERROR("EvalUnordSet: trying to evaluate Interval");
  return LIB::UnordSet(); 
}

LIB::UnordSet EvalUnordSet::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalUnordSet: trying to evaluate InterUnaryOp");
  return LIB::UnordSet(); 
}

LIB::UnordSet EvalUnordSet::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalUnordSet: trying to evaluate InterBinOp");
  return LIB::UnordSet(); 
}

LIB::UnordSet EvalUnordSet::operator()(AST::MultiDimInter v) const
{ 
  Util::ERROR("EvalUnordSet: trying to evaluate MultiDimInter");
  return LIB::UnordSet(); 
}

LIB::UnordSet EvalUnordSet::operator()(AST::MDInterUnaryOp v) const
{
  Util::ERROR("EvalUnordSet: trying to evaluate MDInterUnaryOp");
  return LIB::UnordSet(); 
}

LIB::UnordSet EvalUnordSet::operator()(AST::MDInterBinOp v) const
{
  Util::ERROR("EvalUnordSet: trying to evaluate MDInterBinOp");
  return LIB::UnordSet(); 
}

LIB::UnordSet EvalUnordSet::operator()(AST::Set v) const 
{
  LIB::UnordSet res;

  EvalMDI mdi_visit(env_);
  for (AST::Expr e : v.pieces())
    res.emplaceBack(boost::apply_visitor(mdi_visit, e));

  return res;
}

LIB::UnordSet EvalUnordSet::operator()(AST::SetUnaryOp v) const 
{
  AST::Expr e = v.e();
  switch (v.op()) {
    default:
      Util::ERROR("EvalUnordSet: SetUnaryOp ", v.op(), " unsupported\n");
      return LIB::UnordSet(); 
  }
}

LIB::UnordSet EvalUnordSet::operator()(AST::SetBinOp v) const 
{
  LIB::UnordSet l = boost::apply_visitor(*this, v.left());
  LIB::UnordSet r = boost::apply_visitor(*this, v.right());
  switch (v.op()) {
    case AST::ContainerOp::cap:
      return l.intersection(r);

    case AST::ContainerOp::diff:
      return l.difference(r);

    default:
      Util::ERROR("EvalUnordSet: SetBinOp ", v.op(), " unsupported\n");
      return LIB::UnordSet(); 
  }
}

LIB::UnordSet EvalUnordSet::operator()(AST::LinearExp v) const
{ 
  Util::ERROR("EvalUnordSet: trying to evaluate LinearExp ", v, "\n");
  return LIB::UnordSet(); 
}

LIB::UnordSet EvalUnordSet::operator()(AST::LExpBinOp v) const
{ 
  Util::ERROR("EvalUnordSet: trying to evaluate LExpBinOp ", v, "\n");
  return LIB::UnordSet(); 
}

LIB::UnordSet EvalUnordSet::operator()(AST::MDLExp v) const
{ 
  Util::ERROR("EvalUnordSet: trying to evaluate MDLExp ", v, "\n");
  return LIB::UnordSet(); 
}

LIB::UnordSet EvalUnordSet::operator()(AST::MDLExpBinOp v) const
{ 
  Util::ERROR("EvalUnordSet: trying to evaluate MDLExpBinOp ", v, "\n");
  return LIB::UnordSet(); 
}

LIB::UnordSet EvalUnordSet::operator()(AST::LinearMap v) const
{ 
  Util::ERROR("EvalUnordSet: trying to evaluate LinearMap ", v, "\n");
  return LIB::UnordSet(); 
}

LIB::UnordSet EvalUnordSet::operator()(AST::PWLMap v) const
{ 
  Util::ERROR("EvalUnordSet: trying to evaluate PWLMap ", v, "\n");
  return LIB::UnordSet(); 
}

LIB::UnordSet EvalUnordSet::operator()(AST::SBG v) const
{ 
  Util::ERROR("EvalUnordSet: trying to evaluate SBG ", v, "\n");
  return LIB::UnordSet(); 
}

LIB::UnordSet EvalUnordSet::operator()(AST::DSBG v) const
{ 
  Util::ERROR("EvalUnordSet: trying to evaluate DSBG ", v, "\n");
  return LIB::UnordSet(); 
}

} // namespace Eval

} // namespace SBG
