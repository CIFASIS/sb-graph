/** @file eval_set.hpp

 @brief <b>Set expression evaluator</b>

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

#ifndef AST_VISITOR_EVALSET
#define AST_VISITOR_EVALSET

#include <eval/visitors/eval_interval.hpp>

namespace SBG {

namespace Eval {

struct EvalSet : public boost::static_visitor<SBG::Set> {
  public:
  EvalSet();
  EvalSet(VarEnv env);

  SBG::Set operator()(AST::Integer v) const;
  SBG::Set operator()(AST::Rational v) const;
  SBG::Set operator()(AST::Boolean v) const;
  SBG::Set operator()(Util::VariableName v) const;
  SBG::Set operator()(AST::BinOp v) const;
  SBG::Set operator()(AST::Call v) const;
  SBG::Set operator()(AST::Interval i) const;
  SBG::Set operator()(AST::InterUnaryOp i) const;
  SBG::Set operator()(AST::InterBinOp i) const;
  SBG::Set operator()(AST::Set i) const;
  SBG::Set operator()(AST::SetUnaryOp i) const;
  SBG::Set operator()(AST::SetBinOp i) const;

  private:
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
