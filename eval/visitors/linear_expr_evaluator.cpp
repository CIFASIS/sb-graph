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
#include "eval/visitors/linear_expr_evaluator.hpp"
#include "eval/visitors/rational_evaluator.hpp"

namespace SBG {

namespace Eval {

LinearExprEvaluator::LinearExprEvaluator(VarEnv &venv) : venv_(venv) {}

LIB::LExp LinearExprEvaluator::operator()(AST::Natural v) const
{
  return LIB::LExp(0, LIB::RATIONAL(v)); 
}

LIB::LExp LinearExprEvaluator::operator()(AST::Rational v) const
{
  IntEvaluator visit_int(venv_);
  LIB::INT p = boost::apply_visitor(visit_int, v.num());
  LIB::INT q = boost::apply_visitor(visit_int, v.den());
  return LIB::LExp(0, LIB::RATIONAL(p, q));
}

LIB::LExp LinearExprEvaluator::operator()(AST::Name v) const
{
  if (v == "x")
    return LIB::LExp(1, 0);

  LIB::RATIONAL off = boost::apply_visitor(RationalEvaluator(venv_)
    , AST::Expr(v));
  return LIB::LExp(0, off);
}

LIB::LExp LinearExprEvaluator::operator()(AST::UnaryOp v) const
{
  LIB::LExp le = boost::apply_visitor(*this, v.expr());

  switch (v.op()) {
    case AST::UnOp::oppo:
      LIB::LExp(-le.slope(), -le.offset()); 
      break;

    default:
      Util::ERROR("LinearExprEvaluator: UnaryOp ", v.op(), " is not arithmetic\n");
      break;
  };

  return LIB::LExp();
}

LIB::LExp LinearExprEvaluator::operator()(AST::BinOp v) const 
{
  LIB::LExp vl = boost::apply_visitor(*this, v.left());
  LIB::LExp vr = boost::apply_visitor(*this, v.right());

  switch (v.op()) {
    case AST::Op::add:
      return LIB::LExp(vl.slope() + vr.slope(), vl.offset() + vr.offset()); 
      break;

    case AST::Op::sub:
      return LIB::LExp(vl.slope() - vr.slope(), vl.offset() - vr.offset()); 
      break;

    case AST::Op::mult:
      if (vl.slope() == 0 && vr.slope() == 0)
          return LIB::LExp(0, vr.offset()*vl.offset());

      else if (vl.slope() == 0)
        return LIB::LExp(vl.offset()*vr.slope(), vl.offset()*vr.offset());

      else if (vr.slope() == 0)
        return LIB::LExp(vl.slope()*vr.offset(), vl.offset()*vr.offset());

      Util::ERROR("LinearExprEvaluator: expression ", v, " is not linear\n");
      break;

    default:
      Util::ERROR("LinearExprEvaluator: UnaryOp ", v.op(), " is not arithmetic\n");
      break;
  };

  return LIB::LExp();
}

LIB::LExp LinearExprEvaluator::operator()(AST::Call v) const
{
  Util::ERROR("LinearExprEvaluator: trying to evaluate Call ", v, "\n");
  return LIB::LExp(); 
}

LIB::LExp LinearExprEvaluator::operator()(AST::Interval v) const
{ 
  Util::ERROR("LinearExprEvaluator: trying to evaluate Interval ", v, "\n");
  return LIB::LExp(); 
}

LIB::LExp LinearExprEvaluator::operator()(AST::MultiDimInter v) const
{ 
  Util::ERROR("LinearExprEvaluator: trying to evaluate MultiDimInter ", v, "\n");
  return LIB::LExp(); 
}

LIB::LExp LinearExprEvaluator::operator()(AST::Set v) const 
{
  Util::ERROR("LinearExprEvaluator: trying to evaluate Set ", v, "\n");
  return LIB::LExp(); 
}

LIB::LExp LinearExprEvaluator::operator()(AST::LinearExp v) const
{ 
  RationalEvaluator visit_rat(venv_);
  AST::Expr m = v.slope();
  AST::Expr h = v.offset();

  return LIB::LExp(boost::apply_visitor(visit_rat, m)
                   , boost::apply_visitor(visit_rat, h));
}

LIB::LExp LinearExprEvaluator::operator()(AST::MDLExp v) const
{
  Util::ERROR("LinearExprEvaluator: trying to evaluate MDLExp ", v, "\n");
  return LIB::LExp(); 
}

LIB::LExp LinearExprEvaluator::operator()(AST::LinearMap v) const 
{
  Util::ERROR("LinearExprEvaluator: trying to evaluate LinearMap ", v, "\n");
  return LIB::LExp(); 
}

LIB::LExp LinearExprEvaluator::operator()(AST::PWLMap v) const 
{
  Util::ERROR("LinearExprEvaluator: trying to evaluate PWLMap ", v, "\n");
  return LIB::LExp(); 
}

LIB::LExp LinearExprEvaluator::operator()(AST::SBG v) const 
{
  Util::ERROR("LinearExprEvaluator: trying to evaluate SBG ", v, "\n");
  return LIB::LExp(); 
}

LIB::LExp LinearExprEvaluator::operator()(AST::DSBG v) const 
{
  Util::ERROR("LinearExprEvaluator: trying to evaluate DSBG ", v, "\n");
  return LIB::LExp(); 
}

LIB::LExp LinearExprEvaluator::operator()(AST::ParenExpr v) const
{
  return boost::apply_visitor(*this, v.e());
}

} // namespace Eval

} // namespace SBG
