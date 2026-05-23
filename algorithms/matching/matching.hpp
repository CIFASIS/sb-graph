/** @file matching.hpp

 @brief <b>SBG Matching Algorithm</b>

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

#ifndef SBGRAPH_ALGORITHMS_MATCHING_MATCHING_HPP_
#define SBGRAPH_ALGORITHMS_MATCHING_MATCHING_HPP_

#include "algorithms/matching/bfs_matching.hpp"
#include "algorithms/matching/match_data.hpp"
#include "sbg/bipartite_sbg.hpp"

#include <variant>

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Matching Algorithm Implementations ------------------------------------------
////////////////////////////////////////////////////////////////////////////////

using MatchImpl = std::variant<BFSMatching>;

} // namespace detail

////////////////////////////////////////////////////////////////////////////////
// Matching Algorithm ----------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

class Matching {
public:
  Matching();

  MatchData calculate(const BipartiteSBG& bsbg);

private:
  detail::MatchImpl _impl;
};

} // namespace LIB

} // namespace SBG

#endif // SBGRAPH_ALGORITHMS_MATCHING_MATCHING_HPP_
