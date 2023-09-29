/** @file eval_base_sbg.hpp

 @brief <b>Base SBG expression evaluator</b>

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

#ifndef AST_VISITOR_EVAL_BASE_SBG
#define AST_VISITOR_EVAL_BASE_SBG

#include "eval/visitors/eval_base_pwmap.hpp"
#include "sbg/sbg.hpp"

namespace SBG {

namespace Eval {

struct EvalBaseSBG : public boost::static_visitor<LIB::BaseSBG> {
  public:
  EvalBaseSBG();
  EvalBaseSBG(VarEnv env);

  LIB::BaseSBG operator()(AST::Natural v) const;
  LIB::BaseSBG operator()(AST::MDNatural v) const;
  LIB::BaseSBG operator()(AST::Rational v) const;
  LIB::BaseSBG operator()(AST::Boolean v) const;
  LIB::BaseSBG operator()(Util::VariableName v) const;
  LIB::BaseSBG operator()(AST::UnaryOp v) const;
  LIB::BaseSBG operator()(AST::BinOp v) const;
  LIB::BaseSBG operator()(AST::Call v) const;
  LIB::BaseSBG operator()(AST::Interval v) const;
  LIB::BaseSBG operator()(AST::InterUnaryOp v) const;
  LIB::BaseSBG operator()(AST::InterBinOp v) const;
  LIB::BaseSBG operator()(AST::MultiDimInter v) const;
  LIB::BaseSBG operator()(AST::MDInterUnaryOp v) const;
  LIB::BaseSBG operator()(AST::MDInterBinOp v) const;
  LIB::BaseSBG operator()(AST::Set v) const;
  LIB::BaseSBG operator()(AST::SetUnaryOp v) const;
  LIB::BaseSBG operator()(AST::SetBinOp v) const;
  LIB::BaseSBG operator()(AST::LinearExp v) const;
  LIB::BaseSBG operator()(AST::LExpBinOp v) const;
  LIB::BaseSBG operator()(AST::MDLExp v) const;
  LIB::BaseSBG operator()(AST::MDLExpBinOp v) const;
  LIB::BaseSBG operator()(AST::LinearMap v) const;
  LIB::BaseSBG operator()(AST::PWLMap v) const;
  LIB::BaseSBG operator()(AST::SBG v) const;

  private:
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
