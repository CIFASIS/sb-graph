/** @file operation_visitor.hpp

 @brief <b>SBG operations visitors</b>

 For each operation defined in the SBG module we define a visitor to evaluate
 its result in the eval_expr module. To do so we make use of the Overload
 pattern (don't know how to separate declaration/implementation yet, that's
 why its all included in this file).

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

#ifndef AST_VISITOR_OPERATION
#define AST_VISITOR_OPERATION 

#include <sbg/interval.hpp>

namespace SBG {

namespace Eval {

auto empty_visitor_ = Overload{
  [](SBG::Interval a) { return isEmpty(a); },
  [](SBG::Set a) { return isEmpty(a); },
  [](auto a) { return true; } // << default!
};

auto member_visitor_ = Overload{
  [](Util::INT a, SBG::Interval b) { return isMember(a, b); },
  [](Util::INT a, SBG::Set b) { return isMember(a, b); },
  [](auto a, auto b) { return false; } // << default!
};

auto min_visitor_ = Overload{
  [](SBG::Interval a) { return minElem(a); },
  [](SBG::Set a) { return minElem(a); },
  [](auto a) { return (Util::INT) 0; }
};

auto max_visitor_ = Overload{
  [](SBG::Interval a) { return maxElem(a); },
  [](SBG::Set a) { return maxElem(a); },
  [](auto a) { return (Util::INT) 0; }
};

} // namespace Eval

} // namespace SBG

#endif
