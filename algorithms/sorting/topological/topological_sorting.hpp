/** @file topo_sort.hpp

 @brief <b>SBG Topological Sort Algorithm Abstract Interface</b>

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

#ifndef SBGRAPH_ALGORITHMS_SORTING_TOPOLOGICAL_TOPOLOGICAL_SORTING_HPP_
#define SBGRAPH_ALGORITHMS_SORTING_TOPOLOGICAL_TOPOLOGICAL_SORTING_HPP_

#include "algorithms/sorting/topological/min_vertex_ts.hpp"
#include "sbg/directed_sbg.hpp"

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Topological Sorting Algorithm Implementations -------------------------------
////////////////////////////////////////////////////////////////////////////////

using TSImpl = std::variant<MinVertexTS>;

} // namespace detail

enum class TSKind { kMinVertex };

std::ostream& operator<<(std::ostream& out, const TSKind kind);

////////////////////////////////////////////////////////////////////////////////
// Topological Sorting Algorithm -----------------------------------------------
////////////////////////////////////////////////////////////////////////////////

class TopologicalSorting {
public:
  TopologicalSorting(TSKind kind);

  PWMap calculate(const DirectedSBG& dsbg, const PWMap& pmap);

private:
  TSKind _kind;
  detail::TSImpl _impl;
};

} // namespace LIB

} // namespace SBG

#endif // SBGRAPH_ALGORITHMS_SORTING_TOPOLOGICAL_TOPOLOGICAL_SORTING_HPP_
