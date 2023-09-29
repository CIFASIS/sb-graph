/** @file eval_mdi.hpp

 @brief <b>Multi-dimensional interval expression evaluator</b>

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

#ifndef AST_VISITOR_EVAL_MDI
#define AST_VISITOR_EVAL_MDI

#include "eval/visitors/eval_interval.hpp"

namespace SBG {

namespace Eval {

struct EvalMDI : public boost::static_visitor<LIB::SetPiece> {
  public:
  EvalMDI();
  EvalMDI(VarEnv env);

  LIB::SetPiece operator()(AST::Natural v) const;
  LIB::SetPiece operator()(AST::MDNatural v) const;
  LIB::SetPiece operator()(AST::Rational v) const;
  LIB::SetPiece operator()(AST::Boolean v) const;
  LIB::SetPiece operator()(Util::VariableName v) const;
  LIB::SetPiece operator()(AST::UnaryOp v) const;
  LIB::SetPiece operator()(AST::BinOp v) const;
  LIB::SetPiece operator()(AST::Call v) const;
  LIB::SetPiece operator()(AST::Interval v) const;
  LIB::SetPiece operator()(AST::InterUnaryOp v) const;
  LIB::SetPiece operator()(AST::InterBinOp v) const;
  LIB::SetPiece operator()(AST::MultiDimInter v) const;
  LIB::SetPiece operator()(AST::MDInterUnaryOp v) const;
  LIB::SetPiece operator()(AST::MDInterBinOp v) const;
  LIB::SetPiece operator()(AST::Set v) const;
  LIB::SetPiece operator()(AST::SetUnaryOp v) const;
  LIB::SetPiece operator()(AST::SetBinOp v) const;
  LIB::SetPiece operator()(AST::LinearExp v) const;
  LIB::SetPiece operator()(AST::LExpBinOp v) const;
  LIB::SetPiece operator()(AST::MDLExp v) const;
  LIB::SetPiece operator()(AST::MDLExpBinOp v) const;
  LIB::SetPiece operator()(AST::LinearMap v) const;
  LIB::SetPiece operator()(AST::PWLMap v) const;
  LIB::SetPiece operator()(AST::SBG v) const;

  private:
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
