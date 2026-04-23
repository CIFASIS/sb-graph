/** @file scc.hpp

 @brief <b>SBG SCC Algorithm Abstract Interface</b>

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

#ifndef SBGRAPH_ALGORITHMS_SCC_SCC_HPP_
#define SBGRAPH_ALGORITHMS_SCC_SCC_HPP_

#include "algorithms/scc/minreach_scc.hpp"
#include "algorithms/scc/scc_data.hpp"
#include "sbg/directed_sbg.hpp"
#include "sbg/pw_map.hpp"
#include "sbg/set.hpp"

#include <variant>

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// SCC Algorithm implementations -----------------------------------------------
////////////////////////////////////////////////////////////////////////////////

using SCCImpl = std::variant<MinReachSCCV1, MinReachSCCV2>;

} // namespace detail

enum class SCCKind { kMinReachV1, kMinReachV2 };

std::ostream& operator<<(std::ostream& out, const SCCKind kind);

////////////////////////////////////////////////////////////////////////////////
// SCC Algorithm ---------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

class SCC {
public:
  SCC(SCCKind kind);

  SCCData calculate(const DirectedSBG& dsbg);

private:
  SCCKind _kind;
  detail::SCCImpl _impl;
};

} // namespace LIB

} // namespace SBG

#endif // SBGRAPH_ALGORITHMS_SCC_SCC_HPP_
