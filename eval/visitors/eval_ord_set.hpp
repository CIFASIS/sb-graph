/** @file eval_ord_set.hpp

 @brief <b>Ordered set expression evaluator</b>

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

#ifndef AST_VISITOR_EVAL_ORD_SET
#define AST_VISITOR_EVAL_ORD_SET

#include "eval/visitors/eval_mdi.hpp"

namespace SBG {

namespace Eval {

struct EvalOrdSet : public boost::static_visitor<LIB::OrdSet> {
  public:
  EvalOrdSet();
  EvalOrdSet(VarEnv env);

  LIB::OrdSet operator()(AST::Natural v) const;
  LIB::OrdSet operator()(AST::MDNatural v) const;
  LIB::OrdSet operator()(AST::Rational v) const;
  LIB::OrdSet operator()(AST::Boolean v) const;
  LIB::OrdSet operator()(Util::VariableName v) const;
  LIB::OrdSet operator()(AST::UnaryOp v) const;
  LIB::OrdSet operator()(AST::BinOp v) const;
  LIB::OrdSet operator()(AST::Call v) const;
  LIB::OrdSet operator()(AST::Interval v) const;
  LIB::OrdSet operator()(AST::InterUnaryOp v) const;
  LIB::OrdSet operator()(AST::InterBinOp v) const;
  LIB::OrdSet operator()(AST::MultiDimInter v) const;
  LIB::OrdSet operator()(AST::MDInterUnaryOp v) const;
  LIB::OrdSet operator()(AST::MDInterBinOp v) const;
  LIB::OrdSet operator()(AST::Set v) const;
  LIB::OrdSet operator()(AST::SetUnaryOp v) const;
  LIB::OrdSet operator()(AST::SetBinOp v) const;
  LIB::OrdSet operator()(AST::LinearExp v) const;
  LIB::OrdSet operator()(AST::LExpBinOp v) const;
  LIB::OrdSet operator()(AST::MDLExp v) const;
  LIB::OrdSet operator()(AST::MDLExpBinOp v) const;
  LIB::OrdSet operator()(AST::LinearMap v) const;
  LIB::OrdSet operator()(AST::PWLMap v) const;
  LIB::OrdSet operator()(AST::SBG v) const;
  LIB::OrdSet operator()(AST::DSBG v) const;

  private:
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
