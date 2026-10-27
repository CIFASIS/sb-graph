/** @file eval_context.hpp

 @brief <b>Evaluation context</b>

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

#ifndef SBGRAPH_EVAL_EVAL_CONTEXT_HPP_
#define SBGRAPH_EVAL_EVAL_CONTEXT_HPP_

#include "eval/func_env.hpp"
#include "eval/var_env.hpp"

namespace SBG {

namespace Eval {

namespace detail {

/** 
 * @brief Evaluation context that keeps track of the arity of the evaluated
 * program, defined variables, and built-in functions.
 */
class EvalContext {
public:
  EvalContext();

  // Getters
  unsigned int arity() const;
  VarEnv& venv();
  FuncEnv& fenv();
  // Setters
  void setArity(unsigned int arity); 
  void insertVariable(VarEnv::VKey key, VarEnv::VValue value); 
  void insertFunction(FuncEnv::FKey key, FuncEnv::FValue value);

private:
  unsigned int arity_; ///< Number of dimensions of the program
  VarEnv venv_;
  FuncEnv fenv_;
};

} // namespace detail

} // namespace Eval

} // namespace SBG

#endif // SBGRAPH_EVAL_EVAL_CONTEXT_HPP_
