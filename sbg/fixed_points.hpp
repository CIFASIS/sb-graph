/** @file fixed_points.hpp

 @brief <b>Fixed Points Information</b>

 Interface for fixed points information that will be interchanged between sets
 and expressions.

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

#ifndef SBGRAPH_SBG_FIXED_POINTS_HPP_
#define SBGRAPH_SBG_FIXED_POINTS_HPP_

#include "sbg/natural.hpp"

#include <optional>

namespace SBG {

namespace LIB {

enum class SolutionKind { kFree, kFixed };

class Solution {
public:
  Solution(const SolutionKind kind);
  Solution(const SolutionKind kind, const NAT value);

  const SolutionKind& kind() const;
  const std::optional<NAT>& value() const;

private:
  std::optional<NAT> _value;
  SolutionKind _kind;
};

using FixedPointsInfo = std::optional<std::vector<Solution>>;

} // namespace LIB

}  // namespace SBG

#endif // SBGRAPH_SBG_FIXED_POINTS_HPP_
