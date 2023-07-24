/** @file eval_le.hpp

 @brief <b>Linear expression evaluator</b>

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

#ifndef AST_VISITOR_EVALLE
#define AST_VISITOR_EVALLE

#include <boost/variant/static_visitor.hpp>

#include <eval/defs.hpp>
#include <eval/visitors/eval_rat.hpp>
#include <sbg/lexp.hpp>

namespace SBG {

namespace Eval {

struct EvalLE : public boost::static_visitor<SBG::LExp> {
  public:
  EvalLE();
  EvalLE(VarEnv env);

  SBG::LExp operator()(AST::Natural v) const;
  SBG::LExp operator()(AST::Rational v) const;
  SBG::LExp operator()(AST::Boolean v) const;
  SBG::LExp operator()(Util::VariableName v) const;
  SBG::LExp operator()(AST::BinOp v) const;
  SBG::LExp operator()(AST::Call v) const;
  SBG::LExp operator()(AST::Interval v) const;
  SBG::LExp operator()(AST::InterUnaryOp v) const;
  SBG::LExp operator()(AST::InterBinOp v) const;
  SBG::LExp operator()(AST::Set v) const;
  SBG::LExp operator()(AST::SetUnaryOp v) const;
  SBG::LExp operator()(AST::SetBinOp v) const;
  SBG::LExp operator()(AST::LinearExp v) const;
  SBG::LExp operator()(AST::LExpBinOp v) const;

  private:
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
