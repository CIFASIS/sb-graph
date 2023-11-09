/** @file eval_graph.hpp

 @brief <b>Graph expression evaluator</b>

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

#ifndef AST_VISITOR_EVAL_GRAPH
#define AST_VISITOR_EVAL_GRAPH

#include "eval/visitors/eval_pwmap.hpp"

namespace SBG {

namespace Eval {

struct EvalGraphType {
  member_class(LIB::SBGraph, sbg);
  member_class(LIB::DSBGraph, dsbg);

  EvalGraphType();
  EvalGraphType(LIB::SBGraph sbg);
  EvalGraphType(LIB::DSBGraph dsbg);
};

struct EvalGraph : public boost::static_visitor<EvalGraphType> {
  public:
  EvalGraph();
  EvalGraph(unsigned int nmbr_dims, VarEnv env);

  EvalGraphType operator()(AST::Natural v) const;
  EvalGraphType operator()(AST::MDNatural v) const;
  EvalGraphType operator()(AST::Rational v) const;
  EvalGraphType operator()(AST::Boolean v) const;
  EvalGraphType operator()(Util::VariableName v) const;
  EvalGraphType operator()(AST::UnaryOp v) const;
  EvalGraphType operator()(AST::BinOp v) const;
  EvalGraphType operator()(AST::Call v) const;
  EvalGraphType operator()(AST::Interval v) const;
  EvalGraphType operator()(AST::InterUnaryOp v) const;
  EvalGraphType operator()(AST::InterBinOp v) const;
  EvalGraphType operator()(AST::MultiDimInter v) const;
  EvalGraphType operator()(AST::MDInterUnaryOp v) const;
  EvalGraphType operator()(AST::MDInterBinOp v) const;
  EvalGraphType operator()(AST::Set v) const;
  EvalGraphType operator()(AST::SetUnaryOp v) const;
  EvalGraphType operator()(AST::SetBinOp v) const;
  EvalGraphType operator()(AST::LinearExp v) const;
  EvalGraphType operator()(AST::LExpBinOp v) const;
  EvalGraphType operator()(AST::MDLExp v) const;
  EvalGraphType operator()(AST::MDLExpBinOp v) const;
  EvalGraphType operator()(AST::LinearMap v) const;
  EvalGraphType operator()(AST::PWLMap v) const;
  EvalGraphType operator()(AST::SBG v) const;
  EvalGraphType operator()(AST::DSBG v) const;

  private:
  unsigned int nmbr_dims_;
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
