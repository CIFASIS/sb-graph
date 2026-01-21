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

#include "eval/visitors/nat_evaluator.hpp"

namespace SBG {

namespace Eval {

NatEvaluator::NatEvaluator() : venv_() {}
NatEvaluator::NatEvaluator(VarEnv &venv) : venv_(venv) {}

LIB::NAT NatEvaluator::operator()(AST::Natural v) const { return v; }

LIB::NAT NatEvaluator::operator()(AST::Rational v) const 
{ 
  if (boost::apply_visitor(*this, v.den()) == 1)
    return boost::apply_visitor(*this, v.num());

  Util::ERROR("NatEvaluator: trying to evaluate Rational ", v, "\n");
  return 0; 
}

LIB::NAT NatEvaluator::operator()(AST::Name v) const 
{ 
  auto var_definition = venv_.find(v);
  if (var_definition != venv_.end()) { 
    ExprBaseType value = var_definition->second;
    if (std::holds_alternative<LIB::NAT>(value)) 
      return std::get<LIB::NAT>(value);

    else if (std::holds_alternative<LIB::MD_NAT>(value)) {
      LIB::MD_NAT x = std::get<LIB::MD_NAT>(value);
      if (x.arity() == 1)
        return x[0];
    }

    else 
      if (std::holds_alternative<LIB::RATIONAL>(value))
        return std::get<LIB::RATIONAL>(value).toNat();
  }

  Util::ERROR("NatEvaluator: variable ", v, " undefined\n");
  return 0;
}

LIB::NAT NatEvaluator::operator()(AST::UnaryOp v) const 
{
  Util::ERROR("NatEvaluator: trying to evaluate arithmetic UnaryOp ", v, "\n");
  return 0;
}

LIB::NAT NatEvaluator::operator()(AST::BinOp v) const 
{
  LIB::NAT l = boost::apply_visitor(*this, v.left());
  LIB::NAT r = boost::apply_visitor(*this, v.right());
  switch (v.op()) {
    case AST::Op::add:
      return l + r;

    case AST::Op::sub:
      return l - r;

    case AST::Op::mult:
      return l * r;

    case AST::Op::expo:
      return pow(l, r);

    default:
      Util::ERROR("NatEvaluator: BinOp ", v.op(), " unsupported\n");
      return 0;
  }
}

LIB::NAT NatEvaluator::operator()(AST::Call v) const
{
  Util::ERROR("NatEvaluator: trying to evaluate Call ", v, "\n");
  return 0;
}

LIB::NAT NatEvaluator::operator()(AST::Interval v) const
{
  Util::ERROR("NatEvaluator: trying to evaluate Interval ", v, "\n");
  return 0;
}

LIB::NAT NatEvaluator::operator()(AST::MultiDimInter v) const
{
  Util::ERROR("NatEvaluator: trying to evaluate MultiDimInter ", v, "\n");
  return 0;
}

LIB::NAT NatEvaluator::operator()(AST::Set v) const
{
  Util::ERROR("NatEvaluator: trying to evaluate Set ", v, "\n");
  return 0;
}

LIB::NAT NatEvaluator::operator()(AST::LinearExp v) const
{
  Util::ERROR("NatEvaluator: trying to evaluate LinearExp ", v, "\n");
  return 0;
}

LIB::NAT NatEvaluator::operator()(AST::MDLExp v) const
{
  Util::ERROR("NatEvaluator: trying to evaluate MDLExp ", v, "\n");
  return 0;
}

LIB::NAT NatEvaluator::operator()(AST::LinearMap v) const
{
  Util::ERROR("NatEvaluator: trying to evaluate LinearMap ", v, "\n");
  return 0;
}

LIB::NAT NatEvaluator::operator()(AST::PWLMap v) const
{
  Util::ERROR("NatEvaluator: trying to evaluate PWLMap ", v, "\n");
  return 0;
}

LIB::NAT NatEvaluator::operator()(AST::SBG v) const
{
  Util::ERROR("NatEvaluator: trying to evaluate SBG ", v, "\n");
  return 0;
}

LIB::NAT NatEvaluator::operator()(AST::BipartiteSBG v) const
{
  Util::ERROR("NatEvaluator: trying to evaluate BipartiteSBG ", v, "\n");
  return 0;
}

LIB::NAT NatEvaluator::operator()(AST::DSBG v) const
{
  Util::ERROR("NatEvaluator: trying to evaluate DSBG ", v, "\n");
  return 0;
}

LIB::NAT NatEvaluator::operator()(AST::ParenExpr v) const
{
  return boost::apply_visitor(*this, v.e());
}

} // namespace Eval

} // namespace SBG
