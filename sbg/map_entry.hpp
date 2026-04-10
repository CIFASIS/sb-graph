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

#ifndef SBGRAPH_SBG_MAP_ENTRY_HPP_
#define SBGRAPH_SBG_MAP_ENTRY_HPP_

#include "sbg/expression.hpp"
#include "sbg/map.hpp"
#include "sbg/set.hpp"
#include "sbg/perimeter.hpp"

#include <optional>

namespace SBG {

namespace LIB {

namespace detail {

class MapEntry;

using MaybeMapEntry = std::optional<MapEntry>;

class MapEntry {
public:
  MapEntry(const Set& s, const Expression& expr);
  MapEntry(const Map& m);

  const Map& map() const;
  const Perimeter& perimeter() const;

  bool operator==(const MapEntry& other) const;
  bool operator<(const MapEntry& other) const;

  MaybeMapEntry compact(const MapEntry& other) const;

private:
  Map _map;
  Perimeter _perimeter;
};

} // namespace detail

} // namespace LIB

} // namespace SBG

#endif // SBGRAPH_SBG_MAP_ENTRY_HPP_
