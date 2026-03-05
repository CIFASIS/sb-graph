/** @file var_env.hpp

 @brief <b>Variables environment</b>
 
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

#ifndef SBGRAPH_EVAL_VAR_ENV_HPP_
#define SBGRAPH_EVAL_VAR_ENV_HPP_

#include "ast/expr.hpp"
#include "eval/base_type.hpp"

#include <unordered_map>

namespace SBG {

namespace Eval {

namespace detail {

/** 
 * @brief Variable environment (with expressions already evaluated). This env
 * will be populated by StmEvaluator, and used by ExprEvaluator.
 */
class VarEnv {
public:
  using VKey = AST::Name;
  using VValue = ExprBaseType;
  using VType = std::unordered_map<VKey, VValue>;
  using VIt = VType::const_iterator;

  VarEnv();

  VIt begin() const;
  VIt end() const;
  VIt find(const VKey& key) const;
  void insert(VKey key, VValue value);

private:
  mutable VType _variables;
};

} // namespace detail

} // namespace Eval

} // namespace SBG

#endif // SBGRAPH_EVAL_VAR_ENV_HPP_
