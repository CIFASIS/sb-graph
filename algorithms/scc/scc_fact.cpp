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

#include "algorithms/scc/minreach_scc.hpp"
#include "algorithms/scc/scc_fact.hpp"
#include "util/debug.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Minimum Reachable SCC V1 Factory --------------------------------------------
////////////////////////////////////////////////////////////////////////////////

SCC MinReachSCCV1Fact::createSCCAlgorithm() const
{
  return SCC{SCCKind::kMinReachV1};
}

////////////////////////////////////////////////////////////////////////////////
// Minimum Reachable SCC V2 Factory --------------------------------------------
////////////////////////////////////////////////////////////////////////////////

SCC MinReachSCCV2Fact::createSCCAlgorithm() const
{
  return SCC{SCCKind::kMinReachV2};
}

////////////////////////////////////////////////////////////////////////////////
// Factory for clients --------------------------------------------------------- 
////////////////////////////////////////////////////////////////////////////////

SCCFactory::SCCFactory()
  : _kind(SCCKind::kMinReachV2), _impl(MinReachSCCV2Fact{}) {}

SCCFactory& SCCFactory::instance()
{
  static SCCFactory _instance;
  return _instance;
}

const SCCKind& SCCFactory::kind() const { return _kind; }

void SCCFactory::set_scc_fact(SCCKind kind)
{
  _kind = kind;
  switch (kind) {
    case SCCKind::kMinReachV1: {
      _impl = MinReachSCCV1Fact{};
      break;
    }

    case SCCKind::kMinReachV2: {
      _impl = MinReachSCCV2Fact{};
      break;
    }

    default: {
      Util::ERROR("Unsupported SCC algorithm implementation");
      break;
    }
  }
}

SCC SCCFactory::createSCCAlgorithm() const
{
  return std::visit([](const auto& a) { return a.createSCCAlgorithm(); }
    , _impl);
}

} // namespace LIB

} // namespace SBG
