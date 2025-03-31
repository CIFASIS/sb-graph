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
EvalRat::EvalRat(VarEnv &env) : env_(env) {}

LIB::RATIONAL EvalRat::operator()(AST::Natural v) const
{
  return LIB::RATIONAL(v, 1);
}

LIB::RATIONAL EvalRat::operator()(AST::Rational v) const
{
  EvalInt visit_int(env_);
  return LIB::RATIONAL(boost::apply_visitor(visit_int, v.num())
                        , boost::apply_visitor(visit_int, v.den()));
}

LIB::RATIONAL EvalRat::operator()(AST::Name v) const 
{
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (std::holds_alternative<LIB::RATIONAL>(value))
      return std::get<LIB::RATIONAL>(value);
    else if (std::holds_alternative<LIB::MD_NAT>(value)) {
      LIB::MD_NAT x = std::get<LIB::MD_NAT>(value);
      if (x.arity() == 1)
        return LIB::RATIONAL(x[0]);
    }
    else if (std::holds_alternative<LIB::NAT>(value))
      return LIB::RATIONAL(std::get<LIB::NAT>(value));

    else {
      Util::ERROR("EvalRat: variable ", v, " is not rational\n");
      return LIB::RATIONAL(0, 1);
    }
  }

  Util::ERROR("EvalRat: variable ", v, " undefined\n");
  return LIB::RATIONAL(0, 1);
}

LIB::RATIONAL EvalRat::operator()(AST::UnaryOp v) const 
{ 
  EvalRat visit_rat(env_);
  LIB::RATIONAL result = boost::apply_visitor(visit_rat, v.expr());
  switch (v.op()) {
    case AST::UnOp::oppo:
      return -result;

    default:
      Util::ERROR("EvalExpression: UnaryOp ", v.op(), " unsupported\n");
      return 0;
  }

  Util::ERROR("EvalExpression: UnaryOp ", v.op(), " unsupported\n");
  return 0;
}

LIB::RATIONAL EvalRat::operator()(AST::BinOp v) const 
{
  LIB::RATIONAL l = boost::apply_visitor(*this, v.left());
  LIB::RATIONAL r = boost::apply_visitor(*this, v.right());
  switch (v.op()) {
    case AST::Op::add:
      return l + r;

    case AST::Op::sub:
      return l - r;

    case AST::Op::mult:
      return l * r;

    default:
      Util::ERROR("EvalRat: BinOp ", v.op(), " unsupported\n");
      return LIB::RATIONAL(0, 1);
  }

  Util::ERROR("EvalExpression: BinOp ", v.op(), " unsupported\n");
  return 0;
}

LIB::RATIONAL EvalRat::operator()(AST::Call v) const
{
  Util::ERROR("EvalRat: trying to evaluate Call ", v, "\n");
  return LIB::RATIONAL(0, 1);
}

LIB::RATIONAL EvalRat::operator()(AST::Interval v) const
{
  Util::ERROR("EvalRat: trying to evaluate Interval ", v, "\n");
  return LIB::RATIONAL(0, 1);
}

LIB::RATIONAL EvalRat::operator()(AST::MultiDimInter v) const
{
  Util::ERROR("EvalRat: trying to evaluate MultiDimInter ", v, "\n");
  return LIB::RATIONAL(0, 1);
}

LIB::RATIONAL EvalRat::operator()(AST::Set v) const
{
  Util::ERROR("EvalRat: trying to evaluate Set ", v, "\n");
  return LIB::RATIONAL(0, 1);
}

LIB::RATIONAL EvalRat::operator()(AST::LinearExp v) const
{
  Util::ERROR("EvalRat: trying to evaluate LinearExp ", v, "\n");
  return LIB::RATIONAL(0, 1);
}

LIB::RATIONAL EvalRat::operator()(AST::MDLExp v) const
{
  Util::ERROR("EvalRat: trying to evaluate MDLExp ", v, "\n");
  return LIB::RATIONAL(0, 1);
}

LIB::RATIONAL EvalRat::operator()(AST::LinearMap v) const
{
  Util::ERROR("EvalRat: trying to evaluate LinearMap ", v, "\n");
  return LIB::RATIONAL(0, 1);
}

LIB::RATIONAL EvalRat::operator()(AST::PWLMap v) const
{
  Util::ERROR("EvalRat: trying to evaluate PWLMap ", v, "\n");
  return LIB::RATIONAL(0, 1);
}

LIB::RATIONAL EvalRat::operator()(AST::SBG v) const
{
  Util::ERROR("EvalRat: trying to evaluate SBG ", v, "\n");
  return LIB::RATIONAL(0, 1);
}

LIB::RATIONAL EvalRat::operator()(AST::DSBG v) const
{
  Util::ERROR("EvalRat: trying to evaluate DSBG ", v, "\n");
  return LIB::RATIONAL(0, 1);
}

LIB::RATIONAL EvalRat::operator()(AST::ParenExpr v) const
{
  return boost::apply_visitor(*this, v.e());
}

} // namespace Eval

} // namespace SBG
