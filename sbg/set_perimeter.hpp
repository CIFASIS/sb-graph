/** @file set_perimeter.pp

 @brief <b>Set Perimeter</b>

 The set perimeter is the smallest dense MultiDimInter that contains all of the
 elements of the set. Thus, is can be defined using only two multi-dimensional
 values.

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

#ifndef SBGRAPH_SBG_SET_PERIMETER_HPP_
#define SBGRAPH_SBG_SET_PERIMETER_HPP_

#include "sbg/natural.hpp"

namespace SBG {

namespace LIB {

class SetPerimeter {
public:
  SetPerimeter(const MD_NAT& min, const MD_NAT& max);

  const MD_NAT& min() const;
  const MD_NAT& max() const;

  bool overlap(const SetPerimeter& other) const;

private:
  MD_NAT _min;
  MD_NAT _max;
};

} // namespace LIB

} // namespace SBG

#endif // SBGRAPH_SBG_SET_PERIMETER_HPP_
