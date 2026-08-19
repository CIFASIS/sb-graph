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

#include "algorithms/sorting/topological/topological_sorting.hpp"
#include "algorithms/sorting/topological/ts_impl.hpp"
#include "util/debug.hpp"
#include "util/time_profiler.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Topological Sorting Algorithm Interface -------------------------------------
////////////////////////////////////////////////////////////////////////////////

TopologicalSorting::TopologicalSorting() : _impl()
{
  TSKind kind = TS_IMPL.kind();
  switch (kind) {
    case TSKind::kMinVertex: {
      _impl = detail::MinVertexTS{};
      break;
    }

    default: {
      Util::ERROR("TopologicalSorting::operator<<: unsupported topological "
        , "sorting algorithm implementation");
    }
  }  
}

PWMap TopologicalSorting::calculate(const DirectedSBG& dsbg, const PWMap& pmap)
{
  auto text = "Total topological sorting execution time";
  Util::Internal::TimeProfiler profiler{text};

  return std::visit([&](auto& a) { return a.calculate(dsbg, pmap); }, _impl);
}

} // namespace LIB

} // namespace SBG
