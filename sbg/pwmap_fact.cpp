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

#include "sbg/pwmap_fact.hpp"
#include "sbg/ord_pwmap.hpp"
#include "sbg/unord_pwmap.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// PWMap Factory ---------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

PWMapFact::PWMapFact(const MapFact &map_fact)
  : MapFact(map_fact), map_fact_(map_fact) {}

////////////////////////////////////////////////////////////////////////////////
// UnordPWMap Factory ----------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

UnordPWMapFact::UnordPWMapFact(const MapFact &map_fact) : PWMapFact(map_fact) {}

PWMap UnordPWMapFact::createPWMap() const
{
  return PWMap(std::make_unique<UnordPWMap>(map_fact_));
}

PWMap UnordPWMapFact::createPWMap(const Set &s) const
{
  return PWMap(std::make_unique<UnordPWMap>(map_fact_, s));
}

PWMap UnordPWMapFact::createPWMap(const Map &m) const
{
  return PWMap(std::make_unique<UnordPWMap>(map_fact_, m));
}


////////////////////////////////////////////////////////////////////////////////
// OrdPWMap Factory ------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

OrdPWMapFact::OrdPWMapFact(const MapFact &map_fact) : PWMapFact(map_fact) {}

PWMap OrdPWMapFact::createPWMap() const
{
  return PWMap(std::make_unique<OrdPWMap>(map_fact_));
}

PWMap OrdPWMapFact::createPWMap(const Set &s) const
{
  return PWMap(std::make_unique<OrdPWMap>(map_fact_, s));
}

PWMap OrdPWMapFact::createPWMap(const Map &m) const
{
  return PWMap(std::make_unique<OrdPWMap>(map_fact_, m));
}


} // namespace LIB

} // namespace SBG
