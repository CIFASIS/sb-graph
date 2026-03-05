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

#include "algorithms/matching/match_data.hpp"
#include "util/debug.hpp"

namespace SBG {

namespace LIB {

std::ostream& operator<<(std::ostream& out, const Direction& direction)
{
  switch (direction) {
    case Direction::kForward: {
      out << "forward";
      break;
    }

    case Direction::kBackward: {
      out << "backward";
      break;
    }

    default: {
      Util::ERROR("Unsupported direction");
      break;
    }
  }

  return out;
}

MatchData::MatchData(BipartiteSBG bsbg, Set M, bool full_match)
  : _bsbg(bsbg), _M(M), _full_match(full_match) {}

const BipartiteSBG& MatchData::bsbg() const { return _bsbg; }

const Set& MatchData::M() const { return _M; }

const bool& MatchData::full_match() const { return _full_match; }

std::ostream& operator<<(std::ostream& out, const MatchData& data)
{
  out << data.M();
  if (data.full_match()) {
    out << " [FULLY MATCHED]";
  } else {
    out << " [UNMATCHED]";
  }

  return out;
}

} // namespace LIB

} // namespace SBG
