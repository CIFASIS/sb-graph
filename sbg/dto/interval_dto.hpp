/** @file interval_dto.hpp

 @brief <b>IntervalDTO implementation</b>

 The IntervalDTO class is a Data Transfer Object (DTO) for handling
 raw data, such as from JSON. It is designed for conversion into
 concrete implementations like Interval via a dedicated converter.

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

#ifndef SBG_DTO_INTERVAL_HPP
#define SBG_DTO_INTERVAL_HPP

#include "util/defs.hpp"

namespace SBG {

namespace API {

class IntervalDTOConverter;

using NAT = Util::NAT;

struct IntervalDTO {

  IntervalDTO();
  IntervalDTO(const NAT &x);
  IntervalDTO(const NAT &begin, const NAT &step, const NAT &end);

  /**
   * @brief Traditional set operations.
   */
  bool isEmpty() const;

  private:
  NAT begin_;
  NAT step_;
  NAT end_;

  friend std::ostream &operator<<(std::ostream &out, const IntervalDTO &i);
  friend class IntervalDTOConverter;
};

} // namespace API

}  // namespace SBG

#endif
