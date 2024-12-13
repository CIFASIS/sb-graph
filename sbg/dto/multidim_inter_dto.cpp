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

#include "sbg/dto/multidim_inter_dto.hpp"

namespace SBG {

namespace API {

MultiDimInterDTO::MultiDimInterDTO() : intervals_() {}
MultiDimInterDTO::MultiDimInterDTO(const MD_NAT &x) : intervals_() {
  for (NAT xi : x)
    intervals_.emplace_back(IntervalDTO(xi, 1, xi));
}
MultiDimInterDTO::MultiDimInterDTO(const IntervalDTO &i) : intervals_()
{
  intervals_.emplace_back(i);
}
MultiDimInterDTO::MultiDimInterDTO(const unsigned int &nmbr_copies
                             , const IntervalDTO &i) : intervals_() {
  for (unsigned int j = 0; j < nmbr_copies; ++j)
    intervals_.emplace_back(i);
}
MultiDimInterDTO::MultiDimInterDTO(const IntervalDTOVector &iv) : intervals_(iv) {}

member_imp(MultiDimInterDTO, IntervalDTOVector, intervals);

void MultiDimInterDTO::emplaceBack(IntervalDTO i) 
{
  if (i.isEmpty())
    intervals_ = IntervalDTOVector();
  else
    intervals_.emplace_back(i);
  return;
}

IntervalDTO &MultiDimInterDTO::operator[](std::size_t n)
{
  return intervals_[n];
}

const IntervalDTO &MultiDimInterDTO::operator[](std::size_t n) const
{
  return intervals_[n];
}

std::ostream &operator<<(std::ostream &out, const MultiDimInterDTO &mdi)
{
  std::size_t sz = mdi.intervals_.size();

  if (sz > 0) {
    for (std::size_t j = 0; j < sz - 1; ++j)
      out << mdi[j] << "x";
    out << mdi[sz-1];
  }

  return out;
}

} // namespace API

} // namespace SBG
