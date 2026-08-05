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

#ifndef PARSER_EXPR_AST_HPP
#define PARSER_EXPR_AST_HPP

#include <iosfwd>

#include <boost/variant/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>

#include "sbg/natural.hpp"
#include "util/defs.hpp"

namespace SBG {

namespace AST {

// Arithmetic and call structures ----------------------------------------------

using Name = std::string;
using Natural = LIB::NAT;
struct Rational;
struct UnaryOp;
struct BinOp;
struct Call;
struct Interval;
struct MultiDimInter;
struct Set;
struct LinearExp;
struct MDLExp;
struct LinearMap;
struct PWLMap;
class SBG;
class BipartiteSBG;
struct DSBG;
struct ParenExpr;

using Expr = boost::variant<Natural, Name,
  boost::recursive_wrapper<Rational>,
  boost::recursive_wrapper<UnaryOp>, 
  boost::recursive_wrapper<BinOp>, 
  boost::recursive_wrapper<Call>, 
  boost::recursive_wrapper<Interval>, 
  boost::recursive_wrapper<MultiDimInter>,
  boost::recursive_wrapper<Set>,
  boost::recursive_wrapper<LinearExp>,
  boost::recursive_wrapper<MDLExp>,
  boost::recursive_wrapper<LinearMap>,
  boost::recursive_wrapper<PWLMap>,
  boost::recursive_wrapper<SBG>,
  boost::recursive_wrapper<BipartiteSBG>,
  boost::recursive_wrapper<DSBG>,
  boost::recursive_wrapper<ParenExpr>>;
using ExprList = std::vector<Expr>;
std::ostream &operator<<(std::ostream &out, const ExprList &el);

template <typename T>
inline bool is(Expr e) { return e.type() == typeid(T); }

////////////////////////////////////////////////////////////////////////////////
// Arithmetic expressions ------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

struct Rational {
  member_class(Expr, num);
  member_class(Expr, den);

  Rational();
  Rational(Expr num, Expr den);

  bool operator==(const Rational &r) const;
};
std::ostream &operator<<(std::ostream &out, const Rational &op);

////////////////////////////////////////////////////////////////////////////////
// SBG structures --------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

// Intervals -------------------------------------------------------------------

struct Interval {
  member_class(Expr, begin);
  member_class(Expr, step);
  member_class(Expr, end);

  Interval();
  Interval(Expr begin, Expr step, Expr end);

  bool operator==(const Interval &i) const;
};
std::ostream &operator<<(std::ostream &out, const Interval &i);

// Multi-dimensional intervals -------------------------------------------------

struct MultiDimInter {
  member_class(ExprList, intervals);

  MultiDimInter();
  MultiDimInter(ExprList intervals);

  bool operator==(const MultiDimInter &mdi) const;
};
std::ostream &operator<<(std::ostream &out, const MultiDimInter &mdi);

// Sets ------------------------------------------------------------------------

struct Set {
  member_class(ExprList, pieces);

  Set();
  Set(ExprList pieces);

  bool operator==(const Set &s) const;
};
std::ostream &operator<<(std::ostream &out, const Set &s); 

// Linear expression -----------------------------------------------------------

struct LinearExp {
  member_class(Expr, slope);
  member_class(Expr, offset);

  LinearExp();
  LinearExp(Expr slope, Expr offset);

  bool operator==(const LinearExp &le) const;
};
std::ostream &operator<<(std::ostream &out, const LinearExp &le);

// Multi-dimensional linear expression -----------------------------------------

struct MDLExp {
  member_class(ExprList, exps);

  MDLExp();
  MDLExp(ExprList exps);

  bool operator==(const MDLExp &mdle) const;
};
std::ostream &operator<<(std::ostream &out, const MDLExp &le);

// SBG map ---------------------------------------------------------------------

struct LinearMap {
  member_class(Expr, dom);
  member_class(Expr, lexp);

  LinearMap();
  LinearMap(Expr dom, Expr lexp);

  bool operator==(const LinearMap &lmap) const;
};
std::ostream &operator<<(std::ostream &out, const LinearMap &lm);

// Piecewise linear map --------------------------------------------------------

struct PWLMap {
  member_class(ExprList, maps);

  PWLMap();
  PWLMap(ExprList maps);

  bool operator==(const PWLMap &pw) const;
};
std::ostream &operator<<(std::ostream &out, const PWLMap &pwl);

// SBG -------------------------------------------------------------------------

class SBG {
  public:
  SBG();
  SBG(Expr V, Expr Vmap, Expr map1, Expr map2, Expr Emap);

  const Expr& V() const;
  const Expr& Vmap() const;
  const Expr& map1() const;
  const Expr& map2() const;
  const Expr& Emap() const;

  bool operator==(const SBG &sbg) const;

  protected:
  Expr _V;
  Expr _Vmap;
  Expr _map1;
  Expr _map2;
  Expr _Emap;
};
std::ostream &operator<<(std::ostream &out, const SBG &g);

// Bipartite SBG ---------------------------------------------------------------

class BipartiteSBG : public SBG {
  public:
  BipartiteSBG();
  BipartiteSBG(Expr V, Expr Vmap, Expr map1, Expr map2, Expr Emap
    , Expr X, Expr Y);

  const Expr& X() const;
  const Expr& Y() const;

  bool operator==(const BipartiteSBG& sbg) const;

  private:
  Expr _X;
  Expr _Y;
};
std::ostream &operator<<(std::ostream &out, const SBG &g);

// DSBG -------------------------------------------------------------------------

struct DSBG {
  member_class(Expr, V);
  member_class(Expr, Vmap);
  member_class(Expr, mapB);
  member_class(Expr, mapD);
  member_class(Expr, Emap);

  DSBG();
  DSBG(Expr V, Expr Vmap, Expr mapB, Expr mapD, Expr Emap);

  bool operator==(const DSBG &dsbg) const;
};
std::ostream &operator<<(std::ostream &out, const DSBG &g);

////////////////////////////////////////////////////////////////////////////////
// Composite expressions -------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

enum class UnOp { oppo, card, comp };
std::ostream &operator<<(std::ostream &out, const UnOp &op);

struct UnaryOp {
  member_class(UnOp, op);
  member_class(Expr, expr);

  UnaryOp();
  UnaryOp(UnOp op, Expr expr);

 bool operator==(const UnaryOp &uop) const;
};
std::ostream &operator<<(std::ostream &out, const UnaryOp &uop);

enum class Op { add, sub, mult, expo, eq, less, cap, cup, diff };
std::ostream &operator<<(std::ostream &out, const Op &op);

struct BinOp {
  member_class(Expr, left);
  member_class(Op, op);
  member_class(Expr, right);

  BinOp();
  BinOp(Expr left, Op op, Expr right);

  bool operator==(const BinOp &bop) const;
};
std::ostream &operator<<(std::ostream &out, const BinOp &bop);

struct Call {
  member_class(Name, name);
  member_class(ExprList, args);

  Call();
  Call(Name name, Expr args);
  Call(Name name, ExprList args);

  bool operator==(const Call &c) const;
};
std::ostream &operator<<(std::ostream &out, const Call &c);

struct ParenExpr {
  member_class(Expr, e);

  ParenExpr();
  ParenExpr(Expr e);

  bool operator==(const ParenExpr &pe) const;
};
std::ostream &operator<<(std::ostream &out, const ParenExpr &pe);

} // namespace AST

} // namespace SBG

#endif
