/** @file eval_base_dsbg.hpp

 @brief <b>Base DSBG expression evaluator</b>

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

#ifndef AST_VISITOR_EVAL_BASE_DSBG
#define AST_VISITOR_EVAL_BASE_DSBG

#include "eval/visitors/eval_base_pwmap.hpp"
#include "sbg/sbg.hpp"

namespace SBG {

namespace Eval {

struct EvalBaseDSBG : public boost::static_visitor<LIB::BaseDSBG> {
  public:
  EvalBaseDSBG();
  EvalBaseDSBG(VarEnv env);

  LIB::BaseDSBG operator()(AST::Natural v) const;
  LIB::BaseDSBG operator()(AST::MDNatural v) const;
  LIB::BaseDSBG operator()(AST::Rational v) const;
  LIB::BaseDSBG operator()(AST::Boolean v) const;
  LIB::BaseDSBG operator()(Util::VariableName v) const;
  LIB::BaseDSBG operator()(AST::UnaryOp v) const;
  LIB::BaseDSBG operator()(AST::BinOp v) const;
  LIB::BaseDSBG operator()(AST::Call v) const;
  LIB::BaseDSBG operator()(AST::Interval v) const;
  LIB::BaseDSBG operator()(AST::InterUnaryOp v) const;
  LIB::BaseDSBG operator()(AST::InterBinOp v) const;
  LIB::BaseDSBG operator()(AST::MultiDimInter v) const;
  LIB::BaseDSBG operator()(AST::MDInterUnaryOp v) const;
  LIB::BaseDSBG operator()(AST::MDInterBinOp v) const;
  LIB::BaseDSBG operator()(AST::Set v) const;
  LIB::BaseDSBG operator()(AST::SetUnaryOp v) const;
  LIB::BaseDSBG operator()(AST::SetBinOp v) const;
  LIB::BaseDSBG operator()(AST::LinearExp v) const;
  LIB::BaseDSBG operator()(AST::LExpBinOp v) const;
  LIB::BaseDSBG operator()(AST::MDLExp v) const;
  LIB::BaseDSBG operator()(AST::MDLExpBinOp v) const;
  LIB::BaseDSBG operator()(AST::LinearMap v) const;
  LIB::BaseDSBG operator()(AST::PWLMap v) const;
  LIB::BaseDSBG operator()(AST::SBG v) const;
  LIB::BaseDSBG operator()(AST::DSBG v) const;

  private:
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
