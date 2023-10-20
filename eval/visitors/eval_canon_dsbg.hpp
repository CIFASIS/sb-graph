/** @file eval_canon_dsbg.hpp

 @brief <b>Canon DSBG expression evaluator</b>

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

#ifndef AST_VISITOR_EVAL_CANON_DSBG
#define AST_VISITOR_EVAL_CANON_DSBG

#include "eval/visitors/eval_canon_pwmap.hpp"
#include "sbg/sbg.hpp"

namespace SBG {

namespace Eval {

struct EvalCanonDSBG : public boost::static_visitor<LIB::CanonDSBG> {
  public:
  EvalCanonDSBG();
  EvalCanonDSBG(VarEnv env);

  LIB::CanonDSBG operator()(AST::Natural v) const;
  LIB::CanonDSBG operator()(AST::MDNatural v) const;
  LIB::CanonDSBG operator()(AST::Rational v) const;
  LIB::CanonDSBG operator()(AST::Boolean v) const;
  LIB::CanonDSBG operator()(Util::VariableName v) const;
  LIB::CanonDSBG operator()(AST::UnaryOp v) const;
  LIB::CanonDSBG operator()(AST::BinOp v) const;
  LIB::CanonDSBG operator()(AST::Call v) const;
  LIB::CanonDSBG operator()(AST::Interval v) const;
  LIB::CanonDSBG operator()(AST::InterUnaryOp v) const;
  LIB::CanonDSBG operator()(AST::InterBinOp v) const;
  LIB::CanonDSBG operator()(AST::MultiDimInter v) const;
  LIB::CanonDSBG operator()(AST::MDInterUnaryOp v) const;
  LIB::CanonDSBG operator()(AST::MDInterBinOp v) const;
  LIB::CanonDSBG operator()(AST::Set v) const;
  LIB::CanonDSBG operator()(AST::SetUnaryOp v) const;
  LIB::CanonDSBG operator()(AST::SetBinOp v) const;
  LIB::CanonDSBG operator()(AST::LinearExp v) const;
  LIB::CanonDSBG operator()(AST::LExpBinOp v) const;
  LIB::CanonDSBG operator()(AST::MDLExp v) const;
  LIB::CanonDSBG operator()(AST::MDLExpBinOp v) const;
  LIB::CanonDSBG operator()(AST::LinearMap v) const;
  LIB::CanonDSBG operator()(AST::PWLMap v) const;
  LIB::CanonDSBG operator()(AST::SBG v) const;
  LIB::CanonDSBG operator()(AST::DSBG v) const;

  private:
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
