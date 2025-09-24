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

#ifndef SBG_BFS_MATCH_HPP
#define SBG_BFS_MATCH_HPP

#include "algorithms/matching/bfs_paths.cpp"
#include "algorithms/matching/matching.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// BFS Matching Algorithm Implementation (concrete strategy) -------------------
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
class BFSMatching : public MatchStrategy {
  public:
  BFSMatching();

  MatchData calculate(const SBG& sbg) override;

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
   * @brief Initializes data members determined by the input SBG.
   */
  void init(const SBG& sbg);

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
  ExitCondition step(const Set& right_vertices);

  /**
   * @brief Computes alternating paths in a certain direction.
   * @param right_vertices Set of initial right vertices of the original SBG.
   * It is necessary to identify the endings of alternating paths. 
   * @return Edges belonging to alternating paths that reach unmatched vertices
   * in the aforementioned direction. 
   */
  Set directedStep(const Set& E, const Set& right_vertices);

  /**
   * @brief Modifies the `dsbg_` member, swapping mapB and mapD for elements of
   * the domain that belong to `E`.
   */
  void swapEdgesDirection(const Set& E);

  /**
   * @brief Separates in different subset-edges matched and unmatched edges
   * belonging to the same subset-edge in `dsbg_`.
   */
  PWMap partitionSubsetEdges() const;

  /**
   * @brief Returns edges in `E` that belong to the paths described by the
   * successor map `smap`. 
   */
  Set edgesInPaths(const PWMap& smap, const Set& E) const;

  DSBG dsbg_; ///< Input DSBG
  Set M_;     ///< Matched edges
  Direction direction_;
};

} // namespace LIB

} // namespace SBG

#endif
