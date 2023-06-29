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

#include <boost/variant/static_visitor.hpp>

#include <ast/expr.hpp>
#include <eval/defs.hpp>
#include <eval/visitors/eval_int.hpp>

namespace SBG {

namespace Eval {

class EvalInterval : public boost::static_visitor<SBG::Interval> {
  public:
  EvalInterval();
  EvalInterval(VarEnv env);

  SBG::Interval operator()(AST::Integer v) const;
  SBG::Interval operator()(AST::Rational v) const;
  SBG::Interval operator()(AST::Boolean v) const;
  SBG::Interval operator()(Util::VariableName v) const;
  SBG::Interval operator()(AST::BinOp v) const;
  SBG::Interval operator()(AST::Call v) const;
  SBG::Interval operator()(AST::Interval i) const;
  SBG::Interval operator()(AST::InterBinOp i) const;

  private:
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif