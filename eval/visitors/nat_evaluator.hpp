/** @file nat_evaluator.hpp

 @brief <b>Natural expression evaluator</b>

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

#ifndef SBGRAPH_EVAL_VISITORS_NAT_EVALUATOR_HPP_ 
#define SBGRAPH_EVAL_VISITORS_NAT_EVALUATOR_HPP_

#include "ast/expr.hpp"
#include "eval/var_env.hpp"
#include "sbg/natural.hpp"

#include "boost/variant.hpp"

namespace SBG {

namespace Eval {

namespace detail {

class NatEvaluator : public boost::static_visitor<LIB::NAT> {
public:
  NatEvaluator();
  NatEvaluator(VarEnv &venv);

  LIB::NAT operator()(AST::Natural v) const;
  LIB::NAT operator()(AST::Rational v) const;
  LIB::NAT operator()(AST::Name v) const;
  LIB::NAT operator()(AST::UnaryOp v) const;
  LIB::NAT operator()(AST::BinOp v) const;
  LIB::NAT operator()(AST::Call v) const;
  LIB::NAT operator()(AST::Interval v) const;
  LIB::NAT operator()(AST::MultiDimInter v) const;
  LIB::NAT operator()(AST::Set v) const;
  LIB::NAT operator()(AST::LinearExp v) const;
  LIB::NAT operator()(AST::MDLExp v) const;
  LIB::NAT operator()(AST::LinearMap v) const;
  LIB::NAT operator()(AST::PWLMap v) const;
  LIB::NAT operator()(AST::SBG v) const;
  LIB::NAT operator()(AST::BipartiteSBG v) const;
  LIB::NAT operator()(AST::DSBG v) const;
  LIB::NAT operator()(AST::ParenExpr) const;

private:
  mutable VarEnv _venv;
};

} // namespace detail

} // namespace Eval

} // namespace SBG

#endif // SBGRAPH_EVAL_VISITORS_NAT_EVALUTOR_HPP_
