/** @file eval_base_map.hpp

 @brief <b>Linear base map expression evaluator</b>

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

#ifndef AST_VISITOR_EVAL_BASE_MAP
#define AST_VISITOR_EVAL_BASE_MAP

#include "eval/visitors/eval_mdle.hpp"
#include "eval/visitors/eval_unord_set.hpp"
#include "sbg/map.hpp"

namespace SBG {

namespace Eval {

struct EvalBaseMap : public boost::static_visitor<LIB::BaseMap> {
  public:
  EvalBaseMap();
  EvalBaseMap(VarEnv env);

  LIB::BaseMap operator()(AST::Natural v) const;
  LIB::BaseMap operator()(AST::MDNatural v) const;
  LIB::BaseMap operator()(AST::Rational v) const;
  LIB::BaseMap operator()(AST::Boolean v) const;
  LIB::BaseMap operator()(Util::VariableName v) const;
  LIB::BaseMap operator()(AST::UnaryOp v) const;
  LIB::BaseMap operator()(AST::BinOp v) const;
  LIB::BaseMap operator()(AST::Call v) const;
  LIB::BaseMap operator()(AST::Interval v) const;
  LIB::BaseMap operator()(AST::InterUnaryOp v) const;
  LIB::BaseMap operator()(AST::InterBinOp v) const;
  LIB::BaseMap operator()(AST::MultiDimInter v) const;
  LIB::BaseMap operator()(AST::MDInterUnaryOp v) const;
  LIB::BaseMap operator()(AST::MDInterBinOp v) const;
  LIB::BaseMap operator()(AST::Set v) const;
  LIB::BaseMap operator()(AST::SetUnaryOp v) const;
  LIB::BaseMap operator()(AST::SetBinOp v) const;
  LIB::BaseMap operator()(AST::LinearExp v) const;
  LIB::BaseMap operator()(AST::LExpBinOp v) const;
  LIB::BaseMap operator()(AST::MDLExp v) const;
  LIB::BaseMap operator()(AST::MDLExpBinOp v) const;
  LIB::BaseMap operator()(AST::LinearMap v) const;
  LIB::BaseMap operator()(AST::PWLMap v) const;
  LIB::BaseMap operator()(AST::SBG v) const;

  private:
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
