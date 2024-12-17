/** @file lexp_dto_converter.hpp

 @brief <b> LExpDTOConverter implementation</b>

 The LExpDTOConverter is used to convert a LExpDTO to
 the corresponding linear expression representation.
 Currently, it can convert to SBG::LIB::LExp,
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

#ifndef SBG_DTO_LEXP_CONVERTER_HPP
#define SBG_DTO_LEXP_CONVERTER_HPP

#include "sbg/dto/lexp_dto.hpp"
#include "sbg/lexp.hpp"

namespace SBG {

namespace API {

class LExpDTOConverter {
public:
  /**
   * @brief Converts a LExpDTO object to an actual linear
   * expression implementation (e.g., SBG::LIB::LExp).
   * 
   * @param dto: The LExpDTO object to convert.
   * @return SBG::LIB::LExp: The converted linear expression object.
   */
  static SBG::LIB::LExp convertToLExp(const LExpDTO& dto);
};

} // namespace API

} // namespace SBG

#endif // SBG_DTO_LEXP_CONVERTER_HPP
