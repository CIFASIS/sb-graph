/** @file pw_mdinter_dto_converter.hpp

 @brief <b> PWMDInterDTOConverter implementation</b>

 The PWMDInterDTOConverter is used to convert a PWMDInterDTO (SetDTO)
 to the corresponding piecewise multi-dimensional interval representation.
 Currently, it can convert to SBG::LIB::OrdSet and SBG::LIB::UnordSet,
 but more conversions can be added in the future.

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

#ifndef SBG_DTO_PW_MDINTERVAL_CONVERTER_HPP
#define SBG_DTO_PW_MDINTERVAL_CONVERTER_HPP

#include "sbg/dto/pw_mdinter_dto.hpp"
#include "sbg/ord_pw_mdinter.hpp"
#include "sbg/unord_pw_mdinter.hpp"

namespace SBG {

namespace API {

class PWMDInterDTOConverter {
public:
  /**
   * @brief Converts a SetDTO object to an actual piecewise multi-dimensional 
   * interval implementation (e.g., SBG::LIB::OrdSet / SBG::LIB::UnordSet).
   * 
   * @param dto: The SetDTO object to convert.
   * @return SBG::LIB::OrdSet/UnordSet: The converted interval object.
   */
  static SBG::LIB::OrdSet convertToOrdSet(const SetDTO& dto);
  static SBG::LIB::UnordSet convertToUnordSet(const SetDTO& dto);
};

typedef PWMDInterDTOConverter SetDTOConverter;

} // namespace API

} // namespace SBG

#endif // SBG_DTO_PW_MDINTERVAL_CONVERTER_HPP
