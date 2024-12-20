/** @file pw_map.hpp

 @brief <b>Piecewise map DTO implementation</b>

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

#ifndef SBG_DTO_PWMAP_HPP
#define SBG_DTO_PWMAP_HPP

#include "sbg/dto/map_dto.hpp"

namespace SBG {

namespace API {

class PWMapDTOConverter;

/**
 * @brief Unordered collection of map DTOs.
 */

using MapDTOVector = std::vector<MapDTO>; // ex MapSet.
std::ostream &operator<<(std::ostream &out, const MapDTOVector &ms);

struct PWMapDTO {
  member_class(MapDTOVector, maps);

  PWMapDTO();
  PWMapDTO(SetDTO s); // Create id with s as domain
  PWMapDTO(MapDTO m);
  PWMapDTO(MapDTOVector maps);

  std::size_t size() const;
  void emplace(MapDTO m);
  void emplaceBack(MapDTO m);

  /**
   * @brief Traditional map operations.
   */
  bool isEmpty() const;

  friend std::ostream &operator<<(std::ostream &out, const PWMapDTO &pw);
  friend class PWMapDTOConverter;
};

} // namespace API

}  // namespace SBG

#endif
