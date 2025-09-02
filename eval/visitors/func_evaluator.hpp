/** @file func_evaluator.hpp

 @brief <b>Function evaluator</b>

 Evaluator for built-in functions. Currently user-defined functions arenot
 allowed.

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

#ifndef FUNC_EVALUATOR 
#define FUNC_EVALUATOR 

#include "eval/defs.hpp"

namespace SBG {

namespace Eval {

////////////////////////////////////////////////////////////////////////////////
// Overload pattern ------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Provides in-place lambdas for visitation for the different
 * operations. These are needed because different structures share the same
 * functions (for example, isEmpty can be applied to intervals, sets, etc.).
 */

template<class... Ts> struct Overload : Ts... { using Ts::operator()...; };
template<class... Ts> Overload(Ts...) -> Overload<Ts...>;

/*
auto& getOpposite();
auto& getCardinal();
auto& getComplement();
const auto& get();
const auto& get();
const auto& get();
const auto& get();
const auto& get();
const auto& get();
const auto& get();
const auto& get();
const auto& get();
const auto& get();
const auto& get();
const auto& get();
const auto& get();
const auto& get();
const auto& get();
const auto& get();
const auto& get();
const auto& get();
const auto& get();
const auto& get();
const auto& get();
const auto& get();
const auto& get();
const auto& get();
const auto& get();
const auto& get();
*/

} // namespace Eval

} // namespace SBG

#endif
