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

#include "sbg/map_entry.hpp"

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Map entry -------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

MapEntry::MapEntry(const Set& s, const Expression& expr)
  : _map(s, expr), _perimeter(s.perimeter()) {}

MapEntry::MapEntry(const Map& m)
  : _map(m), _perimeter(m.domain().perimeter()) {}

const Map& MapEntry::map() const { return _map; }

const SetPerimeter& MapEntry::perimeter() const { return _perimeter; }

bool MapEntry::operator==(const MapEntry& other) const
{
  return _map == other._map;
}

bool MapEntry::operator<(const MapEntry& other) const 
{
  return _perimeter.min() < other._perimeter.min();
}

MaybeMapEntry MapEntry::compact(const MapEntry& other) const
{
  MaybeMap compacted = _map.compact(other._map);
  if (compacted) {
    return MaybeMapEntry{compacted.value()};
  }

  return {};
}
	
} // namespace detail

} // namespace LIB

} // namespace SBG
