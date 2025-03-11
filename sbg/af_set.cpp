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

#include "sbg/af_set.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Unordered Set AF ------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

Set UnordAF::createSet() const
{
  return Set(std::make_unique<UnorderedSet>());
}

Set UnordAF::createSet(const MD_NAT &x) const
{
  return Set(std::make_unique<UnorderedSet>(x));
}

Set UnordAF::createSet(const Interval &i) const
{
  return Set(std::make_unique<UnorderedSet>(i));
}

Set UnordAF::createSet(const SetPiece &mdi) const
{
  return Set(std::make_unique<UnorderedSet>(mdi));
}

////////////////////////////////////////////////////////////////////////////////
// Ordered Set (1 dimension, dense intervals) AF -------------------------------
////////////////////////////////////////////////////////////////////////////////

Set OrdDenseAF::createSet() const
{
  return Set(std::make_unique<OrderedDenseSet>());
}

Set OrdDenseAF::createSet(const MD_NAT &x) const
{
  return Set(std::make_unique<OrderedDenseSet>(x));
}

Set OrdDenseAF::createSet(const Interval &i) const
{
  return Set(std::make_unique<OrderedDenseSet>(i));
}

Set OrdDenseAF::createSet(const SetPiece &mdi) const
{
  return Set(std::make_unique<OrderedDenseSet>(mdi));
}

} // namespace LIB

} // namespace SBG
