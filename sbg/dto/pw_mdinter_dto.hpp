/** @file pw_mdinter_dto.hpp

 @brief <b>Piecewise multi-dimensional interval dto implementation</b>

 The PWMDInterDTO class is a Data Transfer Object (DTO) for handling 
 raw data, such as from JSON. It is designed for conversion into
 concrete implementations like OrdSet/UnordSet via a dedicated converter.

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

#ifndef SBG_DTO_PW_MDINTERVAL_HPP
#define SBG_DTO_PW_MDINTERVAL_HPP

#include <vector>

#include "sbg/dto/multidim_inter_dto.hpp"

namespace SBG {

namespace API {

class PWMDInterDTOConverter;

// Container -------------------------------------------------------------------

typedef std::vector<SetPieceDTO> SetPieceDTOVector;
std::ostream &operator<<(std::ostream &out, const SetPieceDTOVector &ii);

struct PWMDInterDTO {
  member_class(SetPieceDTOVector, pieces);

  /**
   * @brief Constructors don't check if intervals are disjoint (performance).
   */
  PWMDInterDTO();
  PWMDInterDTO(Util::MD_NAT x);
  PWMDInterDTO(IntervalDTO i);
  PWMDInterDTO(SetPieceDTO mdi);
  PWMDInterDTO(SetPieceDTOVector container);

  std::size_t size() const;
  void emplace(SetPieceDTO mdi);
  void emplaceBack(SetPieceDTO mdi);

  /**
   * @brief Traditional set operations.
   */
  bool isEmpty() const;

  friend std::ostream &operator<<(std::ostream &out, const PWMDInterDTO &i);
  friend class PWMDInterDTOConverter;
};

typedef PWMDInterDTO SetDTO;

} // namespace API

}  // namespace SBG

#endif
