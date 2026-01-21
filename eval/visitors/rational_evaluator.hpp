/** @file rational_evaluator.hpp

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

#ifndef RATIONAL_EVALUATOR 
#define RATIONAL_EVALUATOR 

#include "eval/var_env.hpp"
#include "sbg/rational.hpp"

namespace SBG {

namespace Eval {

class RationalEvaluator : public boost::static_visitor<LIB::RATIONAL> {
  public:
  RationalEvaluator();
  RationalEvaluator(VarEnv &venv);

  LIB::RATIONAL operator()(AST::Natural v) const;
  LIB::RATIONAL operator()(AST::Rational v) const;
  LIB::RATIONAL operator()(AST::Name v) const;
  LIB::RATIONAL operator()(AST::UnaryOp v) const;
  LIB::RATIONAL operator()(AST::BinOp v) const;
  LIB::RATIONAL operator()(AST::Call v) const;
  LIB::RATIONAL operator()(AST::Interval v) const;
  LIB::RATIONAL operator()(AST::MultiDimInter v) const;
  LIB::RATIONAL operator()(AST::Set v) const;
  LIB::RATIONAL operator()(AST::LinearExp v) const;
  LIB::RATIONAL operator()(AST::MDLExp v) const;
  LIB::RATIONAL operator()(AST::LinearMap v) const;
  LIB::RATIONAL operator()(AST::PWLMap v) const;
  LIB::RATIONAL operator()(AST::SBG v) const;
  LIB::RATIONAL operator()(AST::BipartiteSBG v) const;
  LIB::RATIONAL operator()(AST::DSBG v) const;
  LIB::RATIONAL operator()(AST::ParenExpr) const;

  private:
  mutable VarEnv venv_;
};

} // namespace Eval

} // namespace SBG

#endif
