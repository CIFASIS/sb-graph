/** @file multidim_lexp_dto.hpp

 @brief <b>Multi-dimensional linear expressions DTO implementation</b>

 The MDLExpDTO class is a Data Transfer Object (DTO) for handling 
 raw data, such as from JSON. It is designed for conversion into
 concrete implementations like MDLExp via a dedicated converter.

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

#ifndef SBG_DTO_MULTIDIM_LEXP_HPP
#define SBG_DTO_MULTIDIM_LEXP_HPP

#include "sbg/dto/lexp_dto.hpp"

namespace SBG {

namespace API {

class MDLExpDTOConverter;

typedef std::vector<LExpDTO> LExpDTOVector;

struct MDLExpDTO {
  member_class(LExpDTOVector, exps);

  MDLExpDTO();
  MDLExpDTO(Util::MD_NAT x); // Expression mapping to x
  MDLExpDTO(LExpDTO le);
  MDLExpDTO(unsigned int nmbr_copies, LExpDTO le);
  MDLExpDTO(LExpDTOVector v);

  void emplaceBack(LExpDTO le);
  LExpDTO &operator[](std::size_t n);
  const LExpDTO &operator[](std::size_t n) const;

  friend std::ostream &operator<<(std::ostream &out, const MDLExpDTO &mdle); 
  friend class MDLExpDTOConverter;
};

typedef MDLExpDTO ExpDTO;

} // namespace API

} // namespace SBG

#endif
