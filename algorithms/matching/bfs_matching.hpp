/** @file bfs_matching.hpp

 @brief <b>Concrete SBG BFS Matching Algorithm implementation</b>

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

#ifndef SBGRAPH_ALGORITHMS_MATCHING_BFS_MATCHING_HPP_
#define SBGRAPH_ALGORITHMS_MATCHING_BFS_MATCHING_HPP_

#include "algorithms/matching/bfs_paths.hpp"
#include "algorithms/matching/match_data.hpp"
#include "sbg/bipartite_sbg.hpp"
#include "sbg/directed_sbg.hpp"
#include "sbg/pw_map.hpp"
#include "sbg/set.hpp"

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// BFS Matching Algorithm Implementation ---------------------------------------
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief BFS Matching implementation for SBG. It starts orientating free edges
 * from map2 (right vertices) to map1 (left vertices) and matched edges
 * (initially this set is empty) from map1 to map2. This way, augmenting paths
 * in the original SBG are paths from right vertices to left vertices in the
 * resulting DSBG.
 * In each iteration augmenting paths are detected, and the direction of edges
 * in such paths are swapped.
 * The algorithms stops once a full match is calculated (i.e. one that saturates
 * all right vertices), or when no more augmenting paths are found.
 */
class BFSMatching {
public:
  BFSMatching();

  MatchData calculate(const BipartiteSBG& bsbg);

private:
  /**
   * @brief Auxiliary struct to represent the exit condition of the algorithm
   * loop.
   */
  class ExitCondition {
  public:
    ExitCondition(bool full_match, bool found_paths_);

    bool full_match();
    bool found_paths();

    bool isSatisfied();
  
  private:
    bool full_match_;
    bool found_paths_; 
  };

  /**
   * @brief Initializes data members determined by the input bipartite SBG.
   */
  void init(const BipartiteSBG& bsbg);

  /**
   * @brief Performs an iteration of the algorithm. It looks up alternating
   * paths that reach unmatched left vertices. Then it swaps the direction of
   * all edges of the DSBG, and performs the same operation. Edges present
   * in paths in both directions belong to augmenting paths, so their direction
   * is swapped, transforming unmatched edges into matched ones and viceversa.
   * @param right_vertices Set of initial right vertices of the original SBG.
   * It is necessary to identify matched edges.
   * @return Returns the status of the two exit conditions: a) All unknowns are
   * saturated and b) New paths weren't found. This is calculated here instead
   * of the main loop to avoid recalculation of certain values. 
   */
  ExitCondition step();

  /**
   * @brief Computes alternating paths in a certain direction.
   * @param right_vertices Set of initial right vertices of the original SBG.
   * It is necessary to identify the endings of alternating paths. 
   * @return Edges belonging to alternating paths that reach unmatched vertices
   * in the aforementioned direction. 
   */
  Set directedStep(const Set& E);

  /**
   * @brief Modifies the `dsbg_` member, swapping mapB and mapD for elements of
   * the domain that belong to `E`.
   */
  void swapEdgesDirection(const Set& E);

  void swapDirection(const Set& E);

  /**
   * @brief Separates in different subset-edges matched and unmatched edges
   * belonging to the same subset-edge in `dsbg_`.
   */
  PWMap partitionSubsetEdges() const;

  DirectedSBG _dsbg; ///< Directed graph according to matching
  Set _M; ///< Matched edges
  Direction _direction;
  Set _X;
  Set _Y;
};

} // namespace detail

} // namespace LIB

} // namespace SBG

#endif // SBGRAPH_ALGORITHMS_MATCHING_BFS_MATCHING_HPP_
