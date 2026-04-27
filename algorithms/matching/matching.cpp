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

#include "algorithms/matching/matching.hpp"
#include "util/debug.hpp"
#include "util/time_profiler.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Matching Algorithm implementations ------------------------------------------
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

////////////////////////////////////////////////////////////////////////////////
// Matching Algorithm Interface ------------------------------------------------ 
////////////////////////////////////////////////////////////////////////////////

Matching::Matching(MatchKind kind) : _kind(kind), _impl()
{
  switch (kind) {
    case MatchKind::kBFSPaths: {
      _impl = detail::BFSMatching{};
      break;
    }

    default: {
      Util::ERROR("Unsupported matching algorithm implementation");
    }
  }  
}

MatchData Matching::calculate(const BipartiteSBG& bsbg)
{
  Util::Internal::TimeProfiler profiler{"Total matching execution time: "};

  return std::visit([&](auto& a) { return a.calculate(bsbg); }, _impl);
}

} // namespace LIB

} // namespace SBG
