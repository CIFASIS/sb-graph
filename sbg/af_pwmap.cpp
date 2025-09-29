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

PWMapAF::PWMapAF() {}

////////////////////////////////////////////////////////////////////////////////
// UnordPWMap AF ---------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

UnordPWMapAF::UnordPWMapAF() {}

PWMap UnordPWMapAF::createPWMap() const
{
  return PWMap(std::make_unique<UnordPWMap>());
}

PWMap UnordPWMapAF::createPWMap(const Set &s) const
{
  return PWMap(std::make_unique<UnordPWMap>(s));
}

PWMap UnordPWMapAF::createPWMap(const Map &m) const
{
  return PWMap(std::make_unique<UnordPWMap>(m));
}

////////////////////////////////////////////////////////////////////////////////
// Factory for clients --------------------------------------------------------- 
////////////////////////////////////////////////////////////////////////////////

PWFactory::PWFactory() : pw_fact_(std::make_unique<UnordPWMapAF>()) {}

PWMapAF& PWFactory::pw_fact()
{
  return *pw_fact_;
}

void PWFactory::set_pw_fact(std::unique_ptr<PWMapAF> pw_fact)
{
  pw_fact_ = std::move(pw_fact);
}

} // namespace LIB

} // namespace SBG
