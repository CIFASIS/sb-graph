/** @file set_impl_visitor.hpp

 @brief <b>Set Implementation Visitor</b>

 This visitor reads the input AST and decides the optimal set implementation
 that can be used for that instance. For example, to use ordered dense sets it
 checks that all intervals have step=1 and maps return dense intervals.

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

#ifndef SBGRAPH_EVAL_VISITORS_SET_IMPL_VISITOR_HPP_
#define SBGRAPH_EVAL_VISITORS_SET_IMPL_VISITOR_HPP_

#include "ast/sbg_program.hpp"
#include "eval/var_env.hpp"

#include <boost/variant.hpp>

namespace SBG {

namespace Eval {

namespace detail {

/**
 * @brief Single expression visitor to pick the optimal representation for
 * sets. The visitor will return an int value, each one representing a
 * different implementation.
 * The mapping is currently as follows:
 *   - 0: unordered sets.
 *   - 1: ordered sets.
 *   - 2: unidimensional ordered sets.
 */
class SetImplExprVisitor : public boost::static_visitor<int> {
public:
  SetImplExprVisitor(VarEnv& venv);

  int operator()(AST::Natural v) const;
  int operator()(AST::Rational v) const;
  int operator()(AST::Name v) const;
  int operator()(AST::UnaryOp v) const;
  int operator()(AST::BinOp v) const;
  int operator()(AST::Call v) const;
  int operator()(AST::Interval v) const;
  int operator()(AST::MultiDimInter v) const;
  int operator()(AST::Set v) const;
  int operator()(AST::LinearExp v) const;
  int operator()(AST::MDLExp v) const;
  int operator()(AST::LinearMap v) const;
  int operator()(AST::PWLMap v) const;
  int operator()(AST::SBG v) const;
  int operator()(AST::BipartiteSBG v) const;
  int operator()(AST::DSBG v) const;
  int operator()(AST::ParenExpr) const;

private:
  VarEnv& _venv;
};

} // namespace detail

} // namespace Eval

} // namespace SBG

#endif // SBGRAPH_EVAL_VISITORS_SET_IMPL_VISITOR_HPP_
