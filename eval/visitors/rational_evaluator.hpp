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

#ifndef SBGRAPH_EVAL_VISITORS_RATIONAL_EVALUATOR_HPP_ 
#define SBGRAPH_EVAL_VISITORS_RATIONAL_EVALUATOR_HPP_

#include "ast/expr.hpp"
#include "eval/var_env.hpp"
#include "sbg/rational.hpp"

#include "boost/variant.hpp"

namespace SBG {

namespace Eval {

namespace detail {

class RationalEvaluator : public boost::static_visitor<LIB::Rational> {
public:
  RationalEvaluator();
  RationalEvaluator(VarEnv &venv);

  LIB::Rational operator()(AST::Integer v) const;
  LIB::Rational operator()(AST::Rational v) const;
  LIB::Rational operator()(AST::Name v) const;
  LIB::Rational operator()(AST::UnaryOp v) const;
  LIB::Rational operator()(AST::BinOp v) const;
  LIB::Rational operator()(AST::Call v) const;
  LIB::Rational operator()(AST::Interval v) const;
  LIB::Rational operator()(AST::MultiDimInter v) const;
  LIB::Rational operator()(AST::Set v) const;
  LIB::Rational operator()(AST::LinearExp v) const;
  LIB::Rational operator()(AST::MDLExp v) const;
  LIB::Rational operator()(AST::LinearMap v) const;
  LIB::Rational operator()(AST::PWLMap v) const;
  LIB::Rational operator()(AST::SBG v) const;
  LIB::Rational operator()(AST::BipartiteSBG v) const;
  LIB::Rational operator()(AST::DSBG v) const;
  LIB::Rational operator()(AST::ParenExpr) const;

private:
  mutable VarEnv _venv;
};

} // namespace detail

} // namespace Eval

} // namespace SBG

#endif // SBGRAPH_EVAL_VISITORS_RATIONAL_EVALUATOR_HPP_
