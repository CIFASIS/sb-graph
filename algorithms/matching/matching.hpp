/** @file matching.hpp

 @brief <b>SBG Matching Algorithm implementation</b>

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

#ifndef SBG_MATCH_HPP
#define SBG_MATCH_HPP

#include "sbg/sbg.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Auxiliary structures --------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

enum class Direction { kForward, kBackward };
std::ostream& operator<<(std::ostream& out, const Direction& direction);

/**
 * @brief Saves input and output data from a matching algorithm run.
 */
struct MatchData {
  public:
  MatchData(SBG sbg, Set M, bool full_match);

  const SBG& sbg() const;
  const Set& M() const;
  const bool& full_match() const;

  private:
  SBG sbg_;         ///< Original input for the algorithm
  Set M_;           ///< Matched edges
  bool full_match_; ///< Returns true if all right vertices are saturated
};
std::ostream& operator<<(std::ostream& out, const MatchData& data);

////////////////////////////////////////////////////////////////////////////////
// Matching Algorithm Abstract Delegate ----------------------------------------
////////////////////////////////////////////////////////////////////////////////

class MatchDelegate;

typedef std::unique_ptr<MatchDelegate> MatchDelegPtr;

class MatchDelegate {
  public:
  virtual ~MatchDelegate() = default;

  MatchDelegate(const PWMapAF& fact);

  virtual MatchData calculate(const SBG& sbg) = 0;

  protected:
  const PWMapAF& fact_;
};


////////////////////////////////////////////////////////////////////////////////
// BFS Matching Algorithm Implementation (concrete delegate) -------------------
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief BFS Matching implementation for SBG. It starts orientating free edges
 * from map2 (right vertices) to map1 (left vertices) and matched edges
 * (initially this set is empty) from map1 to map2. This way, augmenting paths
 * in the original SBG are paths from right vertices to left vertices in the
 * resulting DSBG.
 * In each iteration augmenting paths are detected, and the direction of edges
 * in such paths are swapped.
 * The algorithms stops once a full match is calculate (i.e. one that saturates
 * all right vertices), or when no more augmenting paths are found.
 */
struct BFSMatching : public MatchDelegate {
  public:
  BFSMatching(const PWMapAF& fact);

  MatchData calculate(const SBG& sbg) override;

  private:
  /**
   * @brief Auxiliary struct to represent the exit condition of the algorithm
   * loop.
   */
  struct ExitCondition {
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
   * @return Returns the status of the two exit conditions: a) All unknowns are
   * saturated and b) New paths weren't found. This is calculated here instead
   * of the main loop to avoid recalculation of certain values. 
   */
  ExitCondition step();

  /**
   * @brief Computes alternating paths in a certain direction.
   * @return Edges belonging to alternating paths that reach unmatched vertices
   * in the aforementioned direction. 
   */
  Set directedStep(const Set& E);

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
  Set right_vertices_; 
  Direction direction_;
};

////////////////////////////////////////////////////////////////////////////////
// Matching Algorithm Implementation (delegator) -------------------------------
////////////////////////////////////////////////////////////////////////////////

class Matching {
  public:
  Matching(MatchDelegPtr deleg);

  MatchData calculate(const SBG& sbg);

  private:
  MatchDelegPtr delegate_;
};

} // namespace LIB

} // namespace SBG

#endif
