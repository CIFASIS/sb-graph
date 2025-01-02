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

#include "sbg/dto/pw_mdinter_dto.hpp"

namespace SBG {

namespace API {

// Type definitions ------------------------------------------------------------

std::ostream &operator<<(std::ostream &out, const SetPieceDTOVector &ii) 
{
  std::size_t sz = ii.size();

  out << "{";
  if (sz > 0) {
    unsigned int j = 0;
    for (const SetPieceDTO &mdi : ii) { 
      if (j < sz - 1)
        out << mdi << ", "; 
      else
        out << mdi;

      ++j;
    }
  }
  out << "}";

  return out;
}

// PWMDInterDTO ------------------------------------------------------------------

PWMDInterDTO::PWMDInterDTO() : pieces_() {}
PWMDInterDTO::PWMDInterDTO(MD_NAT x) : pieces_() {
  pieces_.push_back(SetPieceDTO(x));
}
PWMDInterDTO::PWMDInterDTO(IntervalDTO i) : pieces_() {
  if (!i.isEmpty())
    pieces_.push_back(SetPieceDTO(i));
}
PWMDInterDTO::PWMDInterDTO(SetPieceDTO mdi) : pieces_() { 
  if (!mdi.isEmpty())
    pieces_.push_back(mdi);
}
PWMDInterDTO::PWMDInterDTO(SetPieceDTOVector container) : pieces_() {
  for (const SetPieceDTO &mdi : container) {
    if (!mdi.isEmpty())
      pieces_.push_back(mdi);
  }
}

member_imp(PWMDInterDTO, SetPieceDTOVector, pieces);

SetPieceDTOVector::iterator PWMDInterDTO::begin() { return pieces_.begin(); }

std::size_t PWMDInterDTO::size() const { return pieces_.size(); }

void PWMDInterDTO::emplace(SetPieceDTO mdi)
{
  if (!mdi.isEmpty())
    pieces_.push_back(mdi);
}
void PWMDInterDTO::emplaceBack(SetPieceDTO mdi)
{
  if (!mdi.isEmpty())
    pieces_.push_back(mdi);
}

bool PWMDInterDTO::isEmpty() const { return pieces_.empty(); }

std::ostream &operator<<(std::ostream &out, const PWMDInterDTO &pwi)
{
  out << pwi.pieces_;

  return out;
}

} // namespace API

} // namespace SBG
