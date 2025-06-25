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
// PWMap AF --------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

PWMapAF::PWMapAF(const MapAF &map_fact)
  : MapAF(map_fact), map_fact_(map_fact) {}

////////////////////////////////////////////////////////////////////////////////
// UnordPWMap AF ---------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

UnordPWMapAF::UnordPWMapAF(const MapAF &map_fact) : PWMapAF(map_fact) {}

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


////////////////////////////////////////////////////////////////////////////////
// OrdPWMap AF -----------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

OrdPWMapAF::OrdPWMapAF(const MapAF &map_fact) : PWMapAF(map_fact) {}

PWMap OrdPWMapAF::createPWMap() const
{
  return PWMap(std::make_unique<OrdPWMap>(map_fact_));
}

PWMap OrdPWMapAF::createPWMap(const Set &s) const
{
  return PWMap(std::make_unique<OrdPWMap>(map_fact_, s));
}

PWMap OrdPWMapAF::createPWMap(const Map &m) const
{
  return PWMap(std::make_unique<OrdPWMap>(map_fact_, m));
}


} // namespace LIB

} // namespace SBG
