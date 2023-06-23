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

#include <eval_expr.hpp>

namespace Parser {

EvalExpression::EvalExpression() : env_() {}
EvalExpression::EvalExpression(const VarSymbolTable env) : env_(env) {}

ExprType EvalExpression::operator()(Name v) const { return ApplyThis(env_ref()[v]); }

SBG::INT EvalExpression::operator()(Integer v) const { return v; }

SBG::Rational EvalExpression::operator()(Rational v) const { return v; }

SBG::INT EvalExpression::operator()(Boolean v) const { return v; }

SBG::Rational EvalExpression::operator()(BinOp v) const 
{
  Expression l = v.left(), r = v.right();
  switch (v.op()){
    case add:
      return ApplyThis(l) + ApplyThis(r);

    case sub:
      return ApplyThis(l) - ApplyThis(r);

    case mult:
      return ApplyThis(l) * ApplyThis(r);

    case expo:
      return pow(ApplyThis(l), ApplyThis(r));

    default:
      ERROR("EvalExpression: BinOp %s not supported.", BinOpTypeName[v.op()]);
      return 0;
}

SBG::Interval EvalExpression::operator()(Interval i) const
{
  Expression b = i.begin(), s = i.step(), e = i.end();

  return Interval(ApplyThis(b), ApplyThis(s), ApplyThis(e));
}

SBG::Interval EvalExpression::operator()(InterBinOp i) const
{
  Expression l = i.left(), r = i.right();
  switch (v.op()){
    case cap:
      return intersection(ApplyThis(l), ApplyThis(r));

    default:
      ERROR("EvalExpression: BinOp %s not supported.", BinOpTypeName[v.op()]);
      return 0;
}

} // namespace Parser
