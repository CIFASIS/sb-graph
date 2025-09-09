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

#include <chrono>

#include "algorithms/matching/matching.hpp"
#include "util/logger.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Auxiliary structures --------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& out, const Direction& direction)
{
  switch (direction) {
    case Direction::kForward:
      out << "forward";
      break;
    case Direction::kBackward:
      out << "backward";
      break;
  }

  return out;
}

MatchData::MatchData(SBG sbg, Set M, bool full_match)
  : sbg_(sbg), M_(M), full_match_(full_match) {}

const SBG& MatchData::sbg() const { return sbg_; }
const Set& MatchData::M() const { return M_; }
const bool& MatchData::full_match() const { return full_match_; }

std::ostream& operator<<(std::ostream& out, const MatchData& data)
{
  out << data.M();
  if (data.full_match())
    out << " [FULLY MATCHED]";
  else
    out << " [UNMATCHED]";

  return out;
}

////////////////////////////////////////////////////////////////////////////////
// Matching Algorithm Abstract Strategy Constructors ---------------------------
////////////////////////////////////////////////////////////////////////////////

MatchStrategy::MatchStrategy(const PWMapAF& fact) : fact_(std::move(fact)) {}

////////////////////////////////////////////////////////////////////////////////
// Matching Algorithm Interface ------------------------------------------------ 
////////////////////////////////////////////////////////////////////////////////

Matching::Matching(MatchStratPtr strat) : strategy_(std::move(strat)) {}

MatchData Matching::calculate(const SBG& sbg)
{
  return strategy_->calculate(sbg);
}

} // namespace LIB

} // namespace SBG
