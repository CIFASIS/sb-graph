/** @file stm_evaluator.hpp

 @brief <b>Statement evaluator</b>

 Formally, a statement does not generate a result. This evaluator traverses
 statements evaluating the expressions present in each of them, and storing
 the corresponding values of variables.

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

#ifndef SBGRAPH_EVAL_STM_EVALUATOR_HPP_ 
#define SBGRAPH_EVAL_STM_EVALUATOR_HPP_ 

#include "ast/statement.hpp"
#include "eval/eval_context.hpp"
#include "eval/pretty_print.hpp"

namespace SBG {

namespace Eval {

namespace detail {

class StmEvaluator : public boost::static_visitor<StmResult> {
public:
  StmEvaluator(EvalContext& eval_ctx);

  EvalContext& eval_context() const;
  StmResult operator()(AST::Assign assgn) const;
  StmResult operator()(AST::ConfigDims cfg) const; 

private:
  EvalContext& _eval_context;
};

} // namespace detail

} // namespace Eval

} // namespace SBG

#endif // SBGRAPH_EVAL_STM_EVALUATOR_HPP_
