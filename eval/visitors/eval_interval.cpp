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

#include <eval/visitors/eval_interval.hpp>

namespace SBG {

namespace Eval {

EvalInterval::EvalInterval() : env_() {}
EvalInterval::EvalInterval(VarEnv env) : env_(env) {}

SBG::Interval EvalInterval::operator()(AST::Integer v) const { 
  Util::ERROR("EvalInterval: trying to evaluate an Integer");
  return SBG::Interval(1, 0, 0); 
}

SBG::Interval EvalInterval::operator()(AST::Rational v) const { 
  Util::ERROR("EvalInterval: trying to evaluate a Rational");
  return SBG::Interval(1, 0, 0); 
}

SBG::Interval EvalInterval::operator()(AST::Boolean v) const { 
  Util::ERROR("EvalInterval: trying to evaluate a Boolean");
  return SBG::Interval(1, 0, 0); 
}

SBG::Interval EvalInterval::operator()(Util::VariableName v) const { 
  Util::Option<ExprBaseType> v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (is<SBG::Interval>(value))
      return boost::get<SBG::Interval>(value);

    else {
      Util::ERROR("EvalInterval: variable %s is not an interval", v);
      return SBG::Interval(1, 0, 0); 
    } 
  }

  Util::ERROR("EvalInterval: variable %s not defined", v);
  return SBG::Interval(1, 0, 0); 
}

SBG::Interval EvalInterval::operator()(AST::BinOp v) const 
{
  Util::ERROR("EvalInterval: trying to evaluate an arithmetic BinOp");
  return SBG::Interval(1, 0, 0); 
}

SBG::Interval EvalInterval::operator()(AST::Call v) const
{
  Util::ERROR("EvalInterval: trying to evaluate a Call");
  return SBG::Interval(1, 0, 0); 
}

SBG::Interval EvalInterval::operator()(AST::Interval i) const { 
  EvalInt eval_int(env_);

  return SBG::Interval(Apply(eval_int, i.begin()), Apply(eval_int, i.step()), Apply(eval_int, i.end())); 
}

SBG::Interval EvalInterval::operator()(AST::InterBinOp i) const
{
  AST::Expr l = i.left(), r = i.right();
  switch (i.op()){
    case AST::InterOp::cap:
      return intersection(ApplyThis(l), ApplyThis(r));

    default:
      Util::ERROR("EvalInterval: InterBinOp %s not supported.", AST::InterOpNames[static_cast<int>(i.op())]);
      return SBG::Interval(1, 0, 0); 
  }
}

} // namespace Eval

} // namespace SBG
