/*******************************************************************************

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

#include "algorithms/matching/bfs_matching.hpp"
#include "algorithms/matching/matching_fact.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Minimum Reachable SCC Fact ----------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

Matching BFSMatchingFact::createMatchAlgorithm() const
{
  return Matching(std::make_unique<BFSMatching>());
}

////////////////////////////////////////////////////////////////////////////////
// Factory for clients --------------------------------------------------------- 
////////////////////////////////////////////////////////////////////////////////

MatchFactory::MatchFactory()
  : match_fact_(std::make_unique<BFSMatchingFact>()) {}

MatchingFact& MatchFactory::getMatchFactory()
{
  return *match_fact_;
}

void MatchFactory::setMatchFactory(std::unique_ptr<MatchingFact> match_fact)
{
  match_fact_ = std::move(match_fact);
}

} // namespace LIB

} // namespace SBG
