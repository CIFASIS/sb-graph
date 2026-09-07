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
#include "eval/visitors/rational_evaluator.hpp"
#include "util/debug.hpp"

namespace SBG {

namespace Eval {

namespace detail {

RationalEvaluator::RationalEvaluator() : _venv() {}

RationalEvaluator::RationalEvaluator(VarEnv& venv) : _venv(venv) {}

LIB::Rational RationalEvaluator::operator()(AST::Integer v) const
{
  return LIB::Rational{v};
}

LIB::Rational RationalEvaluator::operator()(AST::Rational v) const
{
  IntEvaluator visit_int{_venv};
  return LIB::Rational(boost::apply_visitor(visit_int, v.num())
                        , boost::apply_visitor(visit_int, v.den()));
}

LIB::Rational RationalEvaluator::operator()(AST::Name v) const 
{
  auto var_definition = _venv.find(v);
  if (var_definition != _venv.end()) { 
    ExprBaseType value = var_definition->second;
    if (std::holds_alternative<LIB::Rational>(value)) {
      return std::get<LIB::Rational>(value);
    } else if (std::holds_alternative<LIB::IntTuple>(value)) {
      LIB::IntTuple x = std::get<LIB::IntTuple>(value);
      if (x.arity() == 1) {
        return LIB::Rational(x[0]);
      }
    } else if (std::holds_alternative<LIB::Int>(value)) {
      return LIB::Rational{std::get<LIB::Int>(value)};
    } else {
      Util::ERROR("RationalEvaluator: variable ", v, " is not rational\n");
      return 0;
    }
  }

  Util::ERROR("RationalEvaluator: variable ", v, " undefined\n");
  return 0;
}

LIB::Rational RationalEvaluator::operator()(AST::UnaryOp v) const 
{ 
  RationalEvaluator visit_rat{_venv};
  LIB::Rational result = boost::apply_visitor(visit_rat, v.expr());
  switch (v.op()) {
    case AST::UnOp::oppo: {
      return -result;
    }

    default: {
      Util::ERROR("RationalEvaluator: UnaryOp ", v.op(), " unsupported\n");
      return 0;
    }
  }

  Util::ERROR("RationalEvaluator: UnaryOp ", v.op(), " unsupported\n");
  return 0;
}

LIB::Rational RationalEvaluator::operator()(AST::BinOp v) const 
{
  LIB::Rational l = boost::apply_visitor(*this, v.left());
  LIB::Rational r = boost::apply_visitor(*this, v.right());
  switch (v.op()) {
    case AST::Op::add: {
      return l + r;
    }

    case AST::Op::sub: {
      return l - r;
    }

    case AST::Op::mult: {
      return l * r;
    }

    default: {
      Util::ERROR("RationalEvaluator: BinOp ", v.op(), " unsupported\n");
      return 0;
    }
  }

  Util::ERROR("RationalEvaluator: BinOp ", v.op(), " unsupported\n");
  return 0;
}

LIB::Rational RationalEvaluator::operator()(AST::Call v) const
{
  Util::ERROR("RationalEvaluator: trying to evaluate Call ", v, "\n");
  return 0;
}

LIB::Rational RationalEvaluator::operator()(AST::Interval v) const
{
  Util::ERROR("RationalEvaluator: trying to evaluate Interval ", v, "\n");
  return 0;
}

LIB::Rational RationalEvaluator::operator()(AST::MultiDimInter v) const
{
  Util::ERROR("RationalEvaluator: trying to evaluate MultiDimInter ", v, "\n");
  return 0;
}

LIB::Rational RationalEvaluator::operator()(AST::Set v) const
{
  Util::ERROR("RationalEvaluator: trying to evaluate Set ", v, "\n");
  return 0;
}

LIB::Rational RationalEvaluator::operator()(AST::LinearExp v) const
{
  Util::ERROR("RationalEvaluator: trying to evaluate LinearExp ", v, "\n");
  return 0;
}

LIB::Rational RationalEvaluator::operator()(AST::MDLExp v) const
{
  Util::ERROR("RationalEvaluator: trying to evaluate MDLExp ", v, "\n");
  return 0;
}

LIB::Rational RationalEvaluator::operator()(AST::LinearMap v) const
{
  Util::ERROR("RationalEvaluator: trying to evaluate LinearMap ", v, "\n");
  return 0;
}

LIB::Rational RationalEvaluator::operator()(AST::PWLMap v) const
{
  Util::ERROR("RationalEvaluator: trying to evaluate PWLMap ", v, "\n");
  return 0;
}

LIB::Rational RationalEvaluator::operator()(AST::SBG v) const
{
  Util::ERROR("RationalEvaluator: trying to evaluate SBG ", v, "\n");
  return 0;
}

LIB::Rational RationalEvaluator::operator()(AST::BipartiteSBG v) const
{
  Util::ERROR("RationalEvaluator: trying to evaluate BipartiteSBG ", v, "\n");
  return 0;
}

LIB::Rational RationalEvaluator::operator()(AST::DSBG v) const
{
  Util::ERROR("RationalEvaluator: trying to evaluate DirectedSBG ", v, "\n");
  return 0;
}

LIB::Rational RationalEvaluator::operator()(AST::ParenExpr v) const
{
  return boost::apply_visitor(*this, v.e());
}

} // namespace detail

} // namespace Eval

} // namespace SBG
