/** @file expression_impl.hpp

 @brief <b>Multi-dimensional Expression implementation</b>

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

#ifndef SBGRAPH_SBG_EXPRESSION_IMPL_HPP_
#define SBGRAPH_SBG_EXPRESSION_IMPL_HPP_

#include "sbg/linear_expr.hpp"

#include <vector>

namespace SBG {

namespace LIB {

namespace detail {

using ExpressionImpl = std::vector<LinearExpr>;

} // namespace detail

} // namespace LIB

} // namespace SBG

#endif // SBGRAPH_SBG_EXPRESSION_IMPL_HPP_
