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

EvalMDNat::EvalMDNat() : env_() {}
EvalMDNat::EvalMDNat(VarEnv env) : env_(env) {}

Util::MD_NAT EvalMDNat::operator()(AST::Natural v) const
{
  Util::ERROR("EvalMDNat: trying to evaluate Natural ", v, "\n");
  return 0;
}

Util::MD_NAT EvalMDNat::operator()(AST::MDNatural v) const { return v; }

Util::MD_NAT EvalMDNat::operator()(AST::Rational v) const 
{ 
  if (v.denominator() == 1)
    return v.numerator();

  Util::ERROR("EvalMDNat: trying to evaluate Rational ", v, "\n");
  return 0; 
}

Util::MD_NAT EvalMDNat::operator()(AST::Boolean v) const 
{ 
  if (v)
    return 1;

  return 0; 
}

Util::MD_NAT EvalMDNat::operator()(Util::VariableName v) const 
{ 
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (std::holds_alternative<Util::MD_NAT>(value)) 
      return std::get<Util::MD_NAT>(value);
  }

  Util::ERROR("EvalMDNat: variable ", v, " undefined"\n);
  return 0;
}

Util::MD_NAT EvalMDNat::operator()(AST::UnaryOp v) const 
{
  Util::ERROR("EvalMDNat: trying to evaluate arithmetic UnaryOp ", v, "\n");
  return 0;
}

Util::MD_NAT EvalMDNat::operator()(AST::BinOp v) const 
{
  AST::Expr l = v.left(), r = v.right();
  switch (v.op()) {
    case AST::Op::add:
      return ApplyThis(l) + ApplyThis(r);

    case AST::Op::sub:
      return ApplyThis(l) - ApplyThis(r);

    case AST::Op::mult:
      return ApplyThis(l) * ApplyThis(r);

    case AST::Op::expo:
      return pow(ApplyThis(l), ApplyThis(r));

    default:
      Util::ERROR("EvalMDNat: BinOp ", v.op(), " unsupported\n");
      return 0;
  }
}

Util::MD_NAT EvalMDNat::operator()(AST::Call v) const
{
  Util::ERROR("EvalMDNat: trying to evaluate Call ", v, "\n");
  return 0;
}

Util::MD_NAT EvalMDNat::operator()(AST::Interval v) const
{
  Util::ERROR("EvalMDNat: trying to evaluate Interval ", v, "\n");
  return 0;
}

Util::MD_NAT EvalMDNat::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalMDNat: trying to evaluate InterUnaryOp ", v, "\n");
  return 0;
}

Util::MD_NAT EvalMDNat::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalMDNat: trying to evaluate InterBinOp ", v, "\n");
  return 0;
}

Util::MD_NAT EvalMDNat::operator()(AST::MultiDimInter v) const
{
  Util::ERROR("EvalMDNat: trying to evaluate MultiDimInter ", v, "\n");
  return 0;
}

Util::MD_NAT EvalMDNat::operator()(AST::MDInterUnaryOp v) const
{
  Util::ERROR("EvalMDNat: trying to evaluate MDInterUnaryOp ", v, "\n");
  return 0;
}

Util::MD_NAT EvalMDNat::operator()(AST::MDInterBinOp v) const
{
  Util::ERROR("EvalMDNat: trying to evaluate MDInterBinOp ", v, "\n");
  return 0;
}

Util::MD_NAT EvalMDNat::operator()(AST::Set v) const
{
  Util::ERROR("EvalMDNat: trying to evaluate Set ", v, "\n");
  return 0;
}

Util::MD_NAT EvalMDNat::operator()(AST::SetUnaryOp v) const
{
  Util::ERROR("EvalMDNat: trying to evaluate SetUnaryOp ", v, "\n");
  return 0;
}

Util::MD_NAT EvalMDNat::operator()(AST::SetBinOp v) const
{
  Util::ERROR("EvalMDNat: trying to evaluate SetBinOp ", v, "\n");
  return 0;
}

Util::MD_NAT EvalMDNat::operator()(AST::LinearExp v) const
{
  Util::ERROR("EvalMDNat: trying to evaluate LinearExp ", v, "\n");
  return 0;
}

Util::MD_NAT EvalMDNat::operator()(AST::LExpBinOp v) const
{
  Util::ERROR("EvalMDNat: trying to evaluate LExpBinOp ", v, "\n");
  return 0;
}

Util::MD_NAT EvalMDNat::operator()(AST::MDLExp v) const
{
  Util::ERROR("EvalMDNat: trying to evaluate MDLExp ", v, "\n");
  return 0;
}

Util::MD_NAT EvalMDNat::operator()(AST::MDLExpBinOp v) const
{
  Util::ERROR("EvalMDNat: trying to evaluate MDLExpBinOp ", v, "\n");
  return 0;
}

Util::MD_NAT EvalMDNat::operator()(AST::LinearMap v) const
{
  Util::ERROR("EvalMDNat: trying to evaluate LinearMap ", v, "\n");
  return 0;
}

Util::MD_NAT EvalMDNat::operator()(AST::PWLMap v) const
{
  Util::ERROR("EvalMDNat: trying to evaluate PWLMap ", v, "\n");
  return 0;
}

Util::MD_NAT EvalMDNat::operator()(AST::SBG v) const
{
  Util::ERROR("EvalMDNat: trying to evaluate a SBG ", v, "\n");
  return 0;
}

Util::MD_NAT EvalMDNat::operator()(AST::DSBG v) const
{
  Util::ERROR("EvalMDNat: trying to evaluate a DSBG ", v, "\n");
  return 0;
}

} // namespace Eval

} // namespace SBG
