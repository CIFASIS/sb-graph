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

#include "eval/visitors/eval_interval.hpp"

namespace SBG {

namespace Eval {

struct EvalSet : public boost::static_visitor<LIB::Set> {
  public:
  EvalSet();
  EvalSet(VarEnv env);

  LIB::Set operator()(AST::Natural v) const;
  LIB::Set operator()(AST::Rational v) const;
  LIB::Set operator()(AST::Boolean v) const;
  LIB::Set operator()(Util::VariableName v) const;
  LIB::Set operator()(AST::UnaryOp v) const;
  LIB::Set operator()(AST::BinOp v) const;
  LIB::Set operator()(AST::Call v) const;
  LIB::Set operator()(AST::Interval v) const;
  LIB::Set operator()(AST::InterUnaryOp v) const;
  LIB::Set operator()(AST::InterBinOp v) const;
  LIB::Set operator()(AST::Set v) const;
  LIB::Set operator()(AST::SetUnaryOp v) const;
  LIB::Set operator()(AST::SetBinOp v) const;
  LIB::Set operator()(AST::LinearExp v) const;
  LIB::Set operator()(AST::LExpBinOp v) const;
  LIB::Set operator()(AST::LinearMap v) const;
  LIB::Set operator()(AST::PWLMap v) const;

  private:
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
