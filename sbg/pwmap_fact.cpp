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
#include "util/debug.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// UnordPWMap Factory ----------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

UnordPWMapFact::UnordPWMapFact() {}

PWMap UnordPWMapFact::createPWMap() const
{
  return PWMap{PWMapKind::kUnordered};
}

PWMap UnordPWMapFact::createPWMap(const Set& s) const
{
  return PWMap{PWMapKind::kUnordered, s};
}

PWMap UnordPWMapFact::createPWMap(const Map& m) const
{
  return PWMap{PWMapKind::kUnordered, m};
}

////////////////////////////////////////////////////////////////////////////////
// OrdPWMap Factory ------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

OrdPWMapFact::OrdPWMapFact() {}

PWMap OrdPWMapFact::createPWMap() const
{
  return PWMap{PWMapKind::kOrdered};
}

PWMap OrdPWMapFact::createPWMap(const Set& s) const
{
  return PWMap{PWMapKind::kOrdered, s};
}

PWMap OrdPWMapFact::createPWMap(const Map& m) const
{
  return PWMap{PWMapKind::kOrdered, m};
}

//////////////////////////////////////////////////////////////////////////////////
//// DomOrdPWMap Factory ---------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////
//
//DomOrdPWMapFact::DomOrdPWMapFact() {}
//
//PWMap DomOrdPWMapFact::createPWMap() const
//{
//  return PWMap(std::make_unique<DomOrdPWMap>());
//}
//
//PWMap DomOrdPWMapFact::createPWMap(const Set &s) const
//{
//  return PWMap(std::make_unique<DomOrdPWMap>(s));
//}
//
//PWMap DomOrdPWMapFact::createPWMap(const Map &m) const
//{
//  return PWMap(std::make_unique<DomOrdPWMap>(m));
//}
//
////////////////////////////////////////////////////////////////////////////////
// Factory for clients --------------------------------------------------------- 
////////////////////////////////////////////////////////////////////////////////

PWMapFactory::PWMapFactory() : _kind(PWMapKind::kUnordered)
  , _impl(UnordPWMapFact{}) {}

PWMapFactory& PWMapFactory::instance()
{
  static PWMapFactory _instance;
  return _instance;
}

const PWMapKind& PWMapFactory::kind() const { return _kind; }

void PWMapFactory::set_pwmap_fact(PWMapKind kind)
{
  _kind = kind;
  switch (kind) {
    case PWMapKind::kUnordered: {
      _impl = UnordPWMapFact{};
      break;
    }

    case PWMapKind::kOrdered: {
      _impl = OrdPWMapFact{};
      break;
    }

    //case PWMapKind::kDomOrdered: {
    //  _impl = DomOrdPWMapFact{};
    //}

    default: {
      Util::ERROR("Unsupported ", kind, " PWMap implementation\n");
      break;
    }
  }
}

PWMap PWMapFactory::createPWMap() const
{
  return std::visit([](const auto& a) { return a.createPWMap(); }, _impl);
}

PWMap PWMapFactory::createPWMap(Set s) const
{
  return std::visit([&s](const auto& a) { return a.createPWMap(std::move(s)); }
    , _impl);
}

PWMap PWMapFactory::createPWMap(Map m) const
{
  return std::visit([&m](const auto& a) { return a.createPWMap(std::move(m)); }
    , _impl);
}

} // namespace LIB

} // namespace SBG
