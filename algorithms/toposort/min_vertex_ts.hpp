/** @file min_vertex_ts.hpp

 @brief <b>Concrete SBG Minimum Vertex Topological Sort Algorithm
 implementation</b>

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

#ifndef SBG_MIN_VERTEX_TS_HPP
#define SBG_MIN_VERTEX_TS_HPP

#include "algorithms/toposort/topo_sort.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Minimum Vertex Topological Sort Algorithm Implementation (concrete strategy)
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief In each step takes out the minimum vertex without dependencies.
 */
class MinVertexTopoSort : public TSStrategy {
  public:
  MinVertexTopoSort();

  PWMap calculate(const DSBG& dsbg) const override; 
};

} // namespace LIB

} // namespace SBG

#endif
