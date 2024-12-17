/** @file multidim_lexp_dto_converter.hpp

 @brief <b> MDLExpDTOConverter implementation</b>

 The MDLExpDTOConverter is used to convert a MDLExpDTO (ExpDTO) to the
 corresponding multi-dimensional linear expression representation.
 Currently, it can convert to SBG::LIB::MDLExp (Exp),
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

#ifndef SBG_DTO_MDLEXP_CONVERTER_HPP
#define SBG_DTO_MDLEXP_CONVERTER_HPP

#include "sbg/dto/multidim_lexp_dto.hpp"
#include "sbg/multidim_lexp.hpp"

namespace SBG {

namespace API {

class MDLExpDTOConverter {
public:
  /**
   * @brief Converts a ExpDTO object to an actual multi-dimensional
   * linear expression implementation (e.g., SBG::LIB::Exp).
   * 
   * @param dto: The ExpDTO object to convert.
   * @return SBG::LIB::Exp: The converted multi-dimensional linear expression object.
   */
  static SBG::LIB::Exp convertToExp(const ExpDTO& dto);
};

typedef MDLExpDTOConverter ExpDTOConverter;

} // namespace API

} // namespace SBG

#endif // SBG_DTO_MDLEXP_CONVERTER_HPP
