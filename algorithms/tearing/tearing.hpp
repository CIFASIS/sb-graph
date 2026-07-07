/** @file tearing.hpp

 @brief <b>SBG Tearing Algorithm Abstract Interface</b>

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

#ifndef SBGRAPH_ALGORITHMS_TEARING_TEARING_HPP_
#define SBGRAPH_ALGORITHMS_TEARING_TEARING_HPP_

#include "sbg/directed_sbg.hpp"
#include "algorithms/tearing/tearing_v1.hpp"

#include <variant>

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Tearing Algorithm implementations -------------------------------------------
////////////////////////////////////////////////////////////////////////////////

using TearingImpl = std::variant<TearingV1>;

} // namespace detail

////////////////////////////////////////////////////////////////////////////////
// Tearing Algorithm -----------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

class Tearing {
public:
  Tearing();

  TearingData calculate(const DirectedSBG& dsbg);

private:
  detail::TearingImpl _impl;
};

} // namespace LIB

} // namespace SBG

#endif // SBGRAPH_ALGORITHMS_TEARING_TEARING_HPP_