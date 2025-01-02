/** @file map_dto.hpp

 @brief <b>SBG map DTO implementation</b>

 The SBGMapDTO class is a Data Transfer Object (DTO) for handling 
 raw data, such as from JSON. It is designed for conversion into
 concrete implementations like SBGMap via a dedicated converter.

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

#ifndef SBG_DTO_MAP_HPP
#define SBG_DTO_MAP_HPP

#include "sbg/dto/multidim_lexp_dto.hpp"
#include "sbg/dto/pw_mdinter_dto.hpp"
#include "util/debug.hpp"

namespace SBG {

namespace API {

class SBGMapDTOConverter;

struct SBGMapDTO {
  member_class(SetDTO, dom);
  member_class(ExpDTO, exp);

  SBGMapDTO();
  SBGMapDTO(Util::MD_NAT x, ExpDTO exp);
  SBGMapDTO(IntervalDTO i, LExpDTO le);
  SBGMapDTO(SetPieceDTO mdi, ExpDTO exp);
  SBGMapDTO(SetDTO dom, ExpDTO exp);

  /**
   * @brief Traditional map operations.
   */
  bool isEmpty() const;

  friend std::ostream &operator<<(std::ostream &out, const SBGMapDTO &sbgmap);
  friend class SBGMapDTOConverter;
};

typedef SBGMapDTO MapDTO;

} // namespace API

}  // namespace SBG

#endif
