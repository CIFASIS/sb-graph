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

#include "eval/visitors/eval_nat.hpp"

namespace SBG {

namespace Eval {

EvalNat::EvalNat() : env_() {}
EvalNat::EvalNat(VarEnv &env) : env_(env) {}

LIB::NAT EvalNat::operator()(AST::Natural v) const { return v; }

LIB::NAT EvalNat::operator()(AST::Rational v) const 
{ 
  if (boost::apply_visitor(*this, v.den()) == 1)
    return boost::apply_visitor(*this, v.num());

  Debug::ERROR("EvalNat: trying to evaluate Rational ", v, "\n");
  return 0; 
}

LIB::NAT EvalNat::operator()(AST::VariableName v) const 
{ 
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (std::holds_alternative<LIB::MD_NAT>(value)) {
      LIB::MD_NAT x = std::get<LIB::MD_NAT>(value);
      if (x.arity() == 1)
        return x[0];
    }

    else 
      if (std::holds_alternative<LIB::RATIONAL>(value))
        return std::get<LIB::RATIONAL>(value).toNat();
  }

  Debug::ERROR("EvalNat: variable ", v, " undefined\n");
  return 0;
}

LIB::NAT EvalNat::operator()(AST::UnaryOp v) const 
{
  Debug::ERROR("EvalNat: trying to evaluate arithmetic UnaryOp ", v, "\n");
  return 0;
}

LIB::NAT EvalNat::operator()(AST::BinOp v) const 
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
      Debug::ERROR("EvalNat: BinOp ", v.op(), " unsupported\n");
      return 0;
  }
}

LIB::NAT EvalNat::operator()(AST::Call v) const
{
  Debug::ERROR("EvalNat: trying to evaluate Call ", v, "\n");
  return 0;
}

LIB::NAT EvalNat::operator()(AST::Interval v) const
{
  Debug::ERROR("EvalNat: trying to evaluate Interval ", v, "\n");
  return 0;
}

LIB::NAT EvalNat::operator()(AST::InterUnaryOp v) const
{
  Debug::ERROR("EvalNat: trying to evaluate InterUnaryOp ", v, "\n");
  return 0;
}

LIB::NAT EvalNat::operator()(AST::InterBinOp v) const
{
  Debug::ERROR("EvalNat: trying to evaluate InterBinOp ", v, "\n");
  return 0;
}

LIB::NAT EvalNat::operator()(AST::MultiDimInter v) const
{
  Debug::ERROR("EvalNat: trying to evaluate MultiDimInter ", v, "\n");
  return 0;
}

LIB::NAT EvalNat::operator()(AST::MDInterUnaryOp v) const
{
  Debug::ERROR("EvalNat: trying to evaluate MDInterUnaryOp ", v, "\n");
  return 0;
}

LIB::NAT EvalNat::operator()(AST::MDInterBinOp v) const
{
  Debug::ERROR("EvalNat: trying to evaluate MDInterBinOp ", v, "\n");
  return 0;
}

LIB::NAT EvalNat::operator()(AST::Set v) const
{
  Debug::ERROR("EvalNat: trying to evaluate Set ", v, "\n");
  return 0;
}

LIB::NAT EvalNat::operator()(AST::SetUnaryOp v) const
{
  Debug::ERROR("EvalNat: trying to evaluate SetUnaryOp ", v, "\n");
  return 0;
}

LIB::NAT EvalNat::operator()(AST::SetBinOp v) const
{
  Debug::ERROR("EvalNat: trying to evaluate SetBinOp ", v, "\n");
  return 0;
}

LIB::NAT EvalNat::operator()(AST::LinearExp v) const
{
  Debug::ERROR("EvalNat: trying to evaluate LinearExp ", v, "\n");
  return 0;
}

LIB::NAT EvalNat::operator()(AST::LExpBinOp v) const
{
  Debug::ERROR("EvalNat: trying to evaluate LExpBinOp ", v, "\n");
  return 0;
}

LIB::NAT EvalNat::operator()(AST::MDLExp v) const
{
  Debug::ERROR("EvalNat: trying to evaluate MDLExp ", v, "\n");
  return 0;
}

LIB::NAT EvalNat::operator()(AST::MDLExpBinOp v) const
{
  Debug::ERROR("EvalNat: trying to evaluate MDLExpBinOp ", v, "\n");
  return 0;
}

LIB::NAT EvalNat::operator()(AST::LinearMap v) const
{
  Debug::ERROR("EvalNat: trying to evaluate LinearMap ", v, "\n");
  return 0;
}

LIB::NAT EvalNat::operator()(AST::PWLMap v) const
{
  Debug::ERROR("EvalNat: trying to evaluate PWLMap ", v, "\n");
  return 0;
}

LIB::NAT EvalNat::operator()(AST::SBG v) const
{
  Debug::ERROR("EvalNat: trying to evaluate SBG ", v, "\n");
  return 0;
}

LIB::NAT EvalNat::operator()(AST::DSBG v) const
{
  Debug::ERROR("EvalNat: trying to evaluate DSBG ", v, "\n");
  return 0;
}

} // namespace Eval

} // namespace SBG
