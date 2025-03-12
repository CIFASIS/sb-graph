/** @file eval_dsbg.hpp

 @brief <b>DSBG expression evaluator</b>

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

#ifndef AST_VISITOR_EVAL_DSBG
#define AST_VISITOR_EVAL_DSBG

#include "eval/visitors/eval_pw.hpp"

namespace SBG {

namespace Eval {

struct EvalDSBG : public boost::static_visitor<LIB::DSBG> {
  public:
  EvalDSBG(unsigned int nmbr_dims, const LIB::PWMapAF &fact, VarEnv &env);

  LIB::DSBG operator()(AST::Natural v) const;
  LIB::DSBG operator()(AST::Rational v) const;
  LIB::DSBG operator()(AST::VariableName v) const;
  LIB::DSBG operator()(AST::UnaryOp v) const;
  LIB::DSBG operator()(AST::BinOp v) const;
  LIB::DSBG operator()(AST::Call v) const;
  LIB::DSBG operator()(AST::Interval v) const;
  LIB::DSBG operator()(AST::InterUnaryOp v) const;
  LIB::DSBG operator()(AST::InterBinOp v) const;
  LIB::DSBG operator()(AST::MultiDimInter v) const;
  LIB::DSBG operator()(AST::MDInterUnaryOp v) const;
  LIB::DSBG operator()(AST::MDInterBinOp v) const;
  LIB::DSBG operator()(AST::Set v) const;
  LIB::DSBG operator()(AST::SetUnaryOp v) const;
  LIB::DSBG operator()(AST::SetBinOp v) const;
  LIB::DSBG operator()(AST::LinearExp v) const;
  LIB::DSBG operator()(AST::LExpBinOp v) const;
  LIB::DSBG operator()(AST::MDLExp v) const;
  LIB::DSBG operator()(AST::MDLExpBinOp v) const;
  LIB::DSBG operator()(AST::LinearMap v) const;
  LIB::DSBG operator()(AST::PWLMap v) const;
  LIB::DSBG operator()(AST::SBG v) const;
  LIB::DSBG operator()(AST::DSBG v) const;

  private:
  unsigned int nmbr_dims_;
  const LIB::PWMapAF &fact_;
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
