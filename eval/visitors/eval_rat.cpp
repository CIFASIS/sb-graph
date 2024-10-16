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

#include "eval/visitors/eval_rat.hpp"

namespace SBG {

namespace Eval {

EvalRat::EvalRat() : env_() {}
EvalRat::EvalRat(VarEnv env) : env_(env) {}

Util::RATIONAL EvalRat::operator()(AST::Natural v) const
{
  return Util::RATIONAL(v, 1);
}

Util::RATIONAL EvalRat::operator()(AST::MDNatural v) const
{
  Util::ERROR("EvalRat: trying to evaluate MDNatural ", v, "\n");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::Rational v) const
{
  EvalNat visit_nat(env_);
  return Util::RATIONAL(boost::apply_visitor(visit_nat, v.num())
                        , boost::apply_visitor(visit_nat, v.den()));
}

Util::RATIONAL EvalRat::operator()(AST::Boolean v) const 
{ 
  if (v)
    return Util::RATIONAL(1, 1);

  return Util::RATIONAL(0, 1); 
}

Util::RATIONAL EvalRat::operator()(Util::VariableName v) const 
{
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (std::holds_alternative<Util::RATIONAL>(value))
      return std::get<Util::RATIONAL>(value);

    if (std::holds_alternative<Util::MD_NAT>(value)) {
      Util::MD_NAT x = std::get<Util::MD_NAT>(value);
      if (x.arity() == 1)
        return Util::RATIONAL(x[0]);
   }

    else {
      Util::ERROR("EvalRat: variable ", v, " is not rational\n");
      return Util::RATIONAL(0, 1);
    }
  }

  Util::ERROR("EvalRat: variable ", v, " undefined\n");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::UnaryOp v) const 
{ 
  std::stringstream ss;
  ss << v.op();

  EvalRat visit_rat(env_);
  Util::RATIONAL result = boost::apply_visitor(visit_rat, v.expr());
  switch (v.op()) {
    case AST::UnOp::neg:
      return -result;

    default:
      Util::ERROR("EvalExpression: UnaryOp ", v.op(), " unsupported\n");
      return 0;
  }

  Util::ERROR("EvalExpression: UnaryOp ", v.op(), " unsupported\n");
  return 0;
}

Util::RATIONAL EvalRat::operator()(AST::BinOp v) const 
{
  Util::RATIONAL l = boost::apply_visitor(*this, v.left());
  Util::RATIONAL r = boost::apply_visitor(*this, v.right());
  switch (v.op()) {
    case AST::Op::add:
      return l + r;

    case AST::Op::sub:
      return l - r;

    case AST::Op::mult:
      return l * r;

    default:
      Util::ERROR("EvalRat: BinOp ", v.op(), " unsupported\n");
      return Util::RATIONAL(0, 1);
  }

  Util::ERROR("EvalExpression: BinOp ", v.op(), " unsupported\n");
  return 0;
}

Util::RATIONAL EvalRat::operator()(AST::Call v) const
{
  Util::ERROR("EvalRat: trying to evaluate Call ", v, "\n");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::Interval v) const
{
  Util::ERROR("EvalRat: trying to evaluate Interval ", v, "\n");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalRat: trying to evaluate InterUnaryOp ", v, "\n");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalRat: trying to evaluate InterBinOp ", v, "\n");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::MultiDimInter v) const
{
  Util::ERROR("EvalRat: trying to evaluate MultiDimInter ", v, "\n");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::MDInterUnaryOp v) const
{
  Util::ERROR("EvalRat: trying to evaluate MDInterUnaryOp ", v, "\n");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::MDInterBinOp v) const
{
  Util::ERROR("EvalRat: trying to evaluate MDInterBinOp ", v, "\n");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::Set v) const
{
  Util::ERROR("EvalRat: trying to evaluate Set ", v, "\n");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::SetUnaryOp v) const
{
  Util::ERROR("EvalRat: trying to evaluate SetUnaryOp ", v, "\n");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::SetBinOp v) const
{
  Util::ERROR("EvalRat: trying to evaluate SetBinOp ", v, "\n");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::LinearExp v) const
{
  Util::ERROR("EvalRat: trying to evaluate LinearExp ", v, "\n");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::LExpBinOp v) const
{
  Util::ERROR("EvalRat: trying to evaluate LExpBinOp ", v, "\n");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::MDLExp v) const
{
  Util::ERROR("EvalRat: trying to evaluate MDLExp ", v, "\n");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::MDLExpBinOp v) const
{
  Util::ERROR("EvalRat: trying to evaluate MDLExpBinOp ", v, "\n");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::LinearMap v) const
{
  Util::ERROR("EvalRat: trying to evaluate LinearMap ", v, "\n");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::PWLMap v) const
{
  Util::ERROR("EvalRat: trying to evaluate PWLMap ", v, "\n");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::SBG v) const
{
  Util::ERROR("EvalRat: trying to evaluate SBG ", v, "\n");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::DSBG v) const
{
  Util::ERROR("EvalRat: trying to evaluate DSBG ", v, "\n");
  return Util::RATIONAL(0, 1);
}

} // namespace Eval

} // namespace SBG
