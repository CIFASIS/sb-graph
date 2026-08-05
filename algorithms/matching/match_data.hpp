/** @file match_data.hpp

 @brief <b>Input and Output Matching data structure</b>

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

#ifndef SBGRAPH_ALGORITHMS_MATCHING_MATCH_DATA_HPP_
#define SBGRAPH_ALGORITHMS_MATCHING_MATCH_DATA_HPP_

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
class MatchData {
public:
  MatchData(BipartiteSBG bsbg, Set M, bool full_match);

  const BipartiteSBG& bsbg() const;
  const Set& M() const;
  const bool& full_match() const;

private:
  BipartiteSBG _bsbg; ///< Original input for the algorithm
  Set _M; ///< Matched edges
  bool _full_match; ///< Returns true if all right vertices are saturated
};

std::ostream& operator<<(std::ostream& out, const MatchData& data);

} // namespace LIB

} // namespace SBG

#endif // SBGRAPH_ALGORITHMS_MATCHING_MATCH_DATA_HPP_
