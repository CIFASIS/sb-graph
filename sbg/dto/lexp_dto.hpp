/** @file lexp_dto.hpp

 @brief <b>Linear expressions DTO implementation</b>

 The LExpDTO class is a Data Transfer Object (DTO) for handling
 raw data, such as from JSON. It is designed for conversion into
 concrete implementations like LExp via a dedicated converter.

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

#ifndef SBG_DTO_LEXP_HPP
#define SBG_DTO_LEXP_HPP

#include "util/defs.hpp"

namespace SBG {

namespace API {

class LExpDTOConverter;

using RAT = Util::RATIONAL;

struct LExpDTO {
  member_class(RAT, slope);
  member_class(RAT, offset);

  LExpDTO();
  LExpDTO(RAT slope, RAT offset);

  friend std::ostream &operator<<(std::ostream &out, const LExpDTO &le);
  friend class LExpDTOConverter;
};

} // namespace API

} // namespace SBG

#endif
