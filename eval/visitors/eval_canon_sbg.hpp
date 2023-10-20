/** @file eval_canon_sbg.hpp

 @brief <b>Canon SBG expression evaluator</b>

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

#ifndef AST_VISITOR_EVAL_CANON_SBG
#define AST_VISITOR_EVAL_CANON_SBG

#include "eval/visitors/eval_canon_pwmap.hpp"
#include "sbg/sbg.hpp"

namespace SBG {

namespace Eval {

struct EvalCanonSBG : public boost::static_visitor<LIB::CanonSBG> {
  public:
  EvalCanonSBG();
  EvalCanonSBG(VarEnv env);

  LIB::CanonSBG operator()(AST::Natural v) const;
  LIB::CanonSBG operator()(AST::MDNatural v) const;
  LIB::CanonSBG operator()(AST::Rational v) const;
  LIB::CanonSBG operator()(AST::Boolean v) const;
  LIB::CanonSBG operator()(Util::VariableName v) const;
  LIB::CanonSBG operator()(AST::UnaryOp v) const;
  LIB::CanonSBG operator()(AST::BinOp v) const;
  LIB::CanonSBG operator()(AST::Call v) const;
  LIB::CanonSBG operator()(AST::Interval v) const;
  LIB::CanonSBG operator()(AST::InterUnaryOp v) const;
  LIB::CanonSBG operator()(AST::InterBinOp v) const;
  LIB::CanonSBG operator()(AST::MultiDimInter v) const;
  LIB::CanonSBG operator()(AST::MDInterUnaryOp v) const;
  LIB::CanonSBG operator()(AST::MDInterBinOp v) const;
  LIB::CanonSBG operator()(AST::Set v) const;
  LIB::CanonSBG operator()(AST::SetUnaryOp v) const;
  LIB::CanonSBG operator()(AST::SetBinOp v) const;
  LIB::CanonSBG operator()(AST::LinearExp v) const;
  LIB::CanonSBG operator()(AST::LExpBinOp v) const;
  LIB::CanonSBG operator()(AST::MDLExp v) const;
  LIB::CanonSBG operator()(AST::MDLExpBinOp v) const;
  LIB::CanonSBG operator()(AST::LinearMap v) const;
  LIB::CanonSBG operator()(AST::PWLMap v) const;
  LIB::CanonSBG operator()(AST::SBG v) const;
  LIB::CanonSBG operator()(AST::DSBG v) const;

  private:
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
