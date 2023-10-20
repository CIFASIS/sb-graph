/** @file eval_unord_set.hpp

 @brief <b>Unordered set expression evaluator</b>

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

#ifndef AST_VISITOR_EVAL_UNORD_SET
#define AST_VISITOR_EVAL_UNORD_SET

#include "eval/visitors/eval_mdi.hpp"

namespace SBG {

namespace Eval {

struct EvalUnordSet : public boost::static_visitor<LIB::UnordSet> {
  public:
  EvalUnordSet();
  EvalUnordSet(VarEnv env);

  LIB::UnordSet operator()(AST::Natural v) const;
  LIB::UnordSet operator()(AST::MDNatural v) const;
  LIB::UnordSet operator()(AST::Rational v) const;
  LIB::UnordSet operator()(AST::Boolean v) const;
  LIB::UnordSet operator()(Util::VariableName v) const;
  LIB::UnordSet operator()(AST::UnaryOp v) const;
  LIB::UnordSet operator()(AST::BinOp v) const;
  LIB::UnordSet operator()(AST::Call v) const;
  LIB::UnordSet operator()(AST::Interval v) const;
  LIB::UnordSet operator()(AST::InterUnaryOp v) const;
  LIB::UnordSet operator()(AST::InterBinOp v) const;
  LIB::UnordSet operator()(AST::MultiDimInter v) const;
  LIB::UnordSet operator()(AST::MDInterUnaryOp v) const;
  LIB::UnordSet operator()(AST::MDInterBinOp v) const;
  LIB::UnordSet operator()(AST::Set v) const;
  LIB::UnordSet operator()(AST::SetUnaryOp v) const;
  LIB::UnordSet operator()(AST::SetBinOp v) const;
  LIB::UnordSet operator()(AST::LinearExp v) const;
  LIB::UnordSet operator()(AST::LExpBinOp v) const;
  LIB::UnordSet operator()(AST::MDLExp v) const;
  LIB::UnordSet operator()(AST::MDLExpBinOp v) const;
  LIB::UnordSet operator()(AST::LinearMap v) const;
  LIB::UnordSet operator()(AST::PWLMap v) const;
  LIB::UnordSet operator()(AST::SBG v) const;
  LIB::UnordSet operator()(AST::DSBG v) const;

  private:
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
