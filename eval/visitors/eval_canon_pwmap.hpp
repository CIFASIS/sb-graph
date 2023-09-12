/** @file eval_canon_pwmap.hpp

 @brief <b>Canon piecewise linear map expression evaluator</b>

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

#ifndef AST_VISITOR_EVAL_CANON_PWMAP
#define AST_VISITOR_EVAL_CANON_PWMAP

#include "eval/visitors/eval_canon_map.hpp"
#include "sbg/pw_map.hpp"

namespace SBG {

namespace Eval {

struct EvalCanonPWMap : public boost::static_visitor<LIB::CanonPWMap> {
  public:
  EvalCanonPWMap();
  EvalCanonPWMap(VarEnv env);

  LIB::CanonPWMap operator()(AST::Natural v) const;
  LIB::CanonPWMap operator()(AST::MDNatural v) const;
  LIB::CanonPWMap operator()(AST::Rational v) const;
  LIB::CanonPWMap operator()(AST::Boolean v) const;
  LIB::CanonPWMap operator()(Util::VariableName v) const;
  LIB::CanonPWMap operator()(AST::UnaryOp v) const;
  LIB::CanonPWMap operator()(AST::BinOp v) const;
  LIB::CanonPWMap operator()(AST::Call v) const;
  LIB::CanonPWMap operator()(AST::Interval v) const;
  LIB::CanonPWMap operator()(AST::InterUnaryOp v) const;
  LIB::CanonPWMap operator()(AST::InterBinOp v) const;
  LIB::CanonPWMap operator()(AST::MultiDimInter v) const;
  LIB::CanonPWMap operator()(AST::MDInterUnaryOp v) const;
  LIB::CanonPWMap operator()(AST::MDInterBinOp v) const;
  LIB::CanonPWMap operator()(AST::Set v) const;
  LIB::CanonPWMap operator()(AST::SetUnaryOp v) const;
  LIB::CanonPWMap operator()(AST::SetBinOp v) const;
  LIB::CanonPWMap operator()(AST::LinearExp v) const;
  LIB::CanonPWMap operator()(AST::LExpBinOp v) const;
  LIB::CanonPWMap operator()(AST::MDLExp v) const;
  LIB::CanonPWMap operator()(AST::MDLExpBinOp v) const;
  LIB::CanonPWMap operator()(AST::LinearMap v) const;
  LIB::CanonPWMap operator()(AST::PWLMap v) const;

  private:
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
