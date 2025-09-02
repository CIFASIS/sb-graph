/** @file autom_impl_visitor.hpp

 @brief <b>Implementation Visitor</b>

 This visitor reads the input AST and decides the optimal implementation that
 can be used for that instance. For example, to use ordered dense sets it checks
 that all intervals have step=1 and maps return dense intervals.

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

#ifndef AST_VISITOR_AUTOM_IMPL
#define AST_VISITOR_AUTOM_IMPL

#include <boost/variant.hpp>

#include "ast/sbg_program.hpp"
#include "eval/visitors/nat_evaluator.hpp"
#include "eval/visitors/rational_evaluator.hpp"
#include "eval/visitors/stm_evaluator.hpp"

namespace SBG {

namespace Eval {

// Desired implementation of sets and pws.
// If set to "-1", the evaluator decides automatically, if not, use the
// value passed as argument in the command line.
struct Impl {
  member_class(int, set_impl);
  member_class(int, pw_impl);

  Impl(int set_impl, int pw_impl);
};

struct AutomImplVisitor : public boost::static_visitor<Impl> {
  member_class(Impl, user_impl);

  /**
   * @brief user_impl is the command line input implementation (-1 if the user
   * didn't specify a value).
   */
  AutomImplVisitor(Impl user_impl);

  Impl operator()(AST::Program p) const;

  private:
  mutable VarEnv env_;
};

struct SetImplVisitor : public boost::static_visitor<bool> {
  public:
  SetImplVisitor();
  SetImplVisitor(VarEnv env);

  bool operator()(AST::Natural v) const;
  bool operator()(AST::Rational v) const;
  bool operator()(AST::Name v) const;
  bool operator()(AST::UnaryOp v) const;
  bool operator()(AST::BinOp v) const;
  bool operator()(AST::Call v) const;
  bool operator()(AST::Interval v) const;
  bool operator()(AST::MultiDimInter v) const;
  bool operator()(AST::Set v) const;
  bool operator()(AST::LinearExp v) const;
  bool operator()(AST::MDLExp v) const;
  bool operator()(AST::LinearMap v) const;
  bool operator()(AST::PWLMap v) const;
  bool operator()(AST::SBG v) const;
  bool operator()(AST::DSBG v) const;
  bool operator()(AST::ParenExpr) const;

  private:
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
