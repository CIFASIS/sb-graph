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

#include "sbg/af_pwmap.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// UnordPWMap AF ---------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

UnordPWMapAF::UnordPWMapAF(const MapAF &map_fact) : map_fact_(map_fact) {}

Set UnordPWMapAF::createSet() const { return map_fact_.createSet(); }

Set UnordPWMapAF::createSet(const MD_NAT &x) const
{
  return map_fact_.createSet(x);
}

Set UnordPWMapAF::createSet(const Interval &i) const
{
  return map_fact_.createSet(i);
}

Set UnordPWMapAF::createSet(const SetPiece &mdi) const
{
  return map_fact_.createSet(mdi);
}

Map UnordPWMapAF::createMap() const
{
  return map_fact_.createMap();
}

Map UnordPWMapAF::createMap(MD_NAT x, Exp exp) const
{
  return map_fact_.createMap(x, exp);
}

Map UnordPWMapAF::createMap(Interval i, LExp le) const
{
  return map_fact_.createMap(i, le);
}

Map UnordPWMapAF::createMap(SetPiece mdi, Exp exp) const
{
  return map_fact_.createMap(mdi, exp);
}

Map UnordPWMapAF::createMap(Set s, Exp exp) const
{
  return map_fact_.createMap(s, exp);
}

PWMap UnordPWMapAF::createPWMap() const
{
  return PWMap(std::make_unique<UnordPWMap>(map_fact_));
}

PWMap UnordPWMapAF::createPWMap(const Set &s) const
{
  return PWMap(std::make_unique<UnordPWMap>(map_fact_, s));
}

PWMap UnordPWMapAF::createPWMap(const Map &m) const
{
  return PWMap(std::make_unique<UnordPWMap>(map_fact_, m));
}

} // namespace LIB

} // namespace SBG
