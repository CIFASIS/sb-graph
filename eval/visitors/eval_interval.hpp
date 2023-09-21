/** @file eval_interval.hpp

 @brief <b>Interval expression evaluator</b>

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

#ifndef AST_VISITOR_EVALINTERVAL
#define AST_VISITOR_EVALINTERVAL

#include "eval/visitors/eval_nat.hpp"

namespace SBG {

namespace Eval {

struct EvalInterval : public boost::static_visitor<LIB::Interval> {
  public:
  EvalInterval();
  EvalInterval(VarEnv env);

  LIB::Interval operator()(AST::Natural v) const;
  LIB::Interval operator()(AST::MDNatural v) const;
  LIB::Interval operator()(AST::Rational v) const;
  LIB::Interval operator()(AST::Boolean v) const;
  LIB::Interval operator()(Util::VariableName v) const;
  LIB::Interval operator()(AST::UnaryOp v) const;
  LIB::Interval operator()(AST::BinOp v) const;
  LIB::Interval operator()(AST::Call v) const;
  LIB::Interval operator()(AST::Interval v) const;
  LIB::Interval operator()(AST::InterUnaryOp v) const;
  LIB::Interval operator()(AST::InterBinOp v) const;
  LIB::Interval operator()(AST::MultiDimInter v) const;
  LIB::Interval operator()(AST::MDInterUnaryOp v) const;
  LIB::Interval operator()(AST::MDInterBinOp v) const;
  LIB::Interval operator()(AST::Set v) const;
  LIB::Interval operator()(AST::SetUnaryOp v) const;
  LIB::Interval operator()(AST::SetBinOp v) const;
  LIB::Interval operator()(AST::LinearExp v) const;
  LIB::Interval operator()(AST::LExpBinOp v) const;
  LIB::Interval operator()(AST::MDLExp v) const;
  LIB::Interval operator()(AST::MDLExpBinOp v) const;
  LIB::Interval operator()(AST::LinearMap v) const;
  LIB::Interval operator()(AST::PWLMap v) const;

  private:
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
