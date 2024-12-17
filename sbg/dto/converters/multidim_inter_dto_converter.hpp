/** @file interval_dto_converter.hpp

 @brief <b>MultiDimInterDTOConverter implementation</b>

 The MultiDimInterDTOConverter is used to convert a MultiDimInterDTO (SetPieceDTO)
 to the corresponding multidimensional interval representation. Currently, it can
 convert to LIB::SetPiece, but more conversions can/will be added in the future.

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

#ifndef SBG_DTO_MULTIDIM_INTERVAL_CONVERTER_HPP
#define SBG_DTO_MULTIDIM_INTERVAL_CONVERTER_HPP

#include "sbg/dto/multidim_inter_dto.hpp"
#include "sbg/multidim_inter.hpp"

namespace SBG {

namespace API {

class MultiDimInterDTOConverter {
public:
  /**
   * @brief Converts a SetPieceDTO object to an actual multidimensional
   * interval implementation (e.g., SBG::LIB::MultiDimInter).
   * 
   * @param dto The SetPieceDTO object to convert.
   * @return SBG::LIB::SetPiece The converted interval object.
   */
  static SBG::LIB::SetPiece convertToSetPiece(const SetPieceDTO& dto);
};

typedef MultiDimInterDTOConverter SetPieceDTOConverter;

} // namespace API

} // namespace SBG

#endif // SBG_DTO_MULTIDIM_INTERVAL_CONVERTER_HPP
