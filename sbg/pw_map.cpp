/*******************************************************************************

 This file is part of PWMap--Based Graph Library.

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

#include "sbg/pw_map.hpp"
#include "sbg/pwmap_impl.hpp"
#include "util/debug.hpp"
#include "util/defs.hpp"

#include <iostream>

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// PWMap Iterator --------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

PWMap::ConstIt::ConstIt(detail::UnordPWMap::ConstIt it) : _it(it) {}

PWMap::ConstIt::ConstIt(detail::OrdPWMap::ConstIt it) : _it(it) {}

const Map& PWMap::ConstIt::operator*()
{
  auto it_visitor = SBG::Util::Overload {
    [](const detail::UnordPWMap::ConstIt& i) -> const Map& { return *i; },
    [](const detail::OrdPWMap::ConstIt& i) -> const Map& { return i->map(); }
  };
  return std::visit(it_visitor, _it);
}

PWMap::ConstIt PWMap::ConstIt::operator++()
{
  std::visit([](auto& i) { ++i; }, _it);
  return *this;
}

bool PWMap::ConstIt::operator==(const ConstIt& other)
{
  return _it == other._it;
}

bool PWMap::ConstIt::operator!=(const ConstIt& other)
{
  return _it != other._it;
}

////////////////////////////////////////////////////////////////////////////////
// PWMap  ----------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

// Constructors ----------------------------------------------------------------

PWMap::PWMap() : _impl()
{
  PWMapKind kind = PWMAP_IMPL.kind();
  switch (kind) {
    case PWMapKind::kUnordered: {
      _impl = detail::UnordPWMap{};
      break;
    }

    case PWMapKind::kOrdered: {
      _impl = detail::OrdPWMap{};
      break;
    }

    case PWMapKind::kDomOrdered: {
      _impl = detail::DomOrdPWMap{};
      break;
    }

    default: {
      Util::ERROR("Unsupported ", kind, " PWMap implementation\n");
      break;
    }
  }
}

PWMap::PWMap(Set s) : _impl()
{
  PWMapKind kind = PWMAP_IMPL.kind();
  switch (kind) {
    case PWMapKind::kUnordered: {
      _impl = detail::UnordPWMap{s};
      break;
    }

    case PWMapKind::kOrdered: {
      _impl = detail::OrdPWMap{s};
      break;
    }

    case PWMapKind::kDomOrdered: {
      _impl = detail::DomOrdPWMap{s};
      break;
    }

    default: {
      Util::ERROR("Unsupported ", kind, " PWMap implementation\n");
      break;
    }
  }
}

PWMap::PWMap(Map m) : _impl()
{ 
  PWMapKind kind = PWMAP_IMPL.kind();
  switch (kind) {
    case PWMapKind::kUnordered: {
      _impl = detail::UnordPWMap{m};
      break;
    }

    case PWMapKind::kOrdered: {
      _impl = detail::OrdPWMap{m};
      break;
    }

    case PWMapKind::kDomOrdered: {
      _impl = detail::DomOrdPWMap{m};
      break;
    }

    default: {
      Util::ERROR("Unsupported ", kind, " PWMap implementation\n");
      break;
    }
  }
}

PWMap::PWMap(const detail::PWMapImpl& impl) : _impl(impl) {}

PWMap::PWMap(detail::PWMapImpl&& impl) : _impl(std::move(impl)) {}

// Getters ---------------------------------------------------------------------

PWMap::ConstIt PWMap::begin()
{
  return std::visit([](const auto& a) { return PWMap::ConstIt(a.begin()); }
    , _impl);
}

PWMap::ConstIt PWMap::end()
{
  return std::visit([](const auto& a) { return PWMap::ConstIt(a.end()); }
    , _impl);
}

// Setters ---------------------------------------------------------------------

void PWMap::insert(const Map& m)
{
  std::visit([&m](auto& a) -> void { a.insert(m); } , _impl);
}

void PWMap::insert(Map&& m)
{
  std::visit([move_m = std::move(m)](auto& a) { a.insert(move_m); }
    , _impl);
}

// Operators -------------------------------------------------------------------

bool PWMap::operator==(const PWMap& other) const
{
  return _impl == other._impl;
}

bool PWMap::operator!=(const PWMap& other) const
{
  return !(*this == other);
}

PWMap PWMap::operator+(const PWMap& other) const
{
  return std::visit([](const auto& a, const auto& b)
    {
      using A = std::decay_t<decltype(a)>;
      using B = std::decay_t<decltype(b)>;
      if constexpr (std::is_same_v<A, B>) {
        return PWMap{a + b};
      } else {
        Util::ERROR("PWMap::operator+: mismatched implementations\n");
        return PWMap{};
      }
    }
    , _impl, other._impl);
}

std::ostream& PWMap::print(std::ostream& out) const
{
  std::visit([&out](const auto& a) -> void { a.print(out); } , _impl);
  return out;
}

std::ostream& operator<<(std::ostream& out, const PWMap& s)
{
  s.print(out);
  return out;
}

// PWMap operations --------------------------------------------------------------

std::size_t PWMap::arity() const
{
  return std::visit([](const auto& a) { return a.arity(); }, _impl);
}

bool PWMap::isEmpty() const
{
  return std::visit([](const auto& a) { return a.isEmpty(); }, _impl);
}

Set PWMap::domain() const &
{
  return std::visit([](const auto& a) { return a.domain(); }, _impl);
}

Set PWMap::domain() &&
{
  return std::visit([](auto&& a) { return std::move(a).domain(); }, _impl);
}

PWMap PWMap::restrict(const Set& subdom) const
{
  return std::visit([&](const auto& a) -> PWMap
    {
      return PWMap{a.restrict(subdom)};
    }
    , _impl);
}


Set PWMap::image() const
{
  return std::visit([](const auto& a) { return a.image(); }, _impl);
}

Set PWMap::image(const Set& subdom) const
{
  return std::visit([&subdom](const auto& a) { return a.image(subdom); }
    , _impl);
}

Set PWMap::preImage(const Set& subcodom) const
{
  return std::visit([&subcodom](const auto& a) { return a.preImage(subcodom); }
    , _impl);
}

PWMap PWMap::inverse() const
{
  return std::visit([](const auto& a) { return PWMap{a.inverse()}; }
    , _impl);
}

PWMap PWMap::composition(const PWMap& other) const
{
  return std::visit([](const auto& a, const auto& b)
    {
      using A = std::decay_t<decltype(a)>;
      using B = std::decay_t<decltype(b)>;
      if constexpr (std::is_same_v<A, B>) {
        return PWMap{a.composition(b)};
      } else {
        Util::ERROR("PWMap::composition: mismatched implementations\n");
        return PWMap{};
      }
    }
    , _impl, other._impl);
}

PWMap PWMap::mapInf() const
{
  return std::visit([](const auto& a) { return PWMap{a.mapInf()}; }
    , _impl);
}

Set PWMap::fixedPoints() const
{
  return std::visit([](const auto& a) { return a.fixedPoints(); }, _impl);
}

// Extra operations ------------------------------------------------------------

PWMap PWMap::concatenation(const PWMap& other) const &
{
  return PWMap{*this}.concatenation(other);
}

PWMap PWMap::concatenation(const PWMap& other) &&
{
  return std::move(*this).concatenation(PWMap{other});
}

PWMap PWMap::concatenation(PWMap&& other) const &
{
  return PWMap{*this}.concatenation(std::move(other));
}

PWMap PWMap::concatenation(PWMap&& other) &&
{
  return std::visit([](auto&& a, auto&& b)
    {
      using A = std::decay_t<decltype(a)>;
      using B = std::decay_t<decltype(b)>;
      if constexpr (std::is_same_v<A, B>) {
        return PWMap{std::move(a).concatenation(std::move(b))};
      } else {
        Util::ERROR("PWMap::concatenation: mismatched implementations\n");
        return PWMap{};
      }
    }
    , std::move(_impl), std::move(other._impl));
}

PWMap PWMap::combine(const PWMap& other) const &
{
  return PWMap{*this}.combine(other);
}

PWMap PWMap::combine(const PWMap& other) &&
{
  return std::move(*this).combine(PWMap{other});
}

PWMap PWMap::combine(PWMap&& other) const &
{
  return PWMap{*this}.combine(std::move(other));
}

PWMap PWMap::combine(PWMap&& other) &&
{
  return std::visit([](auto&& a, auto&& b)
    {
      using A = std::decay_t<decltype(a)>;
      using B = std::decay_t<decltype(b)>;
      if constexpr (std::is_same_v<A, B>) {
        return PWMap{std::move(a).combine(std::move(b))};
      } else {
        Util::ERROR("PWMap::combine: mismatched implementations\n");
        return PWMap{};
      }
    }
    , std::move(_impl), std::move(other._impl));
}

PWMap PWMap::min(const PWMap& other) const
{
  return std::visit([](const auto& a, const auto& b)
    {
      using A = std::decay_t<decltype(a)>;
      using B = std::decay_t<decltype(b)>;
      if constexpr (std::is_same_v<A, B>) {
        return PWMap{a.min(b)};
      } else {
        Util::ERROR("PWMap::min: mismatched implementations\n");
        return PWMap{};
      }
    }
    , _impl, other._impl);
}

PWMap PWMap::minAdj(const PWMap& other) const
{
  return std::visit([](const auto& a, const auto& b)
    {
      using A = std::decay_t<decltype(a)>;
      using B = std::decay_t<decltype(b)>;
      if constexpr (std::is_same_v<A, B>) {
        return PWMap{a.minAdj(b)};
      } else {
        Util::ERROR("PWMap::minAdj: mismatched implementations\n");
        return PWMap{};
      }
    }
    , _impl, other._impl);
}

Set PWMap::sharedImage() const
{
  return std::visit([](const auto& a) { return a.sharedImage(); }, _impl);
}

Set PWMap::equalImage(const PWMap& other) const
{
  return std::visit([](const auto& a, const auto& b)
    {
      using A = std::decay_t<decltype(a)>;
      using B = std::decay_t<decltype(b)>;
      if constexpr (std::is_same_v<A, B>) {
        return a.equalImage(b);
      } else {
        Util::ERROR("PWMap::equalImage: mismatched implementations\n");
        return Set{};
      }
    }
    , _impl, other._impl);
}

Set PWMap::lessImage(const PWMap& other) const
{
  return std::visit([](const auto& a, const auto& b)
    {
      using A = std::decay_t<decltype(a)>;
      using B = std::decay_t<decltype(b)>;
      if constexpr (std::is_same_v<A, B>) {
        return a.lessImage(b);
      } else {
        Util::ERROR("PWMap::lessImage: mismatched implementations\n");
        return Set{};
      }
    }
    , _impl, other._impl);
}

PWMap PWMap::imageMultiplicity() const
{
  return std::visit([](const auto& a) { return PWMap{a.imageMultiplicity()}; }
    , _impl);
}

void PWMap::compact()
{
  std::visit([](auto& a) { a.compact(); }, _impl);
}

rapidjson::Value PWMap::toJSON(rapidjson::Document::AllocatorType& alloc) const
{
  return std::visit([&alloc](auto a) { return detail::toJSON(a, alloc); }
    , _impl);
}

}  // namespace LIB

}  // namespace SBG
