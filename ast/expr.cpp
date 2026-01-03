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

////////////////////////////////////////////////////////////////////////////////
// Arithmetic expressions ------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

Rational::Rational() : num_(), den_() {}
Rational::Rational(Expr num, Expr den) : num_(num), den_(den) {}

member_imp(Rational, Expr, num);
member_imp(Rational, Expr, den);

bool Rational::operator==(const Rational &other) const
{
  return num() == other.num() && den() == other.den();
}

std::ostream &operator<<(std::ostream &out, const Rational &r)
{
  out << "(" << r.num() << "/" << r.den() << ")";

  return out;
}

////////////////////////////////////////////////////////////////////////////////
// SBG Structures --------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

// Interval --------------------------------------------------------------------

Interval::Interval() : begin_(), step_(), end_() {}
Interval::Interval(Expr begin, Expr step, Expr end) : begin_(begin), step_(step)
  , end_(end) {}

member_imp(Interval, Expr, begin);
member_imp(Interval, Expr, step);
member_imp(Interval, Expr, end);

bool Interval::operator==(const Interval &other) const
{
  return begin() == other.begin() && step() == other.step()
    && step() == other.step();
}

std::ostream &operator<<(std::ostream &out, const Interval &i)
{ 
  out << "[" << i.begin() << ":" << i.step() << ":" << i.end() << "]";

  return out;
}

// Multi-dimensional interval --------------------------------------------------

MultiDimInter::MultiDimInter() : intervals_() {}
MultiDimInter::MultiDimInter(ExprList intervals) : intervals_(intervals) {}

member_imp(MultiDimInter, ExprList, intervals);

bool MultiDimInter::operator==(const MultiDimInter &other) const
{
  return intervals() == other.intervals();
}

std::ostream &operator<<(std::ostream &out, const MultiDimInter &mdi)
{
  MultiDimInter aux = mdi;
  int sz = aux.intervals().size();

  if (sz > 0) {
    auto it = aux.intervals().begin();
    for (int i = 0; i < sz - 1; ++i) {
      out << *it << " x "; 
      ++it;
    }
    out << *it;
  }

  return out; 
}

// Set -------------------------------------------------------------------------

Set::Set() : pieces_() {}
Set::Set(ExprList pieces) : pieces_(pieces) {}

member_imp(Set, ExprList, pieces);

bool Set::operator==(const Set &other) const
{
  return pieces() == other.pieces();
}

std::ostream &operator<<(std::ostream &out, const Set &s)
{
  Set aux = s;
  int sz = aux.pieces().size();

  out << "{";
  if (sz > 0) {
    auto it = aux.pieces().begin();
    for (int i = 0; i < sz - 1; ++i) {
      out << *it << ", "; 
      ++it;
    }
    out << *it;
  }
  out << "}";

  return out; 
}

// Linear expression -----------------------------------------------------------

LinearExp::LinearExp() : slope_(), offset_() {}
LinearExp::LinearExp(Expr slope, Expr offset) : slope_(slope), offset_(offset)
  {}

member_imp(LinearExp, Expr, slope);
member_imp(LinearExp, Expr, offset);

bool LinearExp::operator==(const LinearExp &other) const
{
  return slope() == other.slope() && offset() == other.offset();
}

std::ostream &operator<<(std::ostream &out, const LinearExp &le)
{
  out << "(" << le.slope() << ")x+" << le.offset();

  return out;
}

// Multi-dimensional linear expression -----------------------------------------

MDLExp::MDLExp() : exps_() {}
MDLExp::MDLExp(ExprList exps) : exps_(exps) {}

member_imp(MDLExp, ExprList, exps);

bool MDLExp::operator==(const MDLExp &other) const
{
  return exps() == other.exps();
}

std::ostream &operator<<(std::ostream &out, const MDLExp &le)
{
  MDLExp aux = le;
  int sz = aux.exps().size();

  if (sz > 0) {
    auto it = aux.exps().begin();
    for (int i = 0; i < sz - 1; ++i) {
      out << *it << "|";
      ++it;
    }
    out << *it;
  }

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

bool PWLMap::operator==(const PWLMap &other) const
{
  return maps() == other.maps();
}

std::ostream &operator<<(std::ostream &out, const PWLMap &pwl)
{
  PWLMap aux = pwl;
  unsigned int sz = aux.maps().size();
 
  out << "<<";
  if (sz > 0) {
    auto it = aux.maps().begin();
    for (unsigned int i = 0; i < sz - 1; ++i) {
      out << *it << ", ";
      ++it;
    }
    out << *it;
  }
  out << ">>";

  return out;
}

// SBG -------------------------------------------------------------------------

SBG::SBG() : V_(), Vmap_(), map1_(), map2_(), Emap_(), subE_map_() {}
SBG::SBG(Expr V, Expr Vmap, Expr map1, Expr map2, Expr Emap, Expr subE) : V_(V)
  , Vmap_(Vmap), map1_(map1), map2_(map2), Emap_(Emap), subE_map_(subE) {}

member_imp(SBG, Expr, V);
member_imp(SBG, Expr, Vmap);
member_imp(SBG, Expr, map1);
member_imp(SBG, Expr, map2);
member_imp(SBG, Expr, Emap);
member_imp(SBG, Expr, subE_map);

bool SBG::operator==(const SBG &other) const 
{ 
  return V() == other.V() && Vmap() == other.Vmap() && map1() == other.map1()
    && map2() == other.map2() && Emap() == other.Emap()
    && subE_map() == other.subE_map();
}

std::ostream &operator<<(std::ostream &out, const SBG &g)
{
  out << "V: " << g.V() << "\n";
  out << "Vmap: " << g.Vmap() << "\n\n";
  out << "map1: " << g.map1() << "\n";
  out << "map2: " << g.map2() << "\n";
  out << "Emap: " << g.Emap() << "\n";
  out << "subE_map: " << g.subE_map() << "\n";

  return out;
}

// DSBG ------------------------------------------------------------------------

DSBG::DSBG() : V_(), Vmap_(), mapB_(), mapD_(), Emap_(), subE_map_() {}
DSBG::DSBG(Expr V, Expr Vmap, Expr mapB, Expr mapD, Expr Emap, Expr subE) : V_(V)
  , Vmap_(Vmap), mapB_(mapB), mapD_(mapD), Emap_(Emap), subE_map_(subE) {}

member_imp(DSBG, Expr, V);
member_imp(DSBG, Expr, Vmap);
member_imp(DSBG, Expr, mapB);
member_imp(DSBG, Expr, mapD);
member_imp(DSBG, Expr, Emap);
member_imp(DSBG, Expr, subE_map);

bool DSBG::operator==(const DSBG &other) const 
{ 
  return V() == other.V() && Vmap() == other.Vmap() && mapB() == other.mapB()
    && mapD() == other.mapD() && Emap() == other.Emap()
    && subE_map() == other.subE_map();
}

std::ostream &operator<<(std::ostream &out, const DSBG &g)
{
  out << "V: " << g.V() << "\n";
  out << "Vmap: " << g.Vmap() << "\n\n";
  out << "mapB: " << g.mapB() << "\n";
  out << "mapD: " << g.mapD() << "\n";
  out << "Emap: " << g.Emap() << "\n";
  out << "subE_map: " << g.subE_map() << "\n";

  return out;
}

////////////////////////////////////////////////////////////////////////////////
// Composite expressions -------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

std::ostream &operator<<(std::ostream &out, const UnOp &op)
{
  switch (op) {
    case UnOp::oppo:
      out << "-";
      break;

    case UnOp::card:
      out << "#";
      break;

    case UnOp::comp:
      out << "\'";
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

    case Op::eq:
      out << "==";
      break;

    case Op::less:
      out << "<";
      break;

    case Op::cap:
      out << "/\\";
      break;

    case Op::cup:
      out << "\\/";
      break;

    case Op::diff:
      out << "\\";
      break;

    default:
      break;
  }

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
  switch (uop.op()) {
    case UnOp::comp:
      out << uop.expr() << uop.op();
      break;

    default:
      out << uop.op() << uop.expr();
      break;
  }

  return out;
}

BinOp::BinOp() : left_(), op_(), right_() {}
BinOp::BinOp(Expr left, Op op, Expr right) : left_(left), op_(op)
  , right_(right) {}

member_imp(BinOp, Expr, left);
member_imp(BinOp, Op, op);
member_imp(BinOp, Expr, right);

bool BinOp::operator==(const BinOp &other) const
{
  return left() == other.left() && op() == other.op() 
    && right() == other.right();
}

std::ostream &operator<<(std::ostream &out, const BinOp &bop)
{ 
  out << bop.left() << bop.op() << bop.right();

  return out;
}

Call::Call() : name_(), args_() {}
Call::Call(Name name, ExprList args) : name_(name), args_(args) {}
Call::Call(Name name, Expr args) : name_(name), args_() {
  args_.push_back(args);
}

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
    for (; i < sz - 1; ++i)
      out << c.args()[i] << ", ";
    out << c.args()[i];
  }
  out << ")"; 

  return out;
}

ParenExpr::ParenExpr() : e_() {}
ParenExpr::ParenExpr(Expr e) : e_(e) {}

member_imp(ParenExpr, Expr, e);

bool ParenExpr::operator==(const ParenExpr &pe) const { return e_ == pe.e_; }

std::ostream &operator<<(std::ostream &out, const ParenExpr &pe)
{
  out << "(" << pe.e() << ")";

  return out;
}

std::ostream &operator<<(std::ostream &out, const ExprList &el)
{
  for (Expr e : el)
    out << e << ";\n";

  return out;
}

} // namespace AST

} // namespace SBG
