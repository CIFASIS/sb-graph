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

Util::RATIONAL EvalRat::operator()(AST::Natural v) const { return Util::RATIONAL(v, 1); }

Util::RATIONAL EvalRat::operator()(AST::MDNatural v) const
{
  Util::ERROR("EvalRat: trying to evaluate a MDNatural");
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
      if (x.size() == 1)
        return Util::RATIONAL(x[0]);
   }

    else {
      Util::ERROR("EvalRat: variable %s is not rational", v.c_str());
      return Util::RATIONAL(0, 1);
    }
  }

  Util::ERROR("EvalRat: variable %s not defined", v.c_str());
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
      Util::ERROR("EvalExpression: UnaryOp %s not supported.", ss.str().c_str());
      return 0;
  }

  Util::ERROR("EvalExpression: UnaryOp %s not supported.", ss.str().c_str());
  return 0;
}

Util::RATIONAL EvalRat::operator()(AST::BinOp v) const 
{
  std::stringstream ss;
  ss << v.op();

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
      Util::ERROR("EvalRat: BinOp %s not supported.", ss.str().c_str());
      return Util::RATIONAL(0, 1);
  }

  Util::ERROR("EvalExpression: BinOp %s not supported.", ss.str().c_str());
  return 0;
}

Util::RATIONAL EvalRat::operator()(AST::Call v) const
{
  Util::ERROR("EvalRat: trying to evaluate a Call");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::Interval v) const
{
  Util::ERROR("EvalRat: trying to evaluate an Interval");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalRat: trying to evaluate an InterUnaryOp");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalRat: trying to evaluate an InterBinOp");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::MultiDimInter v) const
{
  Util::ERROR("EvalRat: trying to evaluate a MultiDimInter");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::MDInterUnaryOp v) const
{
  Util::ERROR("EvalRat: trying to evaluate a MDInterUnaryOp");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::MDInterBinOp v) const
{
  Util::ERROR("EvalRat: trying to evaluate a MDInterBinOp");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::Set v) const
{
  Util::ERROR("EvalRat: trying to evaluate a Set");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::SetUnaryOp v) const
{
  Util::ERROR("EvalRat: trying to evaluate a SetUnaryOp");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::SetBinOp v) const
{
  Util::ERROR("EvalRat: trying to evaluate a SetBinOp");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::LinearExp v) const
{
  Util::ERROR("EvalRat: trying to evaluate a LinearExp");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::LExpBinOp v) const
{
  Util::ERROR("EvalRat: trying to evaluate a LExpBinOp");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::MDLExp v) const
{
  Util::ERROR("EvalRat: trying to evaluate a MDLExp");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::MDLExpBinOp v) const
{
  Util::ERROR("EvalRat: trying to evaluate a MDLExpBinOp");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::LinearMap v) const
{
  Util::ERROR("EvalRat: trying to evaluate a LinearMap");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::PWLMap v) const
{
  Util::ERROR("EvalRat: trying to evaluate a PWLMap");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::SBG v) const
{
  Util::ERROR("EvalRat: trying to evaluate a SBG");
  return Util::RATIONAL(0, 1);
}

Util::RATIONAL EvalRat::operator()(AST::DSBG v) const
{
  Util::ERROR("EvalRat: trying to evaluate a DSBG");
  return Util::RATIONAL(0, 1);
}

} // namespace Eval

} // namespace SBG
