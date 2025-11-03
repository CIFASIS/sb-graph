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

PWMapFact::PWMapFact() {}

////////////////////////////////////////////////////////////////////////////////
// UnordPWMap Factory ----------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

UnordPWMapFact::UnordPWMapFact() {}

PWMap UnordPWMapFact::createPWMap() const
{
  return PWMap(std::make_unique<UnordPWMap>());
}

PWMap UnordPWMapFact::createPWMap(const Set &s) const
{
  return PWMap(std::make_unique<UnordPWMap>(s));
}

PWMap UnordPWMapFact::createPWMap(const Map &m) const
{
  return PWMap(std::make_unique<UnordPWMap>(m));
}

std::string UnordPWMapFact::prettyPrint() const
{
  return "unordered";
}

////////////////////////////////////////////////////////////////////////////////
// OrdPWMap Factory ------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

OrdPWMapFact::OrdPWMapFact() {}

PWMap OrdPWMapFact::createPWMap() const
{
  return PWMap(std::make_unique<OrdPWMap>());
}

PWMap OrdPWMapFact::createPWMap(const Set &s) const
{
  return PWMap(std::make_unique<OrdPWMap>(s));
}

PWMap OrdPWMapFact::createPWMap(const Map &m) const
{
  return PWMap(std::make_unique<OrdPWMap>(m));
}

std::string OrdPWMapFact::prettyPrint() const
{
  return "ordered";
}

////////////////////////////////////////////////////////////////////////////////
// Factory for clients --------------------------------------------------------- 
////////////////////////////////////////////////////////////////////////////////

PWFactory::PWFactory() : pw_fact_(std::make_unique<UnordPWMapFact>()) {}

PWMapFact& PWFactory::pw_fact()
{
  return *pw_fact_;
}

void PWFactory::set_pw_fact(PWMapFactPtr pw_fact)
{
  pw_fact_ = std::move(pw_fact);
}

} // namespace LIB

} // namespace SBG
