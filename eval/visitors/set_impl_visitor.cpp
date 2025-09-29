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
#include "eval/visitors/rational_evaluator.hpp"
#include "eval/visitors/set_impl_visitor.hpp"
#include "eval/visitors/stm_evaluator.hpp"

namespace SBG {

namespace Eval {

////////////////////////////////////////////////////////////////////////////////
// Set Implementation single Expression Visitor --------------------------------
////////////////////////////////////////////////////////////////////////////////

SetImplExprVisitor::SetImplExprVisitor(const VarEnv& venv) : venv_(venv) {}

int SetImplExprVisitor::operator()(AST::Natural v) const { return 2; }

int SetImplExprVisitor::operator()(AST::Rational v) const { return 2; }

int SetImplExprVisitor::operator()(AST::Name v) const { return 2; }

int SetImplExprVisitor::operator()(AST::UnaryOp v) const 
{
  return boost::apply_visitor(*this, v.expr());
}

int SetImplExprVisitor::operator()(AST::BinOp v) const 
{
  int impl = 2;
  int limpl = boost::apply_visitor(*this, v.left());
  int rimpl = boost::apply_visitor(*this, v.right());

  return std::min(limpl, rimpl);
}

int SetImplExprVisitor::operator()(AST::Call v) const
{
  int impl = 2;
  for (const AST::Expr &e : v.args())
    impl = std::min(impl, boost::apply_visitor(*this, e));

  return impl;
}

int SetImplExprVisitor::operator()(AST::Interval v) const
{
  NatEvaluator visit_nat(venv_);
  return boost::apply_visitor(visit_nat, v.step()) == 1 ? 2 : 1;
}

int SetImplExprVisitor::operator()(AST::MultiDimInter v) const
{
  int impl = 2;
  for (const AST::Expr &e : v.intervals())
    impl = std::min(impl, boost::apply_visitor(*this, e));

  return impl;
}

int SetImplExprVisitor::operator()(AST::Set v) const
{
  int impl = 2;
  for (const AST::Expr &e : v.pieces())
    impl = std::min(impl, boost::apply_visitor(*this, e));

  return impl;
}

int SetImplExprVisitor::operator()(AST::LinearExp v) const
{
  RationalEvaluator visit_rat(venv_);
  LIB::RATIONAL r = boost::apply_visitor(visit_rat, v.slope());

  return (r == 0 || r == 1) ? 2 : 1;
}

int SetImplExprVisitor::operator()(AST::MDLExp v) const
{
  int impl = 2;
  for (const AST::Expr &e : v.exps())
    impl = std::min(impl, boost::apply_visitor(*this, e));

  return impl;
}

int SetImplExprVisitor::operator()(AST::LinearMap v) const
{
  int dom_impl = boost::apply_visitor(*this, v.dom());
  int lexp_impl = boost::apply_visitor(*this, v.lexp());

  return std::min(dom_impl, lexp_impl);
}

int SetImplExprVisitor::operator()(AST::PWLMap v) const
{
  int impl = 2;
  for (const AST::Expr &e : v.maps())
    impl = std::min(impl, boost::apply_visitor(*this, e));

  return impl; 
}

int SetImplExprVisitor::operator()(AST::SBG v) const
{
  int impl = 2;

  impl = std::min(impl, boost::apply_visitor(*this, v.V()));
  impl = std::min(impl, boost::apply_visitor(*this, v.Vmap()));
  impl = std::min(impl, boost::apply_visitor(*this, v.map1()));
  impl = std::min(impl, boost::apply_visitor(*this, v.map2()));
  impl = std::min(impl, boost::apply_visitor(*this, v.Emap()));

  return impl;
}

int SetImplExprVisitor::operator()(AST::DSBG v) const
{
  int impl = 2;

  impl = std::min(impl, boost::apply_visitor(*this, v.V()));
  impl = std::min(impl, boost::apply_visitor(*this, v.Vmap()));
  impl = std::min(impl, boost::apply_visitor(*this, v.mapB()));
  impl = std::min(impl, boost::apply_visitor(*this, v.mapD()));
  impl = std::min(impl, boost::apply_visitor(*this, v.Emap()));

  return impl;
}

int SetImplExprVisitor::operator()(AST::ParenExpr v) const
{
  return boost::apply_visitor(*this, v.e());
}

} // namespace Eval

} // namespace SBG
