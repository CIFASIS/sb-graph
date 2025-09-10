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

#include "sbg/map_fact.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Map Factory -----------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

MapFact::MapFact(const SetFact &set_fact) : set_fact_(set_fact) {}

Set MapFact::createSet() const { return set_fact_.createSet(); }

Set MapFact::createSet(const MD_NAT &x) const
{
  return set_fact_.createSet(x);
}

Set MapFact::createSet(const Interval &i) const
{
  return set_fact_.createSet(i);
}

Set MapFact::createSet(const SetPiece &mdi) const
{
  return set_fact_.createSet(mdi);
}

Map MapFact::createMap() const
{
  return Map(set_fact_);
}

Map MapFact::createMap(MD_NAT x, Exp exp) const
{
  return Map(set_fact_, x, exp);
}

Map MapFact::createMap(Interval i, LExp le) const
{
  return Map(set_fact_, i, le);
}

Map MapFact::createMap(SetPiece mdi, Exp exp) const
{
  return Map(set_fact_, mdi, exp);
}

Map MapFact::createMap(Set s, Exp exp) const
{
  return Map(set_fact_, s, exp);
}

} // namespace LIB

} // namespace SBG
