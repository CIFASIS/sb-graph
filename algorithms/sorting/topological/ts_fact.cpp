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

#include "algorithms/sorting/topological/ts_fact.hpp"
#include "util/debug.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Minimum Vertex Topological Sorting Factory ----------------------------------
////////////////////////////////////////////////////////////////////////////////

TopologicalSorting MinVertexTSFact::createTSAlgorithm() const
{
  return TopologicalSorting{TSKind::kMinVertex};
}

////////////////////////////////////////////////////////////////////////////////
// Factory for clients --------------------------------------------------------- 
////////////////////////////////////////////////////////////////////////////////

TSFactory::TSFactory()
  : _kind(TSKind::kMinVertex), _impl(MinVertexTSFact{}) {}

TSFactory& TSFactory::instance()
{
  static TSFactory _instance;
  return _instance;
}

const TSKind& TSFactory::kind() const { return _kind; }

void TSFactory::set_ts_fact(TSKind kind)
{
  _kind = kind;
  switch (kind) {
    case TSKind::kMinVertex: {
      _impl = MinVertexTSFact{};
      break;
    }

    default: {
      Util::ERROR("TSFactory::set_ts_fact: unsupported topological sorting ",
        "implementation");
      break;
    }
  }
}

TopologicalSorting TSFactory::createTSAlgorithm() const
{
  return std::visit([](const auto& a) { return a.createTSAlgorithm(); }
    , _impl);
}

} // namespace LIB

} // namespace SBG
