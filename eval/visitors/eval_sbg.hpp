/** @file eval_sbg.hpp

 @brief <b>SBG expression evaluator</b>

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

#ifndef AST_VISITOR_EVAL_SBG
#define AST_VISITOR_EVAL_SBG

#include "eval/visitors/eval_pw.hpp"

namespace SBG {

namespace Eval {

struct EvalSBG : public boost::static_visitor<LIB::SBG> {
  public:
  EvalSBG(unsigned int nmbr_dims, const LIB::PWMapAF &fact, VarEnv &env);

  LIB::SBG operator()(AST::Natural v) const;
  LIB::SBG operator()(AST::Rational v) const;
  LIB::SBG operator()(AST::VariableName v) const;
  LIB::SBG operator()(AST::UnaryOp v) const;
  LIB::SBG operator()(AST::BinOp v) const;
  LIB::SBG operator()(AST::Call v) const;
  LIB::SBG operator()(AST::Interval v) const;
  LIB::SBG operator()(AST::InterUnaryOp v) const;
  LIB::SBG operator()(AST::InterBinOp v) const;
  LIB::SBG operator()(AST::MultiDimInter v) const;
  LIB::SBG operator()(AST::MDInterUnaryOp v) const;
  LIB::SBG operator()(AST::MDInterBinOp v) const;
  LIB::SBG operator()(AST::Set v) const;
  LIB::SBG operator()(AST::SetUnaryOp v) const;
  LIB::SBG operator()(AST::SetBinOp v) const;
  LIB::SBG operator()(AST::LinearExp v) const;
  LIB::SBG operator()(AST::LExpBinOp v) const;
  LIB::SBG operator()(AST::MDLExp v) const;
  LIB::SBG operator()(AST::MDLExpBinOp v) const;
  LIB::SBG operator()(AST::LinearMap v) const;
  LIB::SBG operator()(AST::PWLMap v) const;
  LIB::SBG operator()(AST::SBG v) const;
  LIB::SBG operator()(AST::DSBG v) const;

  private:
  unsigned int nmbr_dims_;
  const LIB::PWMapAF &fact_;
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
