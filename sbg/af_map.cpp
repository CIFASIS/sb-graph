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

#include "sbg/af_map.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Map AF ----------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

MapAF::MapAF() {}

Map MapAF::createMap() const
{
  return Map();
}

Map MapAF::createMap(MD_NAT x, Exp exp) const
{
  return Map(x, exp);
}

Map MapAF::createMap(Interval i, LExp le) const
{
  return Map(i, le);
}

Map MapAF::createMap(SetPiece mdi, Exp exp) const
{
  return Map(mdi, exp);
}

Map MapAF::createMap(Set s, Exp exp) const
{
  return Map(s, exp);
}

////////////////////////////////////////////////////////////////////////////////
// Factory for clients --------------------------------------------------------- 
////////////////////////////////////////////////////////////////////////////////

MapFactory::MapFactory() : map_fact_(std::make_unique<MapAF>()) {}

MapAF& MapFactory::getMapFactory()
{
  return *map_fact_;
}

void MapFactory::setMapFactory(std::unique_ptr<MapAF> map_fact)
{
  map_fact_ = std::move(map_fact);
}

} // namespace LIB

} // namespace SBG
