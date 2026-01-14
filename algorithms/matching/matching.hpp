/** @file matching.hpp

 @brief <b>SBG Matching Algorithm Abstract Interface</b>

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

#include "sbg/bipartite_sbg.hpp"

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
  MatchData(BipartiteSBG bsbg, Set M, bool full_match);

  const BipartiteSBG& bsbg() const;
  const Set& M() const;
  const bool& full_match() const;

  private:
  BipartiteSBG bsbg_; ///< Original input for the algorithm
  Set M_; ///< Matched edges
  bool full_match_; ///< Returns true if all right vertices are saturated
};
std::ostream& operator<<(std::ostream& out, const MatchData& data);

////////////////////////////////////////////////////////////////////////////////
// Matching Algorithm Abstract Strategy ----------------------------------------
////////////////////////////////////////////////////////////////////////////////

class MatchStrategy;

typedef std::unique_ptr<MatchStrategy> MatchStratPtr;

class MatchStrategy {
  public:
  virtual ~MatchStrategy() = default;

  MatchStrategy();

  virtual MatchData calculate(const BipartiteSBG& bsbg) = 0;
};


////////////////////////////////////////////////////////////////////////////////
// Matching Algorithm Interface (context) --------------------------------------
////////////////////////////////////////////////////////////////////////////////

class Matching {
  public:
  Matching(MatchStratPtr strat);

  MatchData calculate(const BipartiteSBG& bsbg);

  private:
  MatchStratPtr strategy_;
};

} // namespace LIB

} // namespace SBG

#endif