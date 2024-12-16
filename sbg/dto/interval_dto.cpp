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

#include "sbg/dto/interval_dto.hpp"

namespace SBG {

namespace API {

IntervalDTO::IntervalDTO() : begin_(1), step_(1), end_(0) {}
IntervalDTO::IntervalDTO(const NAT &x) : begin_(x), step_(1), end_(x) {}
IntervalDTO::IntervalDTO(const NAT &begin, const NAT &step, const NAT &end) 
  : begin_(begin), step_(step), end_(end) 
{
}

// Set functions ---------------------------------------------------------------
bool IntervalDTO::isEmpty() const { return end_ < begin_; }

// Operators -------------------------------------------------------------------
std::ostream &operator<<(std::ostream &out, const IntervalDTO &i) 
{
  out << "[" << Util::toStr(i.begin_);
  if (i.step_ != 1)
    out << ":" << Util::toStr(i.step_);
  out << ":" << Util::toStr(i.end_) << "]";
 
  return out;
}

} // namespace API

} // namespace SBG
