/** @file bfs_paths.hpp

 @brief <b>Concrete SBG Path Discoverer Algorithm</b>

 @see matching.hpp and matching.cpp files to comprehend its purpose.

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

#ifndef SBGRAPH_ALGORITHMS_MATCHING_BFS_PATHS_HPP_
#define SBGRAPH_ALGORITHMS_MATCHING_BFS_PATHS_HPP_

#include "algorithms/matching/paths.hpp"
#include "sbg/directed_sbg.hpp"
#include "sbg/pw_map.hpp"
#include "sbg/set.hpp"

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Paths Discoverer Algorithm --------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Backward BFS implementation to calculate paths.
 */
class BFSPaths : public PathsContext<BFSPaths> {
public:
  BFSPaths();

  /**
   * @brief Concrete implementation that starts with the identity pw for
   * vertices belonging to `endings`. In each step adds adjacent vertices to
   * the map. It also detects recursions (i.e. if a Set-Vertex is visited more)
   * than once, replicating the same path for every element of the same
   * Set-Vertex.
   */
  PWMap calculate(const DirectedSBG& dsbg, const Set& endings);
};

} // namespace detail

} // namespace LIB

} // namespace SBG

#endif // SBGRAPH_ALGORITHMS_MATCHING_BFS_PATHS_HPP_
