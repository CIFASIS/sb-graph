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

#include <sbg/pw_map.hpp>

namespace SBG {

namespace LIB {

// Type definitions ------------------------------------------------------------

bool LTMap::operator()(const SBGMap &x, const SBGMap &y) const { return LTInter(x.dom(), y.dom()); }

std::ostream &operator<<(std::ostream &out, const MapSet &ms)
{
  MapSet aux = ms;
  int sz = aux.size();

  out << "<<";
  if (sz > 0) {
    auto it = aux.begin();
    for (int i = 0; i < sz - 1; ++i) {
      out << *it << ", ";
      ++it;
    }
    out << *it;
  }
  out << ">>";

  return out;
}

// PWMap -----------------------------------------------------------------------

PWMap::PWMap() : maps_() {}
PWMap::PWMap(SBGMap sm) : maps_() { maps_ref().insert(sm); }
PWMap::PWMap(MapSet maps) : maps_(maps) {}

member_imp(PWMap, MapSet, maps);

// TODO
bool PWMap::operator==(const PWMap &other) const 
{
  return true;
}

std::ostream &operator(std::ostream &out, const PWMap &pw)
{
  out << pw.maps();

  return out;
}

// PWMap functions -------------------------------------------------------------

} // namespace LIB

} // namespace SBG
