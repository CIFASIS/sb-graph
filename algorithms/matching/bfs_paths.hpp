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

#ifndef SBG_BFS_PATH_HPP
#define SBG_BFS_PATH_HPP

#include "algorithms/matching/paths.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Paths Discoverer Algorithm --------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Backward BFS implementation to calculate paths.
 */
class BFSPaths : public PathsContext<BFSPaths> {
  public:
  /**
   * @brief Concrete implementation that starts with the identity pw for
   * vertices belonging to `endings`. In each step adds adjacent vertices to
   * the map. It also detects recursions (i.e. if a Set-Vertex is visited more)
   * than once, replicating the same path for every element of the same
   * Set-Vertex.
   */
  PWMap calculate(const DSBG& dsbg, const Set& endings);
};

} // namespace LIB

} // namespace SBG

#endif
