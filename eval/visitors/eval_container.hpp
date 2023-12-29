/** @file eval_container.hpp

 @brief <b>Container base type evaluator</b>

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

#ifndef AST_VISITOR_EVAL_CONTAINER
#define AST_VISITOR_EVAL_CONTAINER

#include "eval/defs.hpp"

namespace SBG {

namespace Eval {

struct EvalContainer {
  public:
  ContainerBaseType operator()(Util::MD_NAT v) const;
  ContainerBaseType operator()(Util::RATIONAL v) const;
  ContainerBaseType operator()(ContainerBaseType v) const;
  ContainerBaseType operator()(LinearBaseType v) const;
  ContainerBaseType operator()(MapBaseType v) const;
  ContainerBaseType operator()(SBGBaseType v) const;
  ContainerBaseType operator()(InfoBaseType v) const;
};

} // namespace Eval

} // namespace SBG

#endif
