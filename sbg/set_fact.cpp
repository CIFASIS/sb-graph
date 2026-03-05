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
#include "sbg/unord_set.hpp"
#include "util/debug.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Unordered Set Factory -------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

UnordSetFact::UnordSetFact() {}

Set UnordSetFact::createSet()
{
  return Set{SetKind::kUnordered};
}

Set UnordSetFact::createSet(const MD_NAT& x)
{
  return Set{SetKind::kUnordered, x};
}

Set UnordSetFact::createSet(const NAT lo, const NAT step, const NAT hi)
{
  return Set{SetKind::kUnordered, lo, step, hi};
}

Set UnordSetFact::createSet(const FixedPointsInfo& info)
{
  return Set{SetKind::kUnordered, info};
}

//////////////////////////////////////////////////////////////////////////////////
//// Ordered Set Factory ---------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////
//
//Set OrdSetFact::createSet() const
//{
//  return Set(std::make_unique<OrderedSet>());
//}
//
//Set OrdSetFact::createSet(const MD_NAT& x) const
//{
//  return Set(std::make_unique<OrderedSet>(x));
//}
//
//Set OrdSetFact::createSet(const Interval& i) const
//{
//  return Set(std::make_unique<OrderedSet>(i));
//}
//
//Set OrdSetFact::createSet(const SetPiece& mdi) const
//{
//  return Set(std::make_unique<OrderedSet>(mdi));
//}
//
//Set OrdSetFact::createSet(const FixedPointsInfo& info) const
//{
//  return Set(std::make_unique<OrderedSet>(info));
//}

////////////////////////////////////////////////////////////////////////////////
// Ordered Unidimensional Dense Set Factory ------------------------------------
////////////////////////////////////////////////////////////////////////////////

Set OrdUnidimDenseSetFact::createSet() const
{
  return Set{SetKind::kOrdUnidimDense};
}

Set OrdUnidimDenseSetFact::createSet(const MD_NAT& x) const
{
  return Set{SetKind::kOrdUnidimDense, x};
}

Set OrdUnidimDenseSetFact::createSet(const NAT lo, const NAT step, const NAT hi)
  const
{
  return Set{SetKind::kOrdUnidimDense, lo, step, hi};
}

Set OrdUnidimDenseSetFact::createSet(const FixedPointsInfo& info) const
{
  return Set{SetKind::kOrdUnidimDense, info};
}

////////////////////////////////////////////////////////////////////////////////
// Factory for clients --------------------------------------------------------- 
////////////////////////////////////////////////////////////////////////////////

SetFactory::SetFactory() : _kind(SetKind::kUnordered), _impl(UnordSetFact{}) {}

SetFactory& SetFactory::instance()
{
  static SetFactory _instance;
  return _instance;
}

const SetKind& SetFactory::kind() const { return _kind; }

void SetFactory::set_set_fact(SetKind kind)
{
  _kind = kind;
  switch (kind) {
    case SetKind::kUnordered: {
      _impl = UnordSetFact{};
      break;
    }

    //case SetKind::kOrdered: {
    //  _impl = OrdSetFact{};
    //  break;
    //}

    case SetKind::kOrdUnidimDense: {
      _impl = OrdUnidimDenseSetFact{};
      break;
    }

    default: {
      Util::ERROR("Unsupported ", kind, " Set implementation\n");
      break;
    }
  }
}

Set SetFactory::createSet() const
{
  return std::visit(
    [](const auto& a) { return a.createSet(); }
    , _impl);
}

Set SetFactory::createSet(const MD_NAT& x) const
{
  return std::visit(
    [&x](const auto& a) { return a.createSet(x); }
    , _impl);
}

Set SetFactory::createSet(const NAT lo, const NAT step, const NAT hi) const
{
  return std::visit(
    [&](const auto& a) { return a.createSet(lo, step, hi); }
    , _impl);
}

Set SetFactory::createSet(const FixedPointsInfo& info) const
{
  return std::visit(
    [&info](const auto& a) { return a.createSet(info); }
    , _impl);
}

} // namespace LIB

} // namespace SBG
