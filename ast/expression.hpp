/** @file expr.hpp

 @brief <b>Expressions AST definition</b>

 <hr>

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

#ifndef SBGRAPH_AST_EXPRESSION_HPP_
#define SBGRAPH_AST_EXPRESSION_HPP_

#include "sbg/integer.hpp"
#include "util/defs.hpp"

#include <boost/variant/recursive_wrapper.hpp>
#include <boost/variant/variant.hpp>

#include <iosfwd>

namespace SBG {

namespace AST {

// Arithmetic and call structures ----------------------------------------------

using Name = std::string;
using Integer = LIB::Int;
class Rational;
class UnaryOp;
class BinOp;
class Call;
class Interval;
class MultiDimInter;
class Set;
class LinearExpr;
class MDLExp;
class LinearMap;
class PWLMap;
class SBG;
class BipartiteSBG;
class DSBG;
class ParenExpr;

using Expr = boost::variant<Integer, Name,
  boost::recursive_wrapper<Rational>,
  boost::recursive_wrapper<UnaryOp>, 
  boost::recursive_wrapper<BinOp>, 
  boost::recursive_wrapper<Call>, 
  boost::recursive_wrapper<Interval>, 
  boost::recursive_wrapper<MultiDimInter>,
  boost::recursive_wrapper<Set>,
  boost::recursive_wrapper<LinearExpr>,
  boost::recursive_wrapper<MDLExp>,
  boost::recursive_wrapper<LinearMap>,
  boost::recursive_wrapper<PWLMap>,
  boost::recursive_wrapper<SBG>,
  boost::recursive_wrapper<BipartiteSBG>,
  boost::recursive_wrapper<DSBG>,
  boost::recursive_wrapper<ParenExpr>>;
using ExprList = std::vector<Expr>;
std::ostream& operator<<(std::ostream& out, const ExprList& el);

template <typename T>
inline bool is(Expr e) { return e.type() == typeid(T); }

////////////////////////////////////////////////////////////////////////////////
// Arithmetic expressions ------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

class Rational {
public:
  Rational();
  Rational(Expr num, Expr den);

  const Expr& numerator() const;
  const Expr& denominator() const;

  bool operator==(const Rational& r) const;

private:
  Expr _numerator;
  Expr _denominator;
};
std::ostream& operator<<(std::ostream& out, const Rational& op);

////////////////////////////////////////////////////////////////////////////////
// SBG structures --------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

// Intervals -------------------------------------------------------------------

class Interval {
public:
  Interval() = default;
  Interval(Expr begin, Expr step, Expr end);

  const Expr& begin() const;
  const Expr& step() const;
  const Expr& end() const;

  bool operator==(const Interval& i) const;

private:
  Expr _begin;
  Expr _step;
  Expr _end;
};
std::ostream& operator<<(std::ostream& out, const Interval& i);

// Multi-dimensional intervals -------------------------------------------------

class MultiDimInter {
public:
  MultiDimInter() = default;
  MultiDimInter(ExprList intervals);

  const ExprList& intervals() const;

  bool operator==(const MultiDimInter& mdi) const;

private:
  ExprList _intervals;
};
std::ostream& operator<<(std::ostream& out, const MultiDimInter& mdi);

// Sets ------------------------------------------------------------------------

class Set {
public:
  Set() = default;
  Set(ExprList pieces);

  const ExprList& pieces() const;

  bool operator==(const Set& s) const;

private:
  ExprList _pieces;
};
std::ostream& operator<<(std::ostream& out, const Set& s); 

// Linear expression -----------------------------------------------------------

class LinearExpr {
public:
  LinearExpr() = default;
  LinearExpr(Expr slope, Expr offset);

  const Expr& slope() const; 
  const Expr& offset() const;

  bool operator==(const LinearExpr& le) const;

private:
  Expr _slope;
  Expr _offset;
};
std::ostream& operator<<(std::ostream& out, const LinearExpr& le);

// Multi-dimensional linear expression -----------------------------------------

class MDLExp {
public:
  MDLExp() = default;
  MDLExp(ExprList exps);

  const ExprList& exps() const;

  bool operator==(const MDLExp& mdle) const;

private:
  ExprList _exps;
};
std::ostream& operator<<(std::ostream& out, const MDLExp& le);

// SBG map ---------------------------------------------------------------------

class LinearMap {
public:
  LinearMap() = default;
  LinearMap(Expr dom, Expr lexp);

  const Expr& domain() const;
  const Expr& law() const;

  bool operator==(const LinearMap& lmap) const;

private:
  Expr _domain;
  Expr _law;
};
std::ostream& operator<<(std::ostream& out, const LinearMap& lm);

// Piecewise linear map --------------------------------------------------------

class PWLMap {
public:
  PWLMap() = default;
  PWLMap(ExprList maps);

  const ExprList& maps() const;

  bool operator==(const PWLMap& pw) const;

private:
  ExprList _maps;
};
std::ostream& operator<<(std::ostream& out, const PWLMap& pwl);

// SBG -------------------------------------------------------------------------

class SBG {
public:
  SBG() = default;
  SBG(Expr V, Expr Vmap, Expr map1, Expr map2, Expr Emap);

  const Expr& V() const;
  const Expr& Vmap() const;
  const Expr& map1() const;
  const Expr& map2() const;
  const Expr& Emap() const;

  bool operator==(const SBG& sbg) const;

protected:
  Expr _V;
  Expr _Vmap;
  Expr _map1;
  Expr _map2;
  Expr _Emap;
};
std::ostream& operator<<(std::ostream& out, const SBG& g);

// Bipartite SBG ---------------------------------------------------------------

class BipartiteSBG : public SBG {
public:
  BipartiteSBG() = default;
  BipartiteSBG(Expr V, Expr Vmap, Expr map1, Expr map2, Expr Emap
    , Expr X, Expr Y);

  const Expr& X() const;
  const Expr& Y() const;

  bool operator==(const BipartiteSBG& sbg) const;

private:
  Expr _X;
  Expr _Y;
};
std::ostream& operator<<(std::ostream& out, const SBG& g);

// DSBG -------------------------------------------------------------------------

class DSBG {
public:
  DSBG() = default;
  DSBG(Expr V, Expr Vmap, Expr mapB, Expr mapD, Expr Emap);

  const Expr& V() const;
  const Expr& Vmap() const;
  const Expr& mapB() const;
  const Expr& mapD() const;
  const Expr& Emap() const;

  bool operator==(const DSBG& dsbg) const;

private:
  Expr _V;
  Expr _Vmap;
  Expr _mapB;
  Expr _mapD;
  Expr _Emap;
};
std::ostream& operator<<(std::ostream& out, const DSBG& g);

////////////////////////////////////////////////////////////////////////////////
// Composite expressions -------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

// Unary operation -------------------------------------------------------------

enum class UnOp { oppo, card, comp };
std::ostream& operator<<(std::ostream& out, const UnOp& op);

class UnaryOp {
public:
  UnaryOp() = default;
  UnaryOp(UnOp op, Expr expr);

  const UnOp& op() const;
  const Expr& expr() const;

 bool operator==(const UnaryOp& uop) const;

private:
  UnOp _op;
  Expr _expr;
};
std::ostream& operator<<(std::ostream& out, const UnaryOp& uop);

// Binary operation ------------------------------------------------------------

enum class Op { add, sub, mult, expo, eq, less, cap, cup, diff };
std::ostream& operator<<(std::ostream& out, const Op& op);

class BinOp {
public:
  BinOp() = default;
  BinOp(Expr left, Op op, Expr right);

  const Expr& left() const;
  const Op& op() const;
  const Expr& right() const;

  bool operator==(const BinOp& bop) const;

private:
  Expr _left;
  Op _op;
  Expr _right;
};
std::ostream& operator<<(std::ostream& out, const BinOp& bop);

// Call ------------------------------------------------------------------------

class Call {
public:
  Call() = default;
  Call(Name name, Expr args);
  Call(Name name, ExprList args);

  const Name& name() const;
  const ExprList& args() const;

  bool operator==(const Call& c) const;

private:
  Name _name;
  ExprList _args;
};
std::ostream& operator<<(std::ostream& out, const Call& c);

// Parenthesis expression ------------------------------------------------------

class ParenExpr {
public:
  ParenExpr() = default;
  ParenExpr(Expr pexpr);

  const Expr& pexpr() const;

  bool operator==(const ParenExpr& pe) const;

private:
  Expr _pexpr;
};
std::ostream& operator<<(std::ostream& out, const ParenExpr& pe);

} // namespace AST

} // namespace SBG

#endif // SBGRAPH_AST_EXPRESSION_HPP_
