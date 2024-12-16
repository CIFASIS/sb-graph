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

#include "sbg/dto/converters/multidim_inter_dto_converter.hpp"
#include "sbg/dto/converters/pw_mdinter_dto_converter.hpp"

namespace SBG {

namespace API {
SBG::LIB::OrdSet PWMDInterDTOConverter::convertToOrdSet(const SetDTO& dto) {
  SBG::LIB::OrdSet ord_set;
  for (const SetPieceDTO& set_piece_dto : dto.pieces_) {
    ord_set.emplace(SetPieceDTOConverter::convertToSetPiece(set_piece_dto));
  }

  return ord_set;
}

SBG::LIB::UnordSet PWMDInterDTOConverter::convertToUnordSet(const PWMDInterDTO& dto) {
  SBG::LIB::UnordSet unord_set;
  for (const SetPieceDTO& set_piece_dto : dto.pieces_) {
    unord_set.emplace(SetPieceDTOConverter::convertToSetPiece(set_piece_dto));
  }

  return unord_set;
}

} // namespace API

} // namespace SBG