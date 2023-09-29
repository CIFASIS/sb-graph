/** @file eval_le.hpp

 @brief <b>Linear expression evaluator</b>

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

#ifndef AST_VISITOR_EVALLE
#define AST_VISITOR_EVALLE

#include "eval/visitors/eval_nat.hpp"
#include "eval/visitors/eval_rat.hpp"
#include "sbg/lexp.hpp"

namespace SBG {

namespace Eval {

struct EvalLE : public boost::static_visitor<LIB::LExp> {
  public:
  EvalLE();
  EvalLE(VarEnv env);

  LIB::LExp operator()(AST::Natural v) const;
  LIB::LExp operator()(AST::MDNatural v) const;
  LIB::LExp operator()(AST::Rational v) const;
  LIB::LExp operator()(AST::Boolean v) const;
  LIB::LExp operator()(Util::VariableName v) const;
  LIB::LExp operator()(AST::UnaryOp v) const;
  LIB::LExp operator()(AST::BinOp v) const;
  LIB::LExp operator()(AST::Call v) const;
  LIB::LExp operator()(AST::Interval v) const;
  LIB::LExp operator()(AST::InterUnaryOp v) const;
  LIB::LExp operator()(AST::InterBinOp v) const;
  LIB::LExp operator()(AST::MultiDimInter v) const;
  LIB::LExp operator()(AST::MDInterUnaryOp v) const;
  LIB::LExp operator()(AST::MDInterBinOp v) const;
  LIB::LExp operator()(AST::Set v) const;
  LIB::LExp operator()(AST::SetUnaryOp v) const;
  LIB::LExp operator()(AST::SetBinOp v) const;
  LIB::LExp operator()(AST::LinearExp v) const;
  LIB::LExp operator()(AST::LExpBinOp v) const;
  LIB::LExp operator()(AST::MDLExp v) const;
  LIB::LExp operator()(AST::MDLExpBinOp v) const;
  LIB::LExp operator()(AST::LinearMap v) const;
  LIB::LExp operator()(AST::PWLMap v) const;
  LIB::LExp operator()(AST::SBG v) const;

  private:
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
