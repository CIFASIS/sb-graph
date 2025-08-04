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

/**
 * @brief Saves input and output data from a matching algorithm run.
 */
struct MatchData {
  public:
  MatchData(SBG sbg, Set M, bool full_match);

  private:
  member_class(SBG, sbg);
  member_class(Set, M);
  member_class(bool, full_match);
};
std::ostream &operator<<(std::ostream &out, const MatchData &data);

////////////////////////////////////////////////////////////////////////////////
// Matching Algorithm Abstract Delegate ----------------------------------------
////////////////////////////////////////////////////////////////////////////////

class MatchDelegate;

typedef std::unique_ptr<MatchDelegate> MatchDelegPtr;

class MatchDelegate {
  public:
  virtual ~MatchDelegate() = default;

  MatchDelegate(const PWMapAF &fact);

  virtual MatchData calculate(const SBG &sbg) = 0;

  protected:
  const PWMapAF &fact_;
};


////////////////////////////////////////////////////////////////////////////////
// BFS Matching Algorithm Implementation (concrete delegate) -------------------
////////////////////////////////////////////////////////////////////////////////

struct BFSMatching : public MatchDelegate {
  public:
  BFSMatching(const PWMapAF &fact);

  MatchData calculate(const SBG &sbg) override;

  private:
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
   * @brief Performs an iteration of the algorithm. It looks up augmenting paths
   * using some implementation of Paths. Then it swaps the direction of edges
   * belonging to such paths. Finally, it swaps the direction of all edges of
   * the DSBG. 
   * @return Returns the status of the two exit conditions: a) All unknowns are
   * saturated and b) New paths weren't found. This is calculated here instead
   * of the main loop to avoid recalculation of certain values. 
   */
  ExitCondition step();

  PWMap directedStep(const Set &E);

  /**
   * @brief Modifies the `dsbg_` member, swapping mapB and mapD for elements of
   * the domain that belong to `E`.
   */
  void swapEdgesDirection(const Set &E);

  PWMap partitionSubsetEdges() const;

  Set edgesInPaths(const PWMap &smap, const Set &E) const;

  DSBG dsbg_;
  Set M_;
  Set U_;
  bool forward_;
};

////////////////////////////////////////////////////////////////////////////////
// Matching Algorithm Implementation (delegator) -------------------------------
////////////////////////////////////////////////////////////////////////////////

class Matching {
  public:
  Matching(MatchDelegPtr deleg);

  MatchData calculate(const SBG &sbg);

  private:
  MatchDelegPtr delegate_;
};

} // namespace LIB

} // namespace SBG

#endif
