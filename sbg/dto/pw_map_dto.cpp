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

#include "sbg/dto/pw_map_dto.hpp"

namespace SBG {

namespace API {

// Type definitions ------------------------------------------------------------

std::ostream &operator<<(std::ostream &out, const MapDTOVector &ms)
{
  MapDTOVector aux = ms;
  int sz = aux.size();

  out << "<<";
  if (sz > 0) {
    auto it = aux.begin();
    for (int i = 0; i < sz - 1; ++i) {
      if (!it->dom().isEmpty())
        out << *it << ", ";
      ++it;
    }
    if (!it->dom().isEmpty())
      out << *it;
  }
  out << ">>";

  return out;
}

// PWMapDTO -----------------------------------------------------------------------

PWMapDTO::PWMapDTO() : maps_() {}
PWMapDTO::PWMapDTO(SetDTO s) : maps_() {
  if (!s.isEmpty()) {
    MapDTO map(s, ExpDTO(s.begin()->arity(), LExpDTO()));
    maps_.emplace_back(map);
  }
}
PWMapDTO::PWMapDTO(MapDTO map) : maps_() {
  if (!map.dom().isEmpty())
    maps_.emplace_back(map);
}
PWMapDTO::PWMapDTO(MapDTOVector maps) : maps_(maps) {}

member_imp(PWMapDTO, MapDTOVector, maps);

void PWMapDTO::emplace(MapDTO map) {
  if (!map.dom().isEmpty())
    maps_.emplace_back(map);
}

void PWMapDTO::emplaceBack(MapDTO map)
{
  if (!map.dom().isEmpty())
    maps_.emplace_back(map);
}

// PWMapDTO functions -------------------------------------------------------------

bool PWMapDTO::isEmpty() const { return maps_.empty(); }

// std::ostream operators ------------------------------------------------------

std::ostream &operator<<(std::ostream &out, const PWMapDTO &pw)
{
  out << pw.maps();

  return out;
}

} // namespace API

} // namespace SBG
