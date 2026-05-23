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

#include "algorithms/matching/matching_impl.hpp"
#include "util/debug.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Matching implementations ----------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& out, const MatchKind kind)
{
  switch (kind) {
    case MatchKind::kBFSPaths: {
      out << "BFS paths";
      break;
    }

    default: {
      Util::ERROR("Unsupported matching algorithm implementation");
      break;
    }
  }

  return out;
}

MatchImplementation::MatchImplementation() : _kind(MatchKind::kBFSPaths) {}

MatchImplementation& MatchImplementation::instance()
{
  static MatchImplementation _instance;
  return _instance;
}

const MatchKind& MatchImplementation::kind() const { return _kind; }

void MatchImplementation::set_match_fact(MatchKind kind) { _kind = kind; }

} // namespace LIB

} // namespace SBG
