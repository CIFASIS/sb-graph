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

MapSetAF::MapSetAF(const SetAF &set_fact) : set_fact_(set_fact) {}

Map MapSetAF::createMap() const
{
  return Map(std::make_unique<MapSetDeleg>(set_fact_));
}

Map MapSetAF::createMap(Util::MD_NAT x, Exp exp) const
{
  return Map(std::make_unique<MapSetDeleg>(set_fact_, x, exp));
}

Map MapSetAF::createMap(Interval i, LExp le) const
{
  return Map(std::make_unique<MapSetDeleg>(set_fact_, i, le));
}

Map MapSetAF::createMap(SetPiece mdi, Exp exp) const
{
  return Map(std::make_unique<MapSetDeleg>(set_fact_, mdi, exp));
}

Map MapSetAF::createMap(Set s, Exp exp) const
{
  return Map(std::make_unique<MapSetDeleg>(set_fact_, s, exp));
}

} // namespace LIB

} // namespace SBG
