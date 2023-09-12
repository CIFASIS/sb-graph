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

#include "ast/expr.hpp"

namespace SBG {

namespace AST {

//------------------------------------------------------------------------------
// Arithmetic expressions ------------------------------------------------------
//------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream &out, const UnOp &op)
{
  switch (op) {
    case UnOp::neg:
      out << "-";
      break;

    default:
      break;
  }

  return out;
}

std::ostream &operator<<(std::ostream &out, const Op &op)
{
  switch (op) {
    case Op::add:
      out << "+";
      break;

    case Op::sub:
      out << "-";
      break;

    case Op::mult:
      out << "*";
      break;

    case Op::expo:
      out << "^";
      break;

    default:
      break;
  }

  return out;
}

std::ostream &operator<<(std::ostream &out, const ExprList &el)
{
  BOOST_FOREACH (Expr e, el)
    out << e << "\n";

  return out;
}

UnaryOp::UnaryOp() : op_(), expr_() {}
UnaryOp::UnaryOp(UnOp op, Expr expr) : op_(op), expr_(expr) {}

member_imp(UnaryOp, UnOp, op);
member_imp(UnaryOp, Expr, expr);

bool UnaryOp::operator==(const UnaryOp &other) const
{
  return op() == other.op() && expr() == other.expr();
}

std::ostream &operator<<(std::ostream &out, const UnaryOp &uop)
{ 
  out << uop.op() << uop.expr();

  return out;
}

BinOp::BinOp() : left_(), op_(), right_() {}
BinOp::BinOp(Expr left, Op op, Expr right) : left_(left), op_(op), right_(right) {}

member_imp(BinOp, Expr, left);
member_imp(BinOp, Op, op);
member_imp(BinOp, Expr, right);

bool BinOp::operator==(const BinOp &other) const
{
  return left() == other.left() && op() == other.op() && right() == other.right();
}

std::ostream &operator<<(std::ostream &out, const BinOp &bop)
{ 
  out << bop.left() << bop.op() << bop.right();

  return out;
}

// Call expressions ------------------------------------------------------------

Call::Call() : name_(), args_() {}
Call::Call(Name name, ExprList args) : name_(name), args_(args) {}
Call::Call(Name name, Expr args) : name_(name), args_() { args_ref().push_back(args); }

member_imp(Call, Name, name);
member_imp(Call, ExprList, args);

bool Call::operator==(const Call &other) const
{
  return name() == other.name() && args() == other.args();
}

std::ostream &operator<<(std::ostream &out, const Call &c)
{
  unsigned int sz = c.args().size();

  out << c.name() << "(";
  if (sz > 0) {
    unsigned int i = 0;
    for (; i < sz - 1; i++)
      out << c.args()[i] << ", ";
    out << c.args()[i];
  }
  out << ")"; 

  return out;
}

//------------------------------------------------------------------------------
// SBG Structures --------------------------------------------------------------
//------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream &out, const ContainerUOp &op)
{
  switch (op) {
    case ContainerUOp::card:
      out << "#";
      break;

    case ContainerUOp::comp:
      out << "\'";
      break;
  }

  return out;
}

std::ostream &operator<<(std::ostream &out, const ContainerOp &op)
{
  switch (op) {
    case ContainerOp::eq:
      out << "==";
      break;

    case ContainerOp::less:
      out << "<";
      break;

    case ContainerOp::cap:
      out << "/\\";
      break;

    case ContainerOp::diff:
      out << "\\";
      break;

    case ContainerOp::cup:
      out << "\\/";
      break;

    default:
      break;
  }

  return out;
}

// Interval --------------------------------------------------------------------

Interval::Interval() : begin_(), step_(), end_() {}
Interval::Interval(Expr begin, Expr step, Expr end) : begin_(begin), step_(step), end_(end) {}

member_imp(Interval, Expr, begin);
member_imp(Interval, Expr, step);
member_imp(Interval, Expr, end);

bool Interval::operator==(const Interval &other) const
{
  return begin() == other.begin() && step() == other.step() && step() == other.step();
}

std::ostream &operator<<(std::ostream &out, const Interval &i)
{ 
  out << "[" << i.begin() << ":" << i.step() << ":" << i.end() << "]";

  return out;
}

InterUnaryOp::InterUnaryOp() : op_(), e_() {}
InterUnaryOp::InterUnaryOp(ContainerUOp op, Expr e) : op_(op), e_(e) {}

member_imp(InterUnaryOp, ContainerUOp, op);
member_imp(InterUnaryOp, Expr, e);

bool InterUnaryOp::operator==(const InterUnaryOp &other) const
{
  return e() == other.e() && op() == other.op();
}

std::ostream &operator<<(std::ostream &out, const InterUnaryOp &i) 
{
  switch (i.op()) {
    default: {
      out << i.op() << i.e(); 
      break;
    }
  }

  return out;
}

InterBinOp::InterBinOp() : left_(), op_(), right_() {}
InterBinOp::InterBinOp(Expr left, ContainerOp op, Expr right) : left_(left), op_(op), right_(right) {}

member_imp(InterBinOp, Expr, left);
member_imp(InterBinOp, ContainerOp, op);
member_imp(InterBinOp, Expr, right);

bool InterBinOp::operator==(const InterBinOp &other) const
{
  return left() == other.left() && op() == other.op() && right() == other.right();
}

std::ostream &operator<<(std::ostream &out, const InterBinOp &i)
{
  out << i.left() << i.op() << i.right(); 

  return out;
}

// Multi-dimensional interval --------------------------------------------------

MultiDimInter::MultiDimInter() : intervals_() {}
MultiDimInter::MultiDimInter(ExprList intervals) : intervals_(intervals) {}

member_imp(MultiDimInter, ExprList, intervals);

bool MultiDimInter::operator==(const MultiDimInter &other) const { return intervals() == other.intervals(); }

std::ostream &operator<<(std::ostream &out, const MultiDimInter &mdi)
{
  MultiDimInter aux = mdi;
  int sz = aux.intervals().size();

  if (sz > 0) {
    auto it = aux.intervals_ref().begin();
    for (int i = 0; i < sz - 1; ++i) {
      out << *it << " x "; 
      ++it;
    }
    out << *it;
  }

  return out; 
}

MDInterUnaryOp::MDInterUnaryOp() : op_(), e_() {}
MDInterUnaryOp::MDInterUnaryOp(ContainerUOp op, Expr e) : op_(op), e_(e) {}

member_imp(MDInterUnaryOp, ContainerUOp, op);
member_imp(MDInterUnaryOp, Expr, e);

bool MDInterUnaryOp::operator==(const MDInterUnaryOp &other) const { return op() == other.op() && e() == other.e(); }

std::ostream &operator<<(std::ostream &out, const MDInterUnaryOp &mdi)
{
  switch (mdi.op()) {
    case ContainerUOp::comp: {
      out << mdi.e() << mdi.op();
      break;
    }

    default: {
      out << mdi.op() << mdi.e();
      break;
    }
  }

  return out;
}

MDInterBinOp::MDInterBinOp() : left_(), op_(), right_() {}
MDInterBinOp::MDInterBinOp(Expr left, ContainerOp op, Expr right) : left_(left), op_(op), right_(right) {}

member_imp(MDInterBinOp, Expr, left);
member_imp(MDInterBinOp, ContainerOp, op);
member_imp(MDInterBinOp, Expr, right);

bool MDInterBinOp::operator==(const MDInterBinOp &other) const
{
  return left() == other.left() && op() == other.op() && right() == other.right();
}

std::ostream &operator<<(std::ostream &out, const MDInterBinOp &mdi)
{
  out << mdi.left() << mdi.op() << mdi.right(); 

  return out;
}

// Set -------------------------------------------------------------------------

Set::Set() : pieces_() {}
Set::Set(ExprList pieces) : pieces_(pieces) {}

member_imp(Set, ExprList, pieces);

bool Set::operator==(const Set &other) const { return pieces() == other.pieces(); }

std::ostream &operator<<(std::ostream &out, const Set &s)
{
  Set aux = s;
  int sz = aux.pieces().size();

  out << "{";
  if (sz > 0) {
    auto it = aux.pieces_ref().begin();
    for (int i = 0; i < sz - 1; ++i) {
      out << *it << ", "; 
      ++it;
    }
    out << *it;
  }
  out << "}";

  return out; 
}

SetUnaryOp::SetUnaryOp() : op_(), e_() {}
SetUnaryOp::SetUnaryOp(ContainerUOp op, Expr e) : op_(op), e_(e) {}

member_imp(SetUnaryOp, ContainerUOp, op);
member_imp(SetUnaryOp, Expr, e);

bool SetUnaryOp::operator==(const SetUnaryOp &other) const { return e() == other.e() && op() == other.op(); }

std::ostream &operator<<(std::ostream &out, const SetUnaryOp &s)
{
  switch (s.op()) {
    case ContainerUOp::comp: {
      out << s.e() << s.op();
      break;
    }

    default: {
      out << s.op() << s.e();
      break;
    }
  }

  return out;
}

SetBinOp::SetBinOp() : left_(), op_(), right_() {}
SetBinOp::SetBinOp(Expr left, ContainerOp op, Expr right) : left_(left), op_(op), right_(right) {}

member_imp(SetBinOp, Expr, left);
member_imp(SetBinOp, ContainerOp, op);
member_imp(SetBinOp, Expr, right);

bool SetBinOp::operator==(const SetBinOp &other) const
{
  return left() == other.left() && op() == other.op() && right() == other.right();
}

std::ostream &operator<<(std::ostream &out, const SetBinOp &s)
{
  out << s.left() << s.op() << s.right(); 

  return out;
}

// Linear expression -----------------------------------------------------------

LinearExp::LinearExp() : slope_(), offset_() {}
LinearExp::LinearExp(Expr slope, Expr offset) : slope_(slope), offset_(offset) {}

member_imp(LinearExp, Expr, slope);
member_imp(LinearExp, Expr, offset);

bool LinearExp::operator==(const LinearExp &other) const { return slope() == other.slope() && offset() == other.offset(); }

std::ostream &operator<<(std::ostream &out, const LinearExp &le)
{
  out << "(" << le.slope() << ")x+" << le.offset();

  return out;
}

std::ostream &operator<<(std::ostream &out, const ExpOp &op)
{
  switch (op) {
    case ExpOp::eq:
      out << "==";
      break;

    case ExpOp::add:
      out << "+";
      break;

    case ExpOp::sub:
      out << "-";
      break;

    default:
      break;
  }

  return out;
}

LExpBinOp::LExpBinOp() : left_(), op_(), right_() {}
LExpBinOp::LExpBinOp(Expr left, ExpOp op, Expr right) : left_(left), op_(op), right_(right) {}

member_imp(LExpBinOp, Expr, left);
member_imp(LExpBinOp, ExpOp, op);
member_imp(LExpBinOp, Expr, right);

bool LExpBinOp::operator==(const LExpBinOp &other) const
{
  return left() == other.left() && op() == other.op() && right() == other.right();
}

std::ostream &operator<<(std::ostream &out, const LExpBinOp &lbop)
{
  out << "(" << lbop.left() << ")" << lbop.op() << "(" << lbop.right() << ")";

  return out;
}

// Multi-dimensional linear expression -----------------------------------------

MDLExp::MDLExp() : exps_() {}
MDLExp::MDLExp(ExprList exps) : exps_(exps) {}

member_imp(MDLExp, ExprList, exps);

bool MDLExp::operator==(const MDLExp &other) const { return exps() == other.exps(); }

std::ostream &operator<<(std::ostream &out, const MDLExp &le)
{
  MDLExp aux = le;
  int sz = aux.exps().size();

  if (sz > 0) {
    auto it = aux.exps_ref().begin();
    for (int i = 0; i < sz - 1; ++i) {
      out << *it << "|";
      ++it;
    }
    out << *it;
  }

  return out; 
}

MDLExpBinOp::MDLExpBinOp() : left_(), op_(), right_() {}
MDLExpBinOp::MDLExpBinOp(Expr left, ExpOp op, Expr right) : left_(left), op_(op), right_(right) {}

member_imp(MDLExpBinOp, Expr, left);
member_imp(MDLExpBinOp, ExpOp, op);
member_imp(MDLExpBinOp, Expr, right);

bool MDLExpBinOp::operator==(const MDLExpBinOp &other) const
{
  return left() == other.left() && op() == other.op() && right() == other.right();
}

std::ostream &operator<<(std::ostream &out, const MDLExpBinOp &lbop)
{
  out << "(" << lbop.left() << ")" << lbop.op() << "(" << lbop.right() << ")";

  return out;
}

// SBG map ---------------------------------------------------------------------

LinearMap::LinearMap() : dom_(), lexp_() {}
LinearMap::LinearMap(Expr dom, Expr lexp) : dom_(dom), lexp_(lexp) {}

member_imp(LinearMap, Expr, dom);
member_imp(LinearMap, Expr, lexp);

bool LinearMap::operator==(const LinearMap &other) const
{
  return dom() == other.dom() && lexp() == other.lexp();
}

std::ostream &operator<<(std::ostream &out, const LinearMap &lmap)
{
  out << lmap.dom() << " ↦ " << lmap.lexp();

  return out;
}

// Piecewise linear map --------------------------------------------------------

PWLMap::PWLMap() : maps_() {}
PWLMap::PWLMap(ExprList maps) : maps_(maps) {}

member_imp(PWLMap, ExprList, maps);

bool PWLMap::operator==(const PWLMap &other) const { return maps() == other.maps(); }

std::ostream &operator<<(std::ostream &out, const PWLMap &pwl)
{
  PWLMap aux = pwl;
  unsigned int sz = aux.maps_ref().size();
 
  out << "<<";
  if (sz > 0) {
    auto it = aux.maps_ref().begin();
    for (unsigned int i = 0; i < sz - 1; ++i) {
      out << *it << ", ";
      ++it;
    }
    out << *it;
  }
  out << ">>";

  return out;
}

} // namespace AST

} // namespace SBG
