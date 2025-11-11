/** @file map_entry.hpp

 @brief <b>Map Entry</b>

 Auxiliar module to implement ordered PWMaps.

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

#ifndef SBG_MAP_ENTRY_HPP
#define SBG_MAP_ENTRY_HPP

#include "sbg/map.hpp"

namespace SBG {

namespace LIB {

namespace Internal {

using SetPerimeter = std::pair<MD_NAT, MD_NAT>;
using MapEntry = std::pair<Map, SetPerimeter>;
using OrdMapCollection = std::vector<MapEntry>;

SetPerimeter calculatePerimeter(const Set &s);
bool doInt(const SetPerimeter& p1, const SetPerimeter& p2);
MapEntry createMapEntry(const Map& m);
bool operator<(const MapEntry& mpe1, const MapEntry& mpe2);
void pushBack(OrdMapCollection& ord_pw, const MapEntry& m);
void pushBack(OrdMapCollection& ord_pw, const Map& m);
void emplaceHint(OrdMapCollection& ord_pw, const Map& m, NAT hint);
void advanceHint(OrdMapCollection& ord_pw, const MD_NAT crit, NAT hint);

} // namespace Internal

} // namespace LIB

} // namespace SBG

#endif
