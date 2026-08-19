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

#include "algorithms/mfvs/min_feedback_vertex_set.hpp"
#include "algorithms/mfvs/mfvs_impl.hpp"
#include "util/debug.hpp"
#include "util/time_profiler.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// MFVS Algorithm Interface ----------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

MinFeedbackVertexSet::MinFeedbackVertexSet() : _impl()
{
  MFVSKind kind = MFVS_IMPL.kind();
  switch (kind) {
    case MFVSKind::kGreedy: {
      _impl = detail::GreedyMFVS{};
      break;
    }

    case MFVSKind::kSmallSV: {
      _impl = detail::SmallestSVMFVS{};
      break;
    }

    default: {
      Util::ERROR("MinFeedbackVertexSet: unsupported MFVS implementation");
      break;
    }
  }
}

Set MinFeedbackVertexSet::calculate(const DirectedSBG& dsbg)
{
  Util::Internal::TimeProfiler profiler{"Total MFVS exec time"};

  return std::visit([&](auto& a) { return a.calculate(dsbg); }, _impl);
}

} // namespace LIB

} // namespace SBG
