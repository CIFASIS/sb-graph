/** @file eval_base_pwmap.hpp

 @brief <b>Base piecewise linear map expression evaluator</b>

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

#ifndef AST_VISITOR_EVAL_BASE_PWMAP
#define AST_VISITOR_EVAL_BASE_PWMAP

#include "eval/visitors/eval_base_map.hpp"
#include "sbg/pw_map.hpp"

namespace SBG {

namespace Eval {

struct EvalBasePWMap : public boost::static_visitor<LIB::BasePWMap> {
  public:
  EvalBasePWMap();
  EvalBasePWMap(VarEnv env);

  LIB::BasePWMap operator()(AST::Natural v) const;
  LIB::BasePWMap operator()(AST::MDNatural v) const;
  LIB::BasePWMap operator()(AST::Rational v) const;
  LIB::BasePWMap operator()(AST::Boolean v) const;
  LIB::BasePWMap operator()(Util::VariableName v) const;
  LIB::BasePWMap operator()(AST::UnaryOp v) const;
  LIB::BasePWMap operator()(AST::BinOp v) const;
  LIB::BasePWMap operator()(AST::Call v) const;
  LIB::BasePWMap operator()(AST::Interval v) const;
  LIB::BasePWMap operator()(AST::InterUnaryOp v) const;
  LIB::BasePWMap operator()(AST::InterBinOp v) const;
  LIB::BasePWMap operator()(AST::MultiDimInter v) const;
  LIB::BasePWMap operator()(AST::MDInterUnaryOp v) const;
  LIB::BasePWMap operator()(AST::MDInterBinOp v) const;
  LIB::BasePWMap operator()(AST::Set v) const;
  LIB::BasePWMap operator()(AST::SetUnaryOp v) const;
  LIB::BasePWMap operator()(AST::SetBinOp v) const;
  LIB::BasePWMap operator()(AST::LinearExp v) const;
  LIB::BasePWMap operator()(AST::LExpBinOp v) const;
  LIB::BasePWMap operator()(AST::MDLExp v) const;
  LIB::BasePWMap operator()(AST::MDLExpBinOp v) const;
  LIB::BasePWMap operator()(AST::LinearMap v) const;
  LIB::BasePWMap operator()(AST::PWLMap v) const;
  LIB::BasePWMap operator()(AST::SBG v) const;

  private:
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
