/** @file eval_mdle.hpp

 @brief <b>Multi-dimensional linear expression evaluator</b>

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

#ifndef AST_VISITOR_EVAL_MDLE
#define AST_VISITOR_EVAL_MDLE

#include "eval/visitors/eval_le.hpp"
#include "sbg/multidim_lexp.hpp"

namespace SBG {

namespace Eval {

struct EvalMDLE : public boost::static_visitor<LIB::MDLExp> {
  public:
  EvalMDLE();
  EvalMDLE(VarEnv env);

  LIB::MDLExp operator()(AST::Natural v) const;
  LIB::MDLExp operator()(AST::MDNatural v) const;
  LIB::MDLExp operator()(AST::Rational v) const;
  LIB::MDLExp operator()(AST::Boolean v) const;
  LIB::MDLExp operator()(Util::VariableName v) const;
  LIB::MDLExp operator()(AST::UnaryOp v) const;
  LIB::MDLExp operator()(AST::BinOp v) const;
  LIB::MDLExp operator()(AST::Call v) const;
  LIB::MDLExp operator()(AST::Interval v) const;
  LIB::MDLExp operator()(AST::InterUnaryOp v) const;
  LIB::MDLExp operator()(AST::InterBinOp v) const;
  LIB::MDLExp operator()(AST::MultiDimInter v) const;
  LIB::MDLExp operator()(AST::MDInterUnaryOp v) const;
  LIB::MDLExp operator()(AST::MDInterBinOp v) const;
  LIB::MDLExp operator()(AST::Set v) const;
  LIB::MDLExp operator()(AST::SetUnaryOp v) const;
  LIB::MDLExp operator()(AST::SetBinOp v) const;
  LIB::MDLExp operator()(AST::LinearExp v) const;
  LIB::MDLExp operator()(AST::LExpBinOp v) const;
  LIB::MDLExp operator()(AST::MDLExp v) const;
  LIB::MDLExp operator()(AST::MDLExpBinOp v) const;
  LIB::MDLExp operator()(AST::LinearMap v) const;
  LIB::MDLExp operator()(AST::PWLMap v) const;

  private:
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
