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

#include <ast/expr.hpp>

namespace SBG {

namespace AST {

// Arithmetic expressions ------------------------------------------------------

const char *OpNames[] = {"+", "-", "*", "^"};

std::ostream &operator<<(std::ostream &out, const Op &op)
{
  out << OpNames[static_cast<int>(op)];

  return out;
}

std::ostream &operator<<(std::ostream &out, const InterOp &op)
{
  out << InterOpNames[static_cast<int>(op)];

  return out;
}

std::ostream &operator<<(std::ostream &out, const ExprList &el)
{
  BOOST_FOREACH (Expr e, el)
    out << e << "\n";

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
      out << c.args()[i] << ",";
    out << c.args()[i];
  }
  out << ")"; 

  return out;
}

// SBG Structures --------------------------------------------------------------

const char *InterOpNames[] = {"/\\"};

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

std::ostream &operator<<(std::ostream &out, const Intervals &ii)
{
  BOOST_FOREACH (Interval i, ii)
    out << i << "\n";

  return out;
}

InterBinOp::InterBinOp() : left_(), op_(), right_() {}
InterBinOp::InterBinOp(Expr left, InterOp op, Expr right) : left_(left), op_(op), right_(right) {}

member_imp(InterBinOp, Expr, left);
member_imp(InterBinOp, InterOp, op);
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

} // namespace AST

} // namespace SBG
