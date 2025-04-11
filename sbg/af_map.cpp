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

MapAF::MapAF(const SetAF &set_fact) : set_fact_(set_fact) {}

Set MapAF::createSet() const { return set_fact_.createSet(); }

Set MapAF::createSet(const MD_NAT &x) const
{
  return set_fact_.createSet(x);
}

Set MapAF::createSet(const Interval &i) const
{
  return set_fact_.createSet(i);
}

Set MapAF::createSet(const SetPiece &mdi) const
{
  return set_fact_.createSet(mdi);
}

Map MapAF::createMap() const
{
  return Map(set_fact_);
}

Map MapAF::createMap(MD_NAT x, Exp exp) const
{
  return Map(set_fact_, x, exp);
}

Map MapAF::createMap(Interval i, LExp le) const
{
  return Map(set_fact_, i, le);
}

Map MapAF::createMap(SetPiece mdi, Exp exp) const
{
  return Map(set_fact_, mdi, exp);
}

Map MapAF::createMap(Set s, Exp exp) const
{
  return Map(set_fact_, s, exp);
}

} // namespace LIB

} // namespace SBG
