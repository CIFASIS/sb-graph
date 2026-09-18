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

#include "ast/expression.hpp"

#include <iostream>

namespace SBG {

namespace AST {

////////////////////////////////////////////////////////////////////////////////
// Arithmetic expressions ------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

Rational::Rational() : _numerator(0), _denominator(1) {}

Rational::Rational(Expr numerator, Expr denominator)
  : _numerator(numerator), _denominator(denominator) {}

const Expr& Rational::numerator() const { return _numerator; }

const Expr& Rational::denominator() const { return _denominator; }

bool Rational::operator==(const Rational& other) const
{
  return _numerator == other._numerator
    && _denominator == other._denominator;
}

std::ostream& operator<<(std::ostream& out, const Rational& r)
{
  out << "(" << r.numerator() << "/" << r.denominator() << ")";

  return out;
}

////////////////////////////////////////////////////////////////////////////////
// SBG Structures --------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

// Interval --------------------------------------------------------------------

Interval::Interval(Expr begin, Expr step, Expr end)
  : _begin(begin), _step(step), _end(end) {}

const Expr& Interval::begin() const { return _begin; }

const Expr& Interval::step() const { return _step; }

const Expr& Interval::end() const { return _end; }

bool Interval::operator==(const Interval& other) const
{
  return _begin == other._begin && _step == other._step
   && _step == other._step;
}

std::ostream& operator<<(std::ostream& out, const Interval& i)
{ 
  out << "[" << i.begin() << ":" << i.step() << ":" << i.end() << "]";

  return out;
}

// Multi-dimensional interval --------------------------------------------------

MultiDimInter::MultiDimInter(ExprList intervals) : _intervals(intervals) {}

const ExprList& MultiDimInter::intervals() const { return _intervals; }

bool MultiDimInter::operator==(const MultiDimInter& other) const
{
  return _intervals == other._intervals;
}

std::ostream& operator<<(std::ostream& out, const MultiDimInter& mdi)
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

Set::Set(ExprList pieces) : _pieces(pieces) {}

const ExprList& Set::pieces() const { return _pieces; }

bool Set::operator==(const Set& other) const
{
  return _pieces == other._pieces;
}

std::ostream& operator<<(std::ostream& out, const Set& s)
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

LinearExpr::LinearExpr(Expr slope, Expr offset)
  : _slope(slope), _offset(offset) {}

const Expr& LinearExpr::slope() const { return _slope; }

const Expr& LinearExpr::offset() const { return _offset; }

bool LinearExpr::operator==(const LinearExpr& other) const
{
  return _slope == other._slope && _offset == other._offset;
}

std::ostream& operator<<(std::ostream& out, const LinearExpr& le)
{
  out << "(" << le.slope() << ")x+" << le.offset();

  return out;
}

// Multi-dimensional linear expression -----------------------------------------

MDLExp::MDLExp(ExprList exps) : _exps(exps) {}

const ExprList& MDLExp::exps() const { return _exps; }

bool MDLExp::operator==(const MDLExp& other) const
{
  return _exps == other._exps;
}

std::ostream& operator<<(std::ostream& out, const MDLExp& le)
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

LinearMap::LinearMap(Expr domain, Expr law) : _domain(domain), _law(law) {}

const Expr& LinearMap::domain() const { return _domain; }

const Expr& LinearMap::law() const { return _law; }

bool LinearMap::operator==(const LinearMap& other) const
{
  return _domain == other._domain && _law == other._law;
}

std::ostream& operator<<(std::ostream& out, const LinearMap& lmap)
{
  out << lmap.domain() << " ↦ " << lmap.law();

  return out;
}

// Piecewise linear map --------------------------------------------------------

PWLMap::PWLMap(ExprList maps) : _maps(maps) {}

const ExprList& PWLMap::maps() const { return _maps; }

bool PWLMap::operator==(const PWLMap& other) const
{
  return _maps == other._maps;
}

std::ostream& operator<<(std::ostream& out, const PWLMap& pwl)
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

SBG::SBG(Expr V, Expr Vmap, Expr map1, Expr map2, Expr Emap) : _V(V)
  , _Vmap(Vmap), _map1(map1), _map2(map2), _Emap(Emap) {}

const Expr& SBG::V() const { return _V; }

const Expr& SBG::Vmap() const { return _Vmap; }

const Expr& SBG::map1() const { return _map1; }

const Expr& SBG::map2() const { return _map2; }

const Expr& SBG::Emap() const { return _Emap; }

bool SBG::operator==(const SBG& other) const 
{ 
  return _V == other._V && _Vmap == other._Vmap && _map1 == other._map1
   && _map2 == other._map2 && _Emap == other._Emap;
}

std::ostream& operator<<(std::ostream& out, const SBG& g)
{
  out << "V: " << g.V() << "\n";
  out << "Vmap: " << g.Vmap() << "\n\n";
  out << "map1: " << g.map1() << "\n";
  out << "map2: " << g.map2() << "\n";
  out << "Emap: " << g.Emap() << "\n";

  return out;
}

// Bipartite SBG ---------------------------------------------------------------

BipartiteSBG::BipartiteSBG(Expr V, Expr Vmap, Expr map1, Expr map2, Expr Emap
  , Expr X, Expr Y)
  : SBG(V, Vmap, map1, map2, Emap), _X(X), _Y(Y) {}

const Expr& BipartiteSBG::X() const { return _X; }

const Expr& BipartiteSBG::Y() const { return _Y; }

bool BipartiteSBG::operator==(const BipartiteSBG& other) const 
{ 
  return _V == other._V && _Vmap == other._Vmap && _map1 == other._map1
   && _map2 == other._map2 && _Emap == other._Emap
   && _X == other._X && _Y == other._Y;
}

std::ostream& operator<<(std::ostream& out, const BipartiteSBG& g)
{
  out << "V: " << g.V() << "\n";
  out << "Vmap: " << g.Vmap() << "\n\n";
  out << "map1: " << g.map1() << "\n";
  out << "map2: " << g.map2() << "\n";
  out << "Emap: " << g.Emap() << "\n";
  out << "X: " << g.X() << "\n";
  out << "Y: " << g.Y();

  return out;
}

// DSBG ------------------------------------------------------------------------

DSBG::DSBG(Expr V, Expr Vmap, Expr mapB, Expr mapD, Expr Emap)
  : _V(V), _Vmap(Vmap), _mapB(mapB), _mapD(mapD), _Emap(Emap) {}

const Expr& DSBG::V() const { return _V; }

const Expr& DSBG::Vmap() const { return _Vmap; }

const Expr& DSBG::mapB() const { return _mapB; }

const Expr& DSBG::mapD() const { return _mapD; }

const Expr& DSBG::Emap() const { return _Emap; }

bool DSBG::operator==(const DSBG& other) const 
{ 
  return _V == other._V && _Vmap == other._Vmap  && _mapB == other._mapB
   && _mapD == other._mapD && _Emap == other._Emap;
}

std::ostream& operator<<(std::ostream& out, const DSBG& dg)
{
  out << "V: " << dg.V() << "\n";
  out << "Vmap: " << dg.Vmap() << "\n\n";
  out << "mapB: " << dg.mapB() << "\n";
  out << "mapD: " << dg.mapD() << "\n";
  out << "Emap: " << dg.Emap() << "\n";

  return out;
}

////////////////////////////////////////////////////////////////////////////////
// Composite expressions -------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& out, const UnOp& op)
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

std::ostream& operator<<(std::ostream& out, const Op& op)
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

// Unary operations ------------------------------------------------------------

UnaryOp::UnaryOp(UnOp op, Expr expr) : _op(op), _expr(expr) {}

const UnOp& UnaryOp::op() const { return _op; }

const Expr& UnaryOp::expr() const { return _expr;}

bool UnaryOp::operator==(const UnaryOp& other) const
{
  return _op == other._op && _expr == other._expr;
}

std::ostream& operator<<(std::ostream& out, const UnaryOp& uop)
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

// Binary operations -----------------------------------------------------------

BinOp::BinOp(Expr left, Op op, Expr right)
  : _left(left), _op(op), _right(right) {}

const Expr& BinOp::left() const { return _left; }

const Op& BinOp::op() const { return _op; }

const Expr& BinOp::right() const { return _right; }

bool BinOp::operator==(const BinOp& other) const
{
  return _left == other._left && _op == other._op 
   && _right == other._right;
}

std::ostream& operator<<(std::ostream& out, const BinOp& bop)
{ 
  out << bop.left() << bop.op() << bop.right();

  return out;
}

// Call ------------------------------------------------------------------------

Call::Call(Name name, Expr arg) : _name(name)
{
  _args.push_back(arg);
}

Call::Call(Name name, ExprList args) : _name(name), _args(args) {}

const Name& Call::name() const { return _name; }

const ExprList& Call::args() const { return _args; }

bool Call::operator==(const Call& other) const
{
  return _name == other._name && _args == other._args;
}

std::ostream& operator<<(std::ostream& out, const Call& c)
{
  unsigned int sz = c.args().size();

  out << c.name() << "(";
  if (sz > 0) {
    unsigned int i = 0;
    for (; i < sz - 1; ++i) {
      out << c.args()[i] << ", ";
    }
    out << c.args()[i];
  }
  out << ")"; 

  return out;
}

// Parenthesis expression ------------------------------------------------------

ParenExpr::ParenExpr(Expr e) : _pexpr(e) {}

const Expr& ParenExpr::pexpr() const { return _pexpr; }

bool ParenExpr::operator==(const ParenExpr& pe) const
{
  return _pexpr == pe._pexpr;
}

std::ostream& operator<<(std::ostream& out, const ParenExpr& pe)
{
  out << "(" << pe.pexpr() << ")";

  return out;
}

std::ostream& operator<<(std::ostream& out, const ExprList& el)
{
  for (const Expr& e : el) {
    out << e << ";\n";
  }

  return out;
}

} // namespace AST

} // namespace SBG
