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

#include <boost/foreach.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>

#include <util/defs.hpp>

namespace SBG {

namespace AST {

// Arithmetic and call structures ----------------------------------------------

typedef std::string Name;
typedef Util::NAT Natural;
typedef Util::RATIONAL Rational;
typedef bool Boolean;
struct Interval;
struct InterUnaryOp;
struct InterBinOp;
struct Set;
struct SetUnaryOp;
struct SetBinOp;
struct BinOp;
struct LinearExp;
struct LExpBinOp;
struct Call;

typedef boost::variant<Natural, Rational, Boolean, Util::VariableName, 
  boost::recursive_wrapper<BinOp>, 
  boost::recursive_wrapper<Call>, 
  boost::recursive_wrapper<Interval>, 
  boost::recursive_wrapper<InterUnaryOp>,
  boost::recursive_wrapper<InterBinOp>,
  boost::recursive_wrapper<Set>,
  boost::recursive_wrapper<SetUnaryOp>,
  boost::recursive_wrapper<SetBinOp>,
  boost::recursive_wrapper<LinearExp>,
  boost::recursive_wrapper<LExpBinOp>> Expr;
typedef std::vector<Expr> ExprList;
std::ostream &operator<<(std::ostream &out, const ExprList &el);

template <typename T>
inline bool is(Expr e) { return e.type() == typeid(T); }

typedef enum { add, sub, mult, expo } Op;
extern const char* OpNames[];
std::ostream &operator<<(std::ostream &out, const Op &op);

struct BinOp {
  member_class(Expr, left);
  member_class(Op, op);
  member_class(Expr, right);

  BinOp();
  BinOp(Expr left, Op op, Expr right);

  eq_class(BinOp);
};
std::ostream &operator<<(std::ostream &out, const BinOp &bop);

struct Call {
  member_class(Name, name);
  member_class(ExprList, args);

  Call();
  Call(Name name, Expr args);
  Call(Name name, ExprList args);

  eq_class(Call);
};
std::ostream &operator<<(std::ostream &out, const Call &c);

// SBG structures --------------------------------------------------------------

typedef enum { card, comp } ContainerUOp;
extern const char* ContUOpNames[];
std::ostream &operator<<(std::ostream &out, const ContainerUOp &op);

typedef enum { cap, diff, less, eq, cup } ContainerOp;
extern const char* ContOpNames[];
std::ostream &operator<<(std::ostream &out, const ContainerOp &op);

// Intervals -------------------------------------------------------------------

struct Interval {
  member_class(Expr, begin);
  member_class(Expr, step);
  member_class(Expr, end);

  Interval();
  Interval(Expr begin, Expr step, Expr end);

  eq_class(Interval);
};
std::ostream &operator<<(std::ostream &out, const Interval &i);

struct InterUnaryOp {
  member_class(ContainerUOp, op);
  member_class(Expr, e);

  InterUnaryOp();
  InterUnaryOp(ContainerUOp op, Expr e);

  eq_class(InterUnaryOp);
};
std::ostream &operator<<(std::ostream &out, const InterUnaryOp &i);

struct InterBinOp {
  member_class(Expr, left);
  member_class(ContainerOp, op);
  member_class(Expr, right);

  InterBinOp();
  InterBinOp(Expr left, ContainerOp op, Expr right);

  eq_class(InterBinOp);
};
std::ostream &operator<<(std::ostream &out, const InterBinOp &i);

// Sets ------------------------------------------------------------------------

struct Set {
  member_class(ExprList, pieces);

  Set();
  Set(ExprList pieces);

  eq_class(Set);
};
std::ostream &operator<<(std::ostream &out, const Set &s); 

struct SetUnaryOp {
  member_class(ContainerUOp, op);
  member_class(Expr, e);

  SetUnaryOp();
  SetUnaryOp(ContainerUOp op, Expr e);

  eq_class(SetUnaryOp);
};
std::ostream &operator<<(std::ostream &out, const SetUnaryOp &s);

struct SetBinOp {
  member_class(Expr, left);
  member_class(ContainerOp, op);
  member_class(Expr, right);

  SetBinOp();
  SetBinOp(Expr left, ContainerOp op, Expr right);

  eq_class(SetBinOp);
};
std::ostream &operator<<(std::ostream &out, const SetBinOp &s);

// Linear expression -----------------------------------------------------------

struct LinearExp {
  member_class(Expr, slope);
  member_class(Expr, offset);

  LinearExp();
  LinearExp(Expr slope, Expr offset);

  eq_class(LinearExp);
};
std::ostream &operator<<(std::ostream &out, const LinearExp &le);

struct LExpBinOp {
  member_class(Expr, left);
  member_class(Op, op);
  member_class(Expr, right);

  LExpBinOp();
  LExpBinOp(Expr left, Op op, Expr right);

  eq_class(LExpBinOp);
};
std::ostream &operator<<(std::ostream &out, const LExpBinOp &lbop);

} // namespace AST

} // namespace SBG

#endif
