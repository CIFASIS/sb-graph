/** @file map_dto_converter.hpp

 @brief <b> SBGMapDTOConverter implementation</b>

 The SBGMapDTOConverter is used to convert a SBGMapDTO (MapDTO)
 to the corresponding SBG map representation.
 Currently, it can convert to SBG::LIB::SBGMap<UnordSet> (BaseMap) and
 SBG::LIB::SBGMap<OrdSet> (CanonMap), but more conversions can be added.

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

#ifndef SBG_DTO_MAP_CONVERTER_HPP
#define SBG_DTO_MAP_CONVERTER_HPP

#include "sbg/dto/map_dto.hpp"
#include "sbg/map.hpp"

namespace SBG {

namespace API {

class SBGMapDTOConverter {
public:
  /**
   * @brief Converts a MapDTO object to an actual SBG map implementation
   * (e.g., SBG::LIB::BaseMap / SBG::LIB::CanonMap).
   * 
   * @param dto: The MapDTO object to convert.
   * @return SBG::LIB::BaseMap/CanonMap: The converted SBG map object.
   */
  static SBG::LIB::BaseMap convertToBaseMap(const MapDTO& dto);
  static SBG::LIB::CanonMap convertToCanonMap(const MapDTO& dto);
};

typedef SBGMapDTOConverter MapDTOConverter;

} // namespace API

} // namespace SBG

#endif // SBG_DTO_MAP_CONVERTER_HPP
