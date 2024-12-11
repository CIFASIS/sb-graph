/** @file interval_dto_converter.hpp

 @brief <b>IntervalDTOConverter implementation</b>

 The IntervalDTOConverter provides the methods to convert an IntervalDTO to
 the corresponding interval representation. Currently, it can convert to 
 LIB::Interval, but more conversions can/will be added in the future.

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

#ifndef SBG_DTO_INTERVAL_CONVERTER_HPP
#define SBG_DTO_INTERVAL_CONVERTER_HPP

#include "sbg/dto/interval_dto.hpp"
#include "sbg/interval.hpp"

namespace SBG {

namespace API {

class IntervalDTOConverter {
public:
    /**
     * @brief Converts an IntervalDTO object to an actual interval implementation (e.g., LIB::Interval).
     * 
     * @param dto The IntervalDTO object to convert.
     * @return SBG::LIB::Interval The converted interval object.
     */
    static SBG::LIB::Interval convertToInterval(const IntervalDTO& dto);
};

} // namespace API

} // namespace SBG

#endif // SBG_DTO_INTERVAL_CONVERTER_HPP
