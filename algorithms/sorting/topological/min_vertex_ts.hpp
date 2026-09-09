/** @file min_vertex_ts.hpp

 @brief <b>Concrete SBG Minimum Vertex Topological Sorting Algorithm
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

#ifndef SBGRAPH_ALGORITHMS_SORTING_TOPOLOGICAL_MIN_VERTEX_TS_HPP_
#define SBGRAPH_ALGORITHMS_SORTING_TOPOLOGICAL_MIN_VERTEX_TS_HPP_

#include "sbg/directed_sbg.hpp"
#include "sbg/expression.hpp"
#include "sbg/integer.hpp"
#include "sbg/pw_map.hpp"
#include "sbg/set.hpp"

#include <tuple>

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Minimum Vertex Topological Sorting Algorithm Implementation -----------------
////////////////////////////////////////////////////////////////////////////////

class MinVertexTS {
public:
  MinVertexTS();

  /**
   * @brief Concrete implementation that in each step takes out the minimum
   * vertex without dependencies. It also implements different strategies to
   * sort a large number of vertices.
   */
  PWMap calculate(const DirectedSBG& dsbg, const PWMap& pmap);

private:
  IntTuple getVertex(Set old_Vj);

  /**
   * @brief Identifies the "path" that leads to the repetition in _smap and
   * extends it for vertices in the same set-vertex as those that participate in
   * the recursive "path".
   */
  std::tuple<Set, PWMap> detectRepetition(Set Vj, const DirectedSBG& dsbg);

  /**
   * @brief Handles a repetition.
   */
  PWMap repetition(const Set& init_V, const DirectedSBG& dsbg);

  DirectedSBG _dsbg;
  PWMap _smap;
  Set _priority;
  Set _independent;
  std::vector<Set> _visitedSV;
  unsigned int _max_repetition_depth;
};

} // namespace detail

} // namespace LIB

} // namespace SBG

#endif // SBGRAPH_ALGORITHMS_SORTING_TOPOLOGICAL_MIN_VERTEX_TS_HPP_
