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

#include "eval/visitors/int_evaluator.hpp"

namespace SBG {

namespace Eval {

IntEvaluator::IntEvaluator() : env_() {}
IntEvaluator::IntEvaluator(VarEnv &env) : env_(env) {}

LIB::INT IntEvaluator::operator()(AST::Natural v) const { return (LIB::INT) v; }

LIB::INT IntEvaluator::operator()(AST::Rational v) const 
{ 
  if (boost::apply_visitor(*this, v.den()) == 1)
    return boost::apply_visitor(*this, v.num());

  Util::ERROR("IntEvaluator: trying to evaluate Rational ", v, "\n");
  return 0; 
}

LIB::INT IntEvaluator::operator()(AST::Name v) const 
{ 
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (std::holds_alternative<LIB::NAT>(value))
      return (LIB::INT)(std::get<LIB::NAT>(value));
    else if (std::holds_alternative<LIB::MD_NAT>(value)) {
      LIB::MD_NAT x = std::get<LIB::MD_NAT>(value);
      return (LIB::INT)(x[0]);
    }
    else if (std::holds_alternative<LIB::RATIONAL>(value)) {
      LIB::RATIONAL x = std::get<LIB::RATIONAL>(value);
      return x.toInt();
    }
  }

  Util::ERROR("IntEvaluator: variable ", v, " undefined\n");
  return 0;
}

LIB::INT IntEvaluator::operator()(AST::UnaryOp v) const 
{
  LIB::INT x = boost::apply_visitor(*this, v.expr());
  switch (v.op()) {
    case AST::UnOp::oppo:
      return -x;

    default:
      Util::ERROR("IntEvaluator: BinOp ", v.op(), " unsupported\n");
      return 0;
  }
}

LIB::INT IntEvaluator::operator()(AST::BinOp v) const 
{
  LIB::INT l = boost::apply_visitor(*this, v.left());
  LIB::INT r = boost::apply_visitor(*this, v.right());
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
      Util::ERROR("IntEvaluator: BinOp ", v.op(), " unsupported\n");
      return 0;
  }
}

LIB::INT IntEvaluator::operator()(AST::Call v) const
{
  Util::ERROR("IntEvaluator: trying to evaluate Call ", v, "\n");
  return 0;
}

LIB::INT IntEvaluator::operator()(AST::Interval v) const
{
  Util::ERROR("IntEvaluator: trying to evaluate Interval ", v, "\n");
  return 0;
}

LIB::INT IntEvaluator::operator()(AST::MultiDimInter v) const
{
  Util::ERROR("IntEvaluator: trying to evaluate MultiDimInter ", v, "\n");
  return 0;
}

LIB::INT IntEvaluator::operator()(AST::Set v) const
{
  Util::ERROR("IntEvaluator: trying to evaluate Set ", v, "\n");
  return 0;
}

LIB::INT IntEvaluator::operator()(AST::LinearExp v) const
{
  Util::ERROR("IntEvaluator: trying to evaluate LinearExp ", v, "\n");
  return 0;
}

LIB::INT IntEvaluator::operator()(AST::MDLExp v) const
{
  Util::ERROR("IntEvaluator: trying to evaluate MDLExp ", v, "\n");
  return 0;
}

LIB::INT IntEvaluator::operator()(AST::LinearMap v) const
{
  Util::ERROR("IntEvaluator: trying to evaluate LinearMap ", v, "\n");
  return 0;
}

LIB::INT IntEvaluator::operator()(AST::PWLMap v) const
{
  Util::ERROR("IntEvaluator: trying to evaluate PWLMap ", v, "\n");
  return 0;
}

LIB::INT IntEvaluator::operator()(AST::SBG v) const
{
  Util::ERROR("IntEvaluator: trying to evaluate SBG ", v, "\n");
  return 0;
}

LIB::INT IntEvaluator::operator()(AST::DSBG v) const
{
  Util::ERROR("IntEvaluator: trying to evaluate DSBG ", v, "\n");
  return 0;
}

LIB::INT IntEvaluator::operator()(AST::ParenExpr v) const
{
  return boost::apply_visitor(*this, v.e());
}

} // namespace Eval

} // namespace SBG
