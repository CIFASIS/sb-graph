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

#include <iostream>
#include "eval/visitors/autom_impl_visitor.hpp"

namespace SBG {

namespace Eval {

member_imp(Impl, int, set_impl);
member_imp(Impl, int, pw_impl);

Impl::Impl(int set_impl, int pw_impl)
  : set_impl_(set_impl), pw_impl_(pw_impl) {}

////////////////////////////////////////////////////////////////////////////////
// Automatic Implementation Visitor --------------------------------------------
////////////////////////////////////////////////////////////////////////////////

AutomImplVisitor::AutomImplVisitor(Impl user_impl)
  : user_impl_(user_impl), env_() {}

Impl AutomImplVisitor::operator()(AST::Program p) const 
{ 
  int set_res = 0, pw_res = 0;

  LIB::NAT dims = 1;
  AST::IsConfig cfg_visit;
  if (!p.stms().empty()) {
    AST::Statement first = p.stms()[0];
    if (boost::apply_visitor(cfg_visit, first))
      dims = boost::get<AST::ConfigDims>(first).nmbr_dims();
  }

  LIB::OrdDenseAF set_fact;
  LIB::MapAF map_fact(set_fact);
  LIB::UnordPWMapAF pw_fact(map_fact);

  StmVisitor stm_visit(dims, pw_fact);
  for (AST::Statement s : p.stms()) {
    if (!boost::apply_visitor(cfg_visit, s))
      StmEval se = boost::apply_visitor(stm_visit, s);
  }

  if (user_impl_.set_impl() < 0 && dims < 2) {
    bool is_optimizable = true;
    SetImplVisitor set_impl_visit(stm_visit.env());
    for (AST::Expr e : p.exprs())
      is_optimizable = is_optimizable && boost::apply_visitor(set_impl_visit, e);
    set_res = is_optimizable ? 2 : 0;
  }

  return Impl(set_res, pw_res);
}

////////////////////////////////////////////////////////////////////////////////
// Set Implementation Visitor --------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

SetImplVisitor::SetImplVisitor() : env_() {}
SetImplVisitor::SetImplVisitor(VarEnv env) : env_(env) {}

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
  EvalNat visit_nat(env_);
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
  EvalRat visit_rat(env_);
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
