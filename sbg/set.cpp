/*******************************************************************************

 This file is part of Set--Based Graph Library.

 SBG Library is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 SBG Library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the stratied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with SBG Library.  If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#include "sbg/set.hpp"
#include "sbg/set_impl.hpp"
#include "util/debug.hpp"

#include <iostream>

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Set  ------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

// Constructors ----------------------------------------------------------------

Set::Set() : _impl()
{
  SetKind kind = SET_IMPL.kind();
  switch (kind) {
    case SetKind::kUnordered: {
      _impl = detail::UnorderedSet{};
      break;
    }

    case SetKind::kOrdered: {
      _impl = detail::OrderedSet{};
      break;
    }

    case SetKind::kOrdUnidimDense: {
      _impl = detail::OrdUnidimDenseSet{};
      break;
    }

    default: {
      Util::ERROR("Unsupported ", kind, " Set implementation\n");
      break;
    }
  }
}

Set::Set(const MD_NAT& x) : _impl()
{
  SetKind kind = SET_IMPL.kind();
  switch (kind) {
    case SetKind::kUnordered: {
      _impl = detail::UnorderedSet{x};
      break;
    }

    case SetKind::kOrdered: {
      _impl = detail::OrderedSet{x};
      break;
    }

    case SetKind::kOrdUnidimDense: {
      _impl = detail::OrdUnidimDenseSet{x[0]};
      break;
    }

    default: {
      Util::ERROR("Unsupported ", kind, " Set implementation\n");
      break;
    }
  }
}

Set::Set(MD_NAT&& x) : _impl()
{
  SetKind kind = SET_IMPL.kind();
  switch (kind) {
    case SetKind::kUnordered: {
      _impl = detail::UnorderedSet{x};
      break;
    }

    case SetKind::kOrdered: {
      _impl = detail::OrderedSet{x};
      break;
    }

    case SetKind::kOrdUnidimDense: {
      _impl = detail::OrdUnidimDenseSet{x[0]};
      break;
    }

    default: {
      Util::ERROR("Unsupported ", kind, " Set implementation\n");
      break;
    }
  }
}

Set::Set(const NAT lo, const NAT step, const NAT hi) : _impl()
{
  SetKind kind = SET_IMPL.kind();
  switch (kind) {
    case SetKind::kUnordered: {
      _impl = detail::UnorderedSet{detail::Interval(lo, step, hi)};
      break;
    }

    case SetKind::kOrdered: {
      _impl = detail::OrderedSet{detail::Interval(lo, step, hi)};
      break;
    }

    case SetKind::kOrdUnidimDense: {
      _impl = detail::OrdUnidimDenseSet{detail::Interval(lo, step, hi)};
      break;
    }

    default: {
      Util::ERROR("Unsupported ", kind, " Set implementation\n");
      break;
    }
  }
}

Set::Set(const FixedPointsInfo& info) : _impl()
{
  SetKind kind = SET_IMPL.kind();
  switch (kind) {
    case SetKind::kUnordered: {
      _impl = detail::UnorderedSet{info};
      break;
    }

    case SetKind::kOrdered: {
      _impl = detail::OrderedSet{info};
      break;
    }

    case SetKind::kOrdUnidimDense: {
      _impl = detail::OrdUnidimDenseSet{info};
      break;
    }

    default: {
      Util::ERROR("Unsupported ", kind, " Set implementation\n");
      break;
    }
  }
}

Set::Set(const detail::SetImpl& impl) : _impl(impl) {}

Set::Set(detail::SetImpl&& impl) : _impl(std::move(impl)) {}

// Operators -------------------------------------------------------------------

bool Set::operator==(const Set& other) const
{
  return _impl == other._impl;
}

bool Set::operator!=(const Set& other) const
{
  return !(*this == other);
}

std::ostream& Set::print(std::ostream& out) const
{
  std::visit([&out](const auto& a) -> void { a.print(out); } , _impl);
  return out;
}

std::ostream& operator<<(std::ostream& out, const Set& s)
{
  s.print(out);
  return out;
}

// Set operations --------------------------------------------------------------

unsigned int Set::cardinal() const
{
  return std::visit([](const auto& a) { return a.cardinal(); }, _impl);
}

bool Set::isEmpty() const
{
  return std::visit([](const auto& a) { return a.isEmpty(); }, _impl);
}

MD_NAT Set::minElem() const
{
  return std::visit([](const auto& a) { return a.minElem(); }, _impl);
}

MD_NAT Set::maxElem() const
{
  return std::visit([](const auto& a) { return a.maxElem(); }, _impl);
}

Set Set::intersection(const Set& other) const &
{
  return std::visit([](const auto& a, const auto& b)
    {
      using A = std::decay_t<decltype(a)>;
      using B = std::decay_t<decltype(b)>;
      if constexpr (std::is_same_v<A, B>) {
        return Set{a.intersection(b)};
      } else {
        Util::ERROR("Set::intersection: mismatched implementations\n");
        return Set{};
      }
    }
    , _impl, other._impl);
}

Set Set::cup(const Set& other) const &
{
  return Set{*this}.cup(other);
}

Set Set::cup(const Set& other) &&
{
  return std::move(*this).cup(Set{other});
}

Set Set::cup(Set&& other) const &
{
  return Set{*this}.cup(std::move(other));
}

Set Set::cup(Set&& other) &&
{
  return std::visit([](auto&& a, auto&& b)
    {
      using A = std::decay_t<decltype(a)>;
      using B = std::decay_t<decltype(b)>;
      if constexpr (std::is_same_v<A, B>) {
        return Set{std::move(a).cup(std::move(b))};
      } else {
        Util::ERROR("Set::cup: mismatched implementations\n");
        return Set{};
      }
    }
    , std::move(_impl), std::move(other._impl));
}

Set Set::complement() const
{
  return std::visit(
    [](const auto& a) -> Set { return Set{a.complement()}; }
    , _impl);
}

Set Set::difference(const Set& other) const
{
  return std::visit([](const auto& a, const auto& b)
    {
      using A = std::decay_t<decltype(a)>;
      using B = std::decay_t<decltype(b)>;
      if constexpr (std::is_same_v<A, B>) {
        return Set{a.difference(b)};
      } else {
        Util::ERROR("Set::difference: mismatched implementations\n");
        return Set{};
      }
    }
    , _impl, other._impl);
}

Set Set::cartesianProduct(const Set& other) const
{
  return std::visit([](const auto& a, const auto& b)
    {
      using A = std::decay_t<decltype(a)>;
      using B = std::decay_t<decltype(b)>;
      if constexpr (std::is_same_v<A, B>) {
        return Set{a.cartesianProduct(b)};
      } else {
        Util::ERROR("Set::cartesianProduct: mismatched implementations\n");
        return Set{};
      }
    }
    , _impl, other._impl);
}

// Extra operations ------------------------------------------------------------

std::size_t Set::arity() const
{
  return std::visit([](const auto& a) { return a.arity(); }, _impl);
}

Set Set::disjointCup(const Set& other) const &
{
  return Set{*this}.disjointCup(other);
}

Set Set::disjointCup(const Set& other) &&
{
  return std::move(*this).disjointCup(Set{other});
}

Set Set::disjointCup(Set&& other) const &
{
  return Set{*this}.disjointCup(std::move(other));
}

Set Set::disjointCup(Set&& other) &&
{
  return std::visit([](auto&& a, auto&& b)
    {
      using A = std::decay_t<decltype(a)>;
      using B = std::decay_t<decltype(b)>;
      if constexpr (std::is_same_v<A, B>) {
        return Set{std::move(a).disjointCup(std::move(b))};
      } else {
        Util::ERROR("Set::disjointCup: mismatched implementations\n");
        return Set{};
      }
    }
    , std::move(_impl), std::move(other._impl));
}

Set Set::offset(const MD_NAT& off) const
{
  return std::visit(
    [&off](const auto& a) -> Set { return Set{a.offset(off)}; }
    , _impl);
}

Perimeter Set::perimeter() const
{
  return std::visit([](const auto& a) { return a.perimeter(); }, _impl);
}

void Set::compact() { std::visit([](auto& a) { a.compact(); }, _impl); }

rapidjson::Value Set::toJSON(rapidjson::Document::AllocatorType& alloc) const
{
  return std::visit([&alloc](auto a) { return detail::toJSON(a, alloc); }
    , _impl);
}

}  // namespace LIB

}  // namespace SBG
