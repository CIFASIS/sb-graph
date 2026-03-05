/** @file cc.hpp

 @brief <b>Connected Components SBG implementation</b>

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

#ifndef SBGRAPH_ALGORITHMS_CC_CC_HPP_
#define SBGRAPH_ALGORITHMS_CC_CC_HPP_

#include "sbg/pw_map.hpp"
#include "sbg/sbg.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Connected components --------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

PWMap connectedComponents(const SBG& g);

} // namespace LIB

} // namespace SBG

#endif // SBGRAPH_ALGORITHMS_CC_CC_HPP_
