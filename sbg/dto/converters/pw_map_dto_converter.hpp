/** @file map_dto_converter.hpp

 @brief <b> PWMapDTOConverter implementation</b>

 The PWMapDTOConverter is used to convert a PWMapDTO
 to the corresponding piecewise SBG map representation.
 Currently, it can convert to SBG::LIB::PWMap<UnordSet> (BasePWMap) and
 SBG::LIB::PWMap<OrdSet> (CanonPWMap), but more conversions can be added.

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

#ifndef SBG_DTO_PWMAP_CONVERTER_HPP
#define SBG_DTO_PWMAP_CONVERTER_HPP

#include "sbg/dto/pw_map_dto.hpp"
#include "sbg/pw_map.hpp"

namespace SBG {

namespace API {

class PWMapDTOConverter {
public:
  /**
   * @brief Converts a PWMapDTO object to an actual SBG piecewise map
   * implementation (e.g., SBG::LIB::BasePWMap / SBG::LIB::CanonPWMap).
   * 
   * @param dto: The PWMapDTO object to convert.
   * @return SBG::LIB::BasePWMap/CanonPWMap: The converted SBG piecewise map object.
   */
  static SBG::LIB::BasePWMap convertToBasePWMap(const PWMapDTO& dto);
  static SBG::LIB::CanonPWMap convertToCanonPWMap(const PWMapDTO& dto);
};

} // namespace API

} // namespace SBG

#endif // SBG_DTO_PWMAP_CONVERTER_HPP
