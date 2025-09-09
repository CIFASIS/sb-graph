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

#ifndef SET_IMPL_VISITOR
#define SET_IMPL_VISITOR

#include <boost/variant.hpp>

#include "ast/sbg_program.hpp"
#include "eval/impl_context.hpp"

namespace SBG {

namespace Eval {

class SetImplVisitor : public boost::static_visitor<bool> {
  public:
  SetImplVisitor(const VarEnv& venv);

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
  const VarEnv& venv_;
};

} // namespace Eval

} // namespace SBG

#endif
