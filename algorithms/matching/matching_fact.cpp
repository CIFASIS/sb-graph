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

#include "algorithms/matching/matching_fact.hpp"
#include "util/debug.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// BFS Paths Matching Factory --------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

Matching BFSMatchingFact::createMatchAlgorithm() const
{
  return Matching(MatchKind::kBFSPaths);
}

////////////////////////////////////////////////////////////////////////////////
// Factory for clients --------------------------------------------------------- 
////////////////////////////////////////////////////////////////////////////////

MatchFactory::MatchFactory() : _kind(MatchKind::kBFSPaths), _impl() {}

MatchFactory& MatchFactory::instance()
{
  static MatchFactory _instance;
  return _instance;
}

const MatchKind& MatchFactory::kind() const { return _kind; }

void MatchFactory::set_match_fact(MatchKind kind)
{
  _kind = kind;
  switch (kind) {
    case MatchKind::kBFSPaths: {
      _impl = BFSMatchingFact{};
      break;
    }

    default: {
      Util::ERROR("Unsupported matching implementation");
    }
  }
}

Matching MatchFactory::createMatchAlgorithm() const
{
  return std::visit([](const auto& a) { return a.createMatchAlgorithm(); }
    , _impl);
}

} // namespace LIB

} // namespace SBG
