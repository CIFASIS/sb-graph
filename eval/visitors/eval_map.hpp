/** @file eval_map.hpp

 @brief <b>Map expression evaluator</b>

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

#ifndef AST_VISITOR_EVAL_MAP
#define AST_VISITOR_EVAL_MAP

#include "eval/visitors/eval_mdle.hpp"
#include "eval/visitors/eval_set.hpp"

namespace SBG {

namespace Eval {

struct EvalMap : public boost::static_visitor<LIB::Map> {
  public:
  EvalMap(unsigned int nmbr_dims, const LIB::PWMapAF &fact, VarEnv &env);

  LIB::Map operator()(AST::Natural v) const;
  LIB::Map operator()(AST::Rational v) const;
  LIB::Map operator()(AST::VariableName v) const;
  LIB::Map operator()(AST::UnaryOp v) const;
  LIB::Map operator()(AST::BinOp v) const;
  LIB::Map operator()(AST::Call v) const;
  LIB::Map operator()(AST::Interval v) const;
  LIB::Map operator()(AST::InterUnaryOp v) const;
  LIB::Map operator()(AST::InterBinOp v) const;
  LIB::Map operator()(AST::MultiDimInter v) const;
  LIB::Map operator()(AST::MDInterUnaryOp v) const;
  LIB::Map operator()(AST::MDInterBinOp v) const;
  LIB::Map operator()(AST::Set v) const;
  LIB::Map operator()(AST::SetUnaryOp v) const;
  LIB::Map operator()(AST::SetBinOp v) const;
  LIB::Map operator()(AST::LinearExp v) const;
  LIB::Map operator()(AST::LExpBinOp v) const;
  LIB::Map operator()(AST::MDLExp v) const;
  LIB::Map operator()(AST::MDLExpBinOp v) const;
  LIB::Map operator()(AST::LinearMap v) const;
  LIB::Map operator()(AST::PWLMap v) const;
  LIB::Map operator()(AST::SBG v) const;
  LIB::Map operator()(AST::DSBG v) const;

  private:
  unsigned int nmbr_dims_;
  const LIB::PWMapAF &fact_;
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
