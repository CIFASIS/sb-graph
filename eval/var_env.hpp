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

#ifndef EVAL_VAR_ENV_HPP
#define EVAL_VAR_ENV_HPP

#include <unordered_map>

#include "ast/expr.hpp"
#include "eval/base_type.hpp"

namespace SBG {

namespace Eval {

/** 
 * @brief Variable environment (with expressions already evaluated). This env
 * will be populated by StmEvaluator, and used by ExprEvaluator.
 */
struct VarEnv {
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
  mutable VType variables_;
};

} // namespace Eval

} // namespace SBG

#endif
