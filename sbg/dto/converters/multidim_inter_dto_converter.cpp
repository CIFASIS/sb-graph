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

#include "sbg/dto/converters/interval_dto_converter.hpp"
#include "sbg/dto/converters/multidim_inter_dto_converter.hpp"
#include "sbg/dto/interval_dto.hpp"
#include "sbg/dto/multidim_inter_dto.hpp"
#include "sbg/multidim_inter.hpp"
#include "sbg/interval.hpp"

namespace SBG {

namespace API {

SBG::LIB::MultiDimInter MultiDimInterDTOConverter::convertToMultiDimInter(const MultiDimInterDTO& dto) {
  SBG::LIB::InterVector intervals;
  intervals.reserve(dto.intervals_.size());

  std::transform(dto.intervals_.begin(), dto.intervals_.end(), intervals.begin(),
                  [](const IntervalDTO& intervalDTO) {
                      return IntervalDTOConverter::convertToInterval(intervalDTO);
                  });

  return SBG::LIB::MultiDimInter(intervals);
}

} // namespace API

} // namespace SBG