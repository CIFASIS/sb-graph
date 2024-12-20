/*******************************************************************************

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

#include "sbg/dto/converters/pw_map_dto_converter.hpp"
#include "sbg/dto/converters/map_dto_converter.hpp"

namespace SBG {

namespace API {
SBG::LIB::BasePWMap PWMapDTOConverter::convertToBasePWMap(const PWMapDTO& dto) {
  SBG::LIB::BasePWMap base_pw_map;
  for (const MapDTO& map_dto : dto.maps_) {
    base_pw_map.emplaceBack(MapDTOConverter::convertToBaseMap(map_dto));
  }

  return base_pw_map;
}

SBG::LIB::CanonPWMap PWMapDTOConverter::convertToCanonPWMap(const PWMapDTO& dto) {
  SBG::LIB::CanonPWMap canon_pw_map;
  for (const MapDTO& map_dto : dto.maps_) {
    canon_pw_map.emplaceBack(MapDTOConverter::convertToCanonMap(map_dto));
  }

  return canon_pw_map;
}

} // namespace API

} // namespace SBG