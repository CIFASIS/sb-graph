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

#include <eval/visitors/eval_rat.hpp>

namespace SBG {

namespace Eval {

EvalRat::EvalRat() : env_() {}
EvalRat::EvalRat(VarEnv env) : env_(env) {}

Util::RATIONAL EvalRat::operator()(AST::Integer v) const { 
  return Util::RATIONAL(v, 1); 
}

Util::RATIONAL EvalRat::operator()(AST::Rational v) const { return v; }

Util::RATIONAL EvalRat::operator()(AST::Boolean v) const { 
  if (v)
    return 1;

  return 0; 
}

Util::RATIONAL EvalRat::operator()(Util::VariableName v) const {
  Util::Option<ExprBaseType> v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (is<Util::RATIONAL>(value))
      return boost::get<Util::RATIONAL>(value);

    else {
      Util::ERROR("EvalRat: variable %s is not rational", v);
      return 0;
    }
  }

  Util::ERROR("EvalRat: variable %s not defined", v);
  return 0;
}

Util::RATIONAL EvalRat::operator()(AST::BinOp v) const 
{
  AST::Expr l = v.left(), r = v.right();
  switch (v.op()){
    case AST::Op::add:
      return ApplyThis(l) + ApplyThis(r);

    case AST::Op::sub:
      return ApplyThis(l) - ApplyThis(r);

    case AST::Op::mult:
      return ApplyThis(l) * ApplyThis(r);

    default:
      Util::ERROR("EvalRat: BinOp %s not supported.", AST::OpNames[static_cast<int>(v.op())]);
      return 0;
  }
}

Util::RATIONAL EvalRat::operator()(AST::Call v) const
{
  Util::ERROR("EvalRat: trying to evaluate a Call");
  return 0;
}

Util::RATIONAL EvalRat::operator()(AST::Interval i) const
{
  Util::ERROR("EvalRat: trying to evaluate an Interval");
  return 0;
}

Util::RATIONAL EvalRat::operator()(AST::InterBinOp i) const
{
  Util::ERROR("EvalRat: trying to evaluate an InterBinOp");
  return 0;
}

} // namespace Eval

} // namespace SBG
