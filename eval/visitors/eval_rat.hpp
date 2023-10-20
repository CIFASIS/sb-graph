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

namespace SBG {

namespace Eval {

struct EvalRat : public boost::static_visitor<Util::RATIONAL> {
  public:
  EvalRat();
  EvalRat(VarEnv env);

  Util::RATIONAL operator()(AST::Natural v) const;
  Util::RATIONAL operator()(AST::MDNatural v) const;
  Util::RATIONAL operator()(AST::Rational v) const;
  Util::RATIONAL operator()(AST::Boolean v) const;
  Util::RATIONAL operator()(Util::VariableName v) const;
  Util::RATIONAL operator()(AST::UnaryOp v) const;
  Util::RATIONAL operator()(AST::BinOp v) const;
  Util::RATIONAL operator()(AST::Call v) const;
  Util::RATIONAL operator()(AST::Interval v) const;
  Util::RATIONAL operator()(AST::InterUnaryOp v) const;
  Util::RATIONAL operator()(AST::InterBinOp v) const;
  Util::RATIONAL operator()(AST::MultiDimInter v) const;
  Util::RATIONAL operator()(AST::MDInterUnaryOp v) const;
  Util::RATIONAL operator()(AST::MDInterBinOp v) const;
  Util::RATIONAL operator()(AST::Set v) const;
  Util::RATIONAL operator()(AST::SetUnaryOp v) const;
  Util::RATIONAL operator()(AST::SetBinOp v) const;
  Util::RATIONAL operator()(AST::LinearExp v) const;
  Util::RATIONAL operator()(AST::LExpBinOp v) const;
  Util::RATIONAL operator()(AST::MDLExp v) const;
  Util::RATIONAL operator()(AST::MDLExpBinOp v) const;
  Util::RATIONAL operator()(AST::LinearMap v) const;
  Util::RATIONAL operator()(AST::PWLMap v) const;
  Util::RATIONAL operator()(AST::SBG v) const;
  Util::RATIONAL operator()(AST::DSBG v) const;

  private:
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
