/** @file eval_canon_map.hpp

 @brief <b>Linear canon map expression evaluator</b>

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

#ifndef AST_VISITOR_EVAL_CANON_MAP
#define AST_VISITOR_EVAL_CANON_MAP

#include "eval/visitors/eval_mdle.hpp"
#include "eval/visitors/eval_ord_set.hpp"
#include "sbg/map.hpp"

namespace SBG {

namespace Eval {

struct EvalCanonMap : public boost::static_visitor<LIB::CanonMap> {
  public:
  EvalCanonMap();
  EvalCanonMap(VarEnv env);

  LIB::CanonMap operator()(AST::Natural v) const;
  LIB::CanonMap operator()(AST::MDNatural v) const;
  LIB::CanonMap operator()(AST::Rational v) const;
  LIB::CanonMap operator()(AST::Boolean v) const;
  LIB::CanonMap operator()(Util::VariableName v) const;
  LIB::CanonMap operator()(AST::UnaryOp v) const;
  LIB::CanonMap operator()(AST::BinOp v) const;
  LIB::CanonMap operator()(AST::Call v) const;
  LIB::CanonMap operator()(AST::Interval v) const;
  LIB::CanonMap operator()(AST::InterUnaryOp v) const;
  LIB::CanonMap operator()(AST::InterBinOp v) const;
  LIB::CanonMap operator()(AST::MultiDimInter v) const;
  LIB::CanonMap operator()(AST::MDInterUnaryOp v) const;
  LIB::CanonMap operator()(AST::MDInterBinOp v) const;
  LIB::CanonMap operator()(AST::Set v) const;
  LIB::CanonMap operator()(AST::SetUnaryOp v) const;
  LIB::CanonMap operator()(AST::SetBinOp v) const;
  LIB::CanonMap operator()(AST::LinearExp v) const;
  LIB::CanonMap operator()(AST::LExpBinOp v) const;
  LIB::CanonMap operator()(AST::MDLExp v) const;
  LIB::CanonMap operator()(AST::MDLExpBinOp v) const;
  LIB::CanonMap operator()(AST::LinearMap v) const;
  LIB::CanonMap operator()(AST::PWLMap v) const;
  LIB::CanonMap operator()(AST::SBG v) const;
  LIB::CanonMap operator()(AST::DSBG v) const;

  private:
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
