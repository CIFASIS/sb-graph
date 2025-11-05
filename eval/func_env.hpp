/** @file func_env.hpp

 @brief <b>Function environment</b>
 
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

#ifndef EVAL_FUNC_ENV_HPP
#define EVAL_FUNC_ENV_HPP

#include <unordered_map>

#include "ast/expr.hpp"
#include "eval/base_type.hpp"

namespace SBG {

namespace Eval {

/**
 * @brief Function environment. Only has built-in functions: SBG programs can't
 * define new functions.
 */
class FuncEnv {
  public:
  using FKey = AST::Name;
  using FValue = std::function<ExprBaseType(const std::vector<ExprBaseType>&)>;
  using FType = std::unordered_map<FKey, FValue>;
  using FIt = FType::const_iterator;

  FuncEnv();

  FIt begin() const;
  FIt end() const;
  FIt find(const FKey& key) const;
  void insert(FKey key, FValue value);

  private:
  FType functions_;
};

} // namespace Eval

} // namespace SBG

#endif
