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

#include "eval/visitors/check_opt_conds.hpp"

namespace SBG {

namespace Eval {

OptConds::OptConds() : env_() {}
OptConds::OptConds(VarEnv env) : env_(env) {}

bool OptConds::operator()(AST::Natural v) const 
{ 
  return true;
}

bool OptConds::operator()(AST::MDNatural v) const 
{ 
  return true;
}

bool OptConds::operator()(AST::Rational v) const 
{ 
  return true; 
}

bool OptConds::operator()(AST::Boolean v) const 
{ 
  return true; 
}

bool OptConds::operator()(Util::VariableName v) const 
{ 
  return true; 
}

bool OptConds::operator()(AST::UnaryOp v) const 
{
  return boost::apply_visitor(*this, v.expr());
}

bool OptConds::operator()(AST::BinOp v) const 
{
  return boost::apply_visitor(*this, v.left()) 
    && boost::apply_visitor(*this, v.right()); 
}

bool OptConds::operator()(AST::Call v) const
{
  bool res = true;

  for (const AST::Expr &e : v.args())
    res = res && boost::apply_visitor(*this, e); 

  return res;
}

bool OptConds::operator()(AST::Interval v) const 
{
  EvalInterval visit_inter(env_);
  LIB::Interval i = boost::apply_visitor(visit_inter, AST::Expr(v));

  return i.step() == 1;
}

bool OptConds::operator()(AST::InterUnaryOp v) const
{
  return boost::apply_visitor(*this, v.e()); 
}

bool OptConds::operator()(AST::InterBinOp v) const
{
  bool l = boost::apply_visitor(*this, v.left());
  bool r = boost::apply_visitor(*this, v.right());
  switch (v.op()) {
    case AST::ContainerOp::cap:
      return l && r;

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("OptConds: InterBinOp %s not supported.", ss.str().c_str());
      return false; 
  }
}

bool OptConds::operator()(AST::MultiDimInter v) const 
{
  bool res = true;

  for (const AST::Expr &e : v.intervals())
    res = res && boost::apply_visitor(*this, e);

  return res;
}

bool OptConds::operator()(AST::MDInterUnaryOp v) const 
{
  return boost::apply_visitor(*this, v.e()); 
}

bool OptConds::operator()(AST::MDInterBinOp v) const 
{
  bool l = boost::apply_visitor(*this, v.left());
  bool r = boost::apply_visitor(*this, v.right());
  switch (v.op()) {
    case AST::ContainerOp::cap:
      return l && r;

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("OptConds: MDInterBinOp %s not supported.", ss.str().c_str());
      return false; 
  }
}

bool OptConds::operator()(AST::Set v) const 
{
  bool res = true;

  for (const AST::Expr &e : v.pieces())
    res = res && boost::apply_visitor(*this, e);   

  return res; 
}

bool OptConds::operator()(AST::SetUnaryOp v) const 
{
  return boost::apply_visitor(*this, v.e()); 
}

bool OptConds::operator()(AST::SetBinOp v) const 
{
  bool l = boost::apply_visitor(*this, v.left());
  bool r = boost::apply_visitor(*this, v.right());
  switch (v.op()) {
    case AST::ContainerOp::cap:
      return l && r;

    case AST::ContainerOp::diff:
      return l && r;

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalUnordSet: SetBinOp %s not supported.", ss.str().c_str());
      return false; 
  }
}

bool OptConds::operator()(AST::LinearExp v) const 
{
  return true; 
}

bool OptConds::operator()(AST::LExpBinOp v) const 
{
  return true; 
}

bool OptConds::operator()(AST::MDLExp v) const 
{
  return true; 
}

bool OptConds::operator()(AST::MDLExpBinOp v) const 
{
  return true; 
}

bool OptConds::operator()(AST::LinearMap v) const 
{
  return boost::apply_visitor(*this, v.dom()); 
}

bool OptConds::operator()(AST::PWLMap v) const 
{
  bool res = true;

  for (const AST::Expr &e : v.maps())
    res = res && boost::apply_visitor(*this, e);

  return res;
}

bool OptConds::operator()(AST::SBG v) const 
{
  bool res = true;

  res = res && boost::apply_visitor(*this, v.V());
  res = res && boost::apply_visitor(*this, v.Vmap());
  res = res && boost::apply_visitor(*this, v.map1());
  res = res && boost::apply_visitor(*this, v.map2());
  res = res && boost::apply_visitor(*this, v.Emap());

  return res;
}

bool OptConds::operator()(AST::DSBG v) const 
{
  bool res = true;

  res = res && boost::apply_visitor(*this, v.V());
  res = res && boost::apply_visitor(*this, v.Vmap());
  res = res && boost::apply_visitor(*this, v.mapB());
  res = res && boost::apply_visitor(*this, v.mapD());
  res = res && boost::apply_visitor(*this, v.Emap());

  return res;
}

} // namespace Eval

} // namespace SBG
