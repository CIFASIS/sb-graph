/** @file multidim_inter.hpp

 @brief <b>Multi-dimensional interval implementation</b>

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

#ifndef SBG_DTO_MULTIDIM_INTERVAL_HPP
#define SBG_DTO_MULTIDIM_INTERVAL_HPP

#include "sbg/dto/interval_dto.hpp"

namespace SBG {

namespace API {

using MD_NAT = Util::MD_NAT;
using NAT = Util::NAT;

typedef std::vector<IntervalDTO> IntervalDTOVector;

struct MultiDimInterDTO {

  member_class(IntervalDTOVector, intervals);

  MultiDimInterDTO();
  MultiDimInterDTO(const MD_NAT &x);
  MultiDimInterDTO(const IntervalDTO &i);
  MultiDimInterDTO(const unsigned int &nmbr_copies, const IntervalDTO &i);
  MultiDimInterDTO(const IntervalDTOVector &iv);

  void emplaceBack(IntervalDTO i);
  IntervalDTO &operator[](std::size_t n);
  const IntervalDTO &operator[](std::size_t n) const;

  friend std::ostream &operator<<(std::ostream &out, const MultiDimInterDTO &i);
  friend class MultiDimInterDTOConverter;
};

typedef MultiDimInterDTO SetPieceDTO;

} // namespace LIB

}  // namespace SBG

#endif
