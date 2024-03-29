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

#include <boost/variant/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>

#include "util/defs.hpp"

namespace SBG {

namespace AST {

// Arithmetic and call structures ----------------------------------------------

typedef std::string Name;
typedef Util::NAT Natural;
typedef Util::MD_NAT MDNatural;
typedef bool Boolean;
struct Rational;
struct UnaryOp;
struct BinOp;
struct Call;
struct Interval;
struct InterUnaryOp;
struct InterBinOp;
struct MultiDimInter;
struct MDInterUnaryOp;
struct MDInterBinOp;
struct Set;
struct SetUnaryOp;
struct SetBinOp;
struct LinearExp;
struct LExpBinOp;
struct MDLExp;
struct MDLExpBinOp;
struct LinearMap;
struct PWLMap;
struct SBG;
struct DSBG;

typedef boost::variant<Natural, MDNatural, Boolean, Util::VariableName,
  boost::recursive_wrapper<Rational>,
  boost::recursive_wrapper<UnaryOp>, 
  boost::recursive_wrapper<BinOp>, 
  boost::recursive_wrapper<Call>, 
  boost::recursive_wrapper<Interval>, 
  boost::recursive_wrapper<InterUnaryOp>,
  boost::recursive_wrapper<InterBinOp>,
  boost::recursive_wrapper<MultiDimInter>,
  boost::recursive_wrapper<MDInterUnaryOp>,
  boost::recursive_wrapper<MDInterBinOp>,
  boost::recursive_wrapper<Set>,
  boost::recursive_wrapper<SetUnaryOp>,
  boost::recursive_wrapper<SetBinOp>,
  boost::recursive_wrapper<LinearExp>,
  boost::recursive_wrapper<LExpBinOp>,
  boost::recursive_wrapper<MDLExp>,
  boost::recursive_wrapper<MDLExpBinOp>,
  boost::recursive_wrapper<LinearMap>,
  boost::recursive_wrapper<PWLMap>,
  boost::recursive_wrapper<SBG>,
  boost::recursive_wrapper<DSBG>> Expr;
typedef std::vector<Expr> ExprList;
std::ostream &operator<<(std::ostream &out, const ExprList &el);

template <typename T>
inline bool is(Expr e) { return e.type() == typeid(T); }

struct Rational {
  member_class(Expr, num);
  member_class(Expr, den);

  Rational();
  Rational(Expr num, Expr den);

  bool operator==(const Rational &r) const;
};
std::ostream &operator<<(std::ostream &out, const Rational &op);

enum class UnOp { neg };
std::ostream &operator<<(std::ostream &out, const UnOp &op);

struct UnaryOp {
  member_class(UnOp, op);
  member_class(Expr, expr);

  UnaryOp();
  UnaryOp(UnOp op, Expr expr);

 bool operator==(const UnaryOp &uop) const;
};
std::ostream &operator<<(std::ostream &out, const UnaryOp &uop);

enum class Op { add, sub, mult, expo };
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

// SBG structures --------------------------------------------------------------

enum class ContainerUOp { card, comp };
std::ostream &operator<<(std::ostream &out, const ContainerUOp &op);

enum class ContainerOp { eq, less, cap, diff, cup };
std::ostream &operator<<(std::ostream &out, const ContainerOp &op);

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

struct InterUnaryOp {
  member_class(ContainerUOp, op);
  member_class(Expr, e);

  InterUnaryOp();
  InterUnaryOp(ContainerUOp op, Expr e);

  bool operator==(const InterUnaryOp &iuop) const;
};
std::ostream &operator<<(std::ostream &out, const InterUnaryOp &i);

struct InterBinOp {
  member_class(Expr, left);
  member_class(ContainerOp, op);
  member_class(Expr, right);

  InterBinOp();
  InterBinOp(Expr left, ContainerOp op, Expr right);

  bool operator==(const InterBinOp &ibop) const;
};
std::ostream &operator<<(std::ostream &out, const InterBinOp &i);

// Multi-dimensional intervals -------------------------------------------------

struct MultiDimInter {
  member_class(ExprList, intervals);

  MultiDimInter();
  MultiDimInter(ExprList intervals);

  bool operator==(const MultiDimInter &mdi) const;
};
std::ostream &operator<<(std::ostream &out, const MultiDimInter &mdi);

struct MDInterUnaryOp {
  member_class(ContainerUOp, op);
  member_class(Expr, e);

  MDInterUnaryOp();
  MDInterUnaryOp(ContainerUOp op, Expr e);

  bool operator==(const MDInterUnaryOp &mdiuop) const;
};
std::ostream &operator<<(std::ostream &out, const MDInterUnaryOp &mdi);

struct MDInterBinOp {
  member_class(Expr, left);
  member_class(ContainerOp, op);
  member_class(Expr, right);

  MDInterBinOp();
  MDInterBinOp(Expr left, ContainerOp op, Expr right);

  bool operator==(const MDInterBinOp &bop) const;
};
std::ostream &operator<<(std::ostream &out, const MDInterBinOp &mdi);

// Sets ------------------------------------------------------------------------

struct Set {
  member_class(ExprList, pieces);

  Set();
  Set(ExprList pieces);

  bool operator==(const Set &s) const;
};
std::ostream &operator<<(std::ostream &out, const Set &s); 

struct SetUnaryOp {
  member_class(ContainerUOp, op);
  member_class(Expr, e);

  SetUnaryOp();
  SetUnaryOp(ContainerUOp op, Expr e);

  bool operator==(const SetUnaryOp &uop) const;
};
std::ostream &operator<<(std::ostream &out, const SetUnaryOp &s);

struct SetBinOp {
  member_class(Expr, left);
  member_class(ContainerOp, op);
  member_class(Expr, right);

  SetBinOp();
  SetBinOp(Expr left, ContainerOp op, Expr right);

  bool operator==(const SetBinOp &bop) const;
};
std::ostream &operator<<(std::ostream &out, const SetBinOp &s);

// Linear expression -----------------------------------------------------------

struct LinearExp {
  member_class(Expr, slope);
  member_class(Expr, offset);

  LinearExp();
  LinearExp(Expr slope, Expr offset);

  bool operator==(const LinearExp &le) const;
};
std::ostream &operator<<(std::ostream &out, const LinearExp &le);

enum class ExpOp { eq, add, sub };
std::ostream &operator<<(std::ostream &out, const ExpOp &op);

struct LExpBinOp {
  member_class(Expr, left);
  member_class(ExpOp, op);
  member_class(Expr, right);

  LExpBinOp();
  LExpBinOp(Expr left, ExpOp op, Expr right);

  bool operator==(const LExpBinOp &bop) const;
};
std::ostream &operator<<(std::ostream &out, const LExpBinOp &lbop);

// Multi-dimensional linear expression -----------------------------------------

struct MDLExp {
  member_class(ExprList, exps);

  MDLExp();
  MDLExp(ExprList exps);

  bool operator==(const MDLExp &mdle) const;
};
std::ostream &operator<<(std::ostream &out, const MDLExp &le);

struct MDLExpBinOp {
  member_class(Expr, left);
  member_class(ExpOp, op);
  member_class(Expr, right);

  MDLExpBinOp();
  MDLExpBinOp(Expr left, ExpOp op, Expr right);

  bool operator==(const MDLExpBinOp &bop) const;
};
std::ostream &operator<<(std::ostream &out, const MDLExpBinOp &lbop);

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

struct SBG {
  member_class(Expr, V);
  member_class(Expr, Vmap);
  member_class(Expr, map1);
  member_class(Expr, map2);
  member_class(Expr, Emap);

  SBG();
  SBG(Expr V, Expr Vmap, Expr map1, Expr map2, Expr Emap);

  bool operator==(const SBG &sbg) const;
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

} // namespace AST

} // namespace SBG

#endif
