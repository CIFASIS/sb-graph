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

#include "algorithms/tearing/tearing.hpp"
#include "algorithms/tearing/tearing_data.hpp"
#include "algorithms/tearing/tearing_impl.hpp"
#include "algorithms/tearing/tearing_v1.hpp"
#include "sbg/directed_sbg.hpp"
#include "util/debug.hpp"
#include "util/logger.hpp"
#include "util/time_profiler.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Tearing Algorithm ---------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

Tearing::Tearing() : _impl()
{
  TearingKind kind = TEARING_IMPL.kind();
  switch (kind) {
    case TearingKind::kTearingV1: {
      _impl = detail::TearingImpl{};
      break;
    }

    default: {
      Util::ERROR("Unsupported Tearing implementation");
      break;
    }
  }
}

TearingData Tearing::calculate(const DirectedSBG& dsbg)
{
  Util::Internal::TimeProfiler profiler{"Total SCC execution time: "};

  return std::visit([&](auto& a) { return a.calculate(dsbg); }, _impl);
}

} // namespace LIB

} // namespace SBG
