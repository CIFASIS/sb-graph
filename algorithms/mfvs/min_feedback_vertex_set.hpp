/** @file cut_vertex.hpp

 @brief <b>SBG Vertex Cut Set Algorithm Abstract Interface</b>

 <hr>

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

#ifndef SBGRAPH_ALGORITHMS_MFVS_MIN_FEEDBACK_VERTEX_SET_HPP_
#define SBGRAPH_ALGORITHMS_MFVS_MIN_FEEDBACK_VERTEX_SET_HPP_

#include "algorithms/mfvs/greedy_mfvs.hpp"
#include "sbg/directed_sbg.hpp"

#include <iosfwd>
#include <variant>

namespace SBG {

namespace LIB {

namespace detail {

///////////////////////////////////////////////////////////////////////////////
// Minimum Feedback Vertex Set Implementations --------------------------------
///////////////////////////////////////////////////////////////////////////////

using MFVSImpl = std::variant<GreedyMFVS>;

}

enum class MFVSKind { kGreedy };

std::ostream& operator<<(std::ostream& out, const MFVSKind kind);

///////////////////////////////////////////////////////////////////////////////
// Minimum Feedback Vertex Set Algorithm --------------------------------------
///////////////////////////////////////////////////////////////////////////////

class MinFeedbackVertexSet {
public:
  MinFeedbackVertexSet(MFVSKind kind);

  Set calculate(const DirectedSBG& dsbg);

private:
  MFVSKind _kind;
  detail::MFVSImpl _impl;
};

} // namespace LIB

} // namespace SBG

#endif // SBGRAPH_ALGORITHMS_MFVS_MIN_FEEDBACK_VERTEX_SET_HPP_
