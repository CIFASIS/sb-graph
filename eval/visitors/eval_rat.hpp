/** @file eval_rat.hpp

 @brief <b>Rational expression evaluator</b>

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

#ifndef AST_VISITOR_EVALRAT
#define AST_VISITOR_EVALRAT

#include "eval/defs.hpp"
#include "eval/visitors/eval_int.hpp"

namespace SBG {

namespace Eval {

struct EvalRat : public boost::static_visitor<LIB::RATIONAL> {
  public:
  EvalRat();
  EvalRat(VarEnv &env);

  LIB::RATIONAL operator()(AST::Natural v) const;
  LIB::RATIONAL operator()(AST::Rational v) const;
  LIB::RATIONAL operator()(AST::VariableName v) const;
  LIB::RATIONAL operator()(AST::UnaryOp v) const;
  LIB::RATIONAL operator()(AST::BinOp v) const;
  LIB::RATIONAL operator()(AST::Call v) const;
  LIB::RATIONAL operator()(AST::Interval v) const;
  LIB::RATIONAL operator()(AST::InterUnaryOp v) const;
  LIB::RATIONAL operator()(AST::InterBinOp v) const;
  LIB::RATIONAL operator()(AST::MultiDimInter v) const;
  LIB::RATIONAL operator()(AST::MDInterUnaryOp v) const;
  LIB::RATIONAL operator()(AST::MDInterBinOp v) const;
  LIB::RATIONAL operator()(AST::Set v) const;
  LIB::RATIONAL operator()(AST::SetUnaryOp v) const;
  LIB::RATIONAL operator()(AST::SetBinOp v) const;
  LIB::RATIONAL operator()(AST::LinearExp v) const;
  LIB::RATIONAL operator()(AST::LExpBinOp v) const;
  LIB::RATIONAL operator()(AST::MDLExp v) const;
  LIB::RATIONAL operator()(AST::MDLExpBinOp v) const;
  LIB::RATIONAL operator()(AST::LinearMap v) const;
  LIB::RATIONAL operator()(AST::PWLMap v) const;
  LIB::RATIONAL operator()(AST::SBG v) const;
  LIB::RATIONAL operator()(AST::DSBG v) const;

  private:
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
