/** @file eval_linear.hpp

 @brief <b>Linear base type evaluator</b>

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

#ifndef AST_VISITOR_EVAL_LINEAR
#define AST_VISITOR_EVAL_LINEAR

#include "eval/defs.hpp"

namespace SBG {

namespace Eval {

struct EvalLinear {
  public:
  LinearBaseType operator()(Util::MD_NAT v) const;
  LinearBaseType operator()(Util::RATIONAL v) const;
  LinearBaseType operator()(ContainerBaseType v) const;
  LinearBaseType operator()(LinearBaseType v) const;
  LinearBaseType operator()(MapBaseType v) const;
  LinearBaseType operator()(SBGBaseType v) const;
  LinearBaseType operator()(InfoBaseType v) const;
};

} // namespace Eval

} // namespace SBG

#endif
