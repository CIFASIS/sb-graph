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

#include <sbgraph/detail/algorithms/sorting/topological/ts_utils.hpp>

#include <sbgraph/sbg/directed_sbg.hpp>
#include <sbgraph/sbg/expression.hpp>
#include <sbgraph/sbg/integer.hpp>
#include <sbgraph/sbg/pw_map.hpp>
#include <sbgraph/sbg/set.hpp>

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
  PWMap extendRepetition();

  /**
   * @brief Handles a repetition.
   */
  PWMap repetition(const PWMap& Emap);

  DirectedSBG _dsbg;
  PWMap _smap;
  Set _priority;
  SortedSets _repetitionSV; ///< Visited set-vertices up to repetition.
  SortedSets _repetitionSE; ///< Set-edges deleted up to repetition.
  std::vector<Map> _repetition_sort; ///< Sorting up to repetition.
  int _depth; ///< Repetition depth.
};

} // namespace detail

} // namespace LIB

} // namespace SBG

#endif // SBGRAPH_ALGORITHMS_SORTING_TOPOLOGICAL_MIN_VERTEX_TS_HPP_
