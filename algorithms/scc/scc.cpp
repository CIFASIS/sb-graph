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

#include "algorithms/scc/mrv.hpp"
#include "algorithms/scc/scc.hpp"
#include "util/debug.hpp"
#include "util/logger.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// SCC Algorithm implementations -----------------------------------------------
////////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& out, const SCCKind kind)
{
  switch (kind) {
    case SCCKind::MinReachV1: {
      out << "minimum reachable V1";
      break;
    }

    case SCCKind::MinReachV2: {
      out << "minimum reachable V2";
      break;
    }

    default: {
      Util::ERROR("Unsupported SCC implementation");
      break;
    }
  }

  return out;
}

////////////////////////////////////////////////////////////////////////////////
// SCC Algorithm ---------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

SCC::SCC(SCCKind kind) : _kind(kind), _impl()
{
  switch (kind) {
    case SCCKind::MinReachV1: {
      _impl = detail::MinReachSCCV1{};
      break;
    }

    case SCCKind::MinReachV2: {
      _impl = detail::MinReachSCCV2{};
      break;
    }

    default: {
      Util::ERROR("Unsupported SCC implementation");
      break;
    }
  }
}

SCCData SCC::calculate(const DirectedSBG& dsbg)
{
  return std::visit([&](auto& a) { return a.calculate(dsbg); }, _impl);
}

} // namespace LIB

} // namespace SBG
