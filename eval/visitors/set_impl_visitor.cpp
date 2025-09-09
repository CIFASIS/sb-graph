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
// Set Implementation Visitor --------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

SetImplVisitor::SetImplVisitor(const VarEnv& venv) : venv_(venv) {}

bool SetImplVisitor::operator()(AST::Natural v) const { return true; }

bool SetImplVisitor::operator()(AST::Rational v) const { return true; }

bool SetImplVisitor::operator()(AST::Name v) const { return true; }

bool SetImplVisitor::operator()(AST::UnaryOp v) const 
{
  return boost::apply_visitor(*this, v.expr());
}

bool SetImplVisitor::operator()(AST::BinOp v) const 
{
  return boost::apply_visitor(*this, v.left())
    && boost::apply_visitor(*this, v.right());
}

bool SetImplVisitor::operator()(AST::Call v) const
{
  bool res = true;

  for (const AST::Expr &e : v.args())
    res = res && boost::apply_visitor(*this, e); 

  return res;
}

bool SetImplVisitor::operator()(AST::Interval v) const
{
  NatEvaluator visit_nat(venv_);
  return boost::apply_visitor(visit_nat, v.step()) == 1;
}

bool SetImplVisitor::operator()(AST::MultiDimInter v) const
{
  bool res = true;

  for (const AST::Expr &e : v.intervals())
    res = res && boost::apply_visitor(*this, e);

  return res;
}

bool SetImplVisitor::operator()(AST::Set v) const
{
  bool res = true;

  for (const AST::Expr &e : v.pieces())
    res = res && boost::apply_visitor(*this, e);   

  return res;
}

bool SetImplVisitor::operator()(AST::LinearExp v) const
{
  RationalEvaluator visit_rat(venv_);
  LIB::RATIONAL r = boost::apply_visitor(visit_rat, v.slope());

  return r == 0 || r == 1;
}

bool SetImplVisitor::operator()(AST::MDLExp v) const
{
  bool res = true;

  for (const AST::Expr &e : v.exps())
    res = res && boost::apply_visitor(*this, e);

  return res;
}

bool SetImplVisitor::operator()(AST::LinearMap v) const
{
  bool dom = boost::apply_visitor(*this, v.dom());
  bool lexp = boost::apply_visitor(*this, v.lexp());

  return dom && lexp;
}

bool SetImplVisitor::operator()(AST::PWLMap v) const
{
  bool res = true;

  for (const AST::Expr &e : v.maps())
    res = res && boost::apply_visitor(*this, e);

  return res; 
}

bool SetImplVisitor::operator()(AST::SBG v) const
{
  bool res = true;

  res = res && boost::apply_visitor(*this, v.V());
  res = res && boost::apply_visitor(*this, v.Vmap());
  res = res && boost::apply_visitor(*this, v.map1());
  res = res && boost::apply_visitor(*this, v.map2());
  res = res && boost::apply_visitor(*this, v.Emap());

  return res;
}

bool SetImplVisitor::operator()(AST::DSBG v) const
{
  bool res = true;

  res = res && boost::apply_visitor(*this, v.V());
  res = res && boost::apply_visitor(*this, v.Vmap());
  res = res && boost::apply_visitor(*this, v.mapB());
  res = res && boost::apply_visitor(*this, v.mapD());
  res = res && boost::apply_visitor(*this, v.Emap());

  return res;
}

bool SetImplVisitor::operator()(AST::ParenExpr v) const
{
  return boost::apply_visitor(*this, v.e());
}

} // namespace Eval

} // namespace SBG
