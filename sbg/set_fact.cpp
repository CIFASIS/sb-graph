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

#include "sbg/set_fact.hpp"
#include "sbg/ord_set.hpp"
#include "sbg/ord_unidim_dense_set.hpp"
#include "sbg/unord_set.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Unordered Set Factory -------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

Set UnordSetFact::createSet() const
{
  return Set(std::make_unique<UnorderedSet>());
}

Set UnordSetFact::createSet(const MD_NAT &x) const
{
  return Set(std::make_unique<UnorderedSet>(x));
}

Set UnordSetFact::createSet(const Interval &i) const
{
  return Set(std::make_unique<UnorderedSet>(i));
}

Set UnordSetFact::createSet(const SetPiece &mdi) const
{
  return Set(std::make_unique<UnorderedSet>(mdi));
}

////////////////////////////////////////////////////////////////////////////////
// Ordered Unidimensional Dense Set Factory ------------------------------------
////////////////////////////////////////////////////////////////////////////////

Set OrdUnidimDenseSetFact::createSet() const
{
  return Set(std::make_unique<OrdUnidimDenseSet>());
}

Set OrdUnidimDenseSetFact::createSet(const MD_NAT &x) const
{
  return Set(std::make_unique<OrdUnidimDenseSet>(x));
}

Set OrdUnidimDenseSetFact::createSet(const Interval &i) const
{
  return Set(std::make_unique<OrdUnidimDenseSet>(i));
}

Set OrdUnidimDenseSetFact::createSet(const SetPiece &mdi) const
{
  return Set(std::make_unique<OrdUnidimDenseSet>(mdi));
}

////////////////////////////////////////////////////////////////////////////////
// Ordered Set Factory ---------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

Set OrdSetFact::createSet() const
{
  return Set(std::make_unique<OrderedSet>());
}

Set OrdSetFact::createSet(const MD_NAT &x) const
{
  return Set(std::make_unique<OrderedSet>(x));
}

Set OrdSetFact::createSet(const Interval &i) const
{
  return Set(std::make_unique<OrderedSet>(i));
}

Set OrdSetFact::createSet(const SetPiece &mdi) const
{
  return Set(std::make_unique<OrderedSet>(mdi));
}

} // namespace LIB

} // namespace SBG
