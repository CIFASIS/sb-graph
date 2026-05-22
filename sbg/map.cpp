/*******************************************************************************

 This file is part of Set--Based Graph Library.

 SBG Library is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 SBG Library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the delegied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with SBG Library.  If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#include "sbg/map.hpp"
#include "sbg/map_detail.hpp"
#include "sbg/set_fact.hpp"

#include <iostream>

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Map Implementation ----------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

// Constructors/Destructors ----------------------------------------------------

Map::Map() : _domain(SET_FACT.createSet()) {}

Map::Map(const MD_NAT& x, const Expression& expr)
  : _domain(SET_FACT.createSet(x)), _law(expr) {}

Map::Map(const Set& s, const Expression& expr) : _domain(s), _law(expr) {}

Map::Map(Set&& s, Expression&& expr)
  : _domain(std::move(s)), _law(std::move(expr)) {}

// Getters ---------------------------------------------------------------------

const Set& Map::domain() const & { return _domain; }

Set Map::domain() && { return std::move(_domain); }

const Expression& Map::law() const { return _law; }

// Operators -------------------------------------------------------------------

bool Map::operator==(const Map& other) const
{
  if (_domain == other._domain) {
    if (_domain.cardinal() == 1) {
      return image() == other.image();
    } else { 
      return _law == other._law;
    }
  }

  return false;
}

bool Map::operator!=(const Map& other) const
{
  return !(*this == other);
}

Map Map::operator+(const Map& other) const
{
  Set result_domain = _domain.intersection(other._domain);
  Expression result_law = _law + other._law;

  return Map{std::move(result_domain), std::move(result_law)};
}

std::ostream& operator<<(std::ostream& out, const Map& m)
{
  out << m.domain() << " -> " << m.law();
  return out;
}

// Map operations --------------------------------------------------------------

std::size_t Map::arity() const { return _law.arity(); }

bool Map::isEmpty() const { return _domain.isEmpty(); }

Map Map::restrict(const Set& subdom) const &
{
  return Map{_domain.intersection(subdom), _law};
}

Map Map::restrict(const Set& subdom) &&
{
  return Map{std::move(_domain).intersection(subdom), _law};
}

Map Map::restrict(Set&& subdom) const &
{
  return Map{_domain.intersection(std::move(subdom)), _law};
}

Set Map::image() const { return image(_domain); }

Set Map::image(const Set& subdom) const
{
  if (isEmpty() || subdom.isEmpty()) {
    return SET_FACT.createSet();
  }

  Set domain_subdom = _domain.intersection(subdom);
  if (domain_subdom.isEmpty()) {
    return SET_FACT.createSet();
  }

  return detail::MapDetail::image(domain_subdom, _law);
}

Set Map::preImage(const Set& subcodom) const
{
  Set image_subcodom = image().intersection(subcodom);
  return detail::MapDetail::preImage(image_subcodom, _law)
    .intersection(_domain);
}

Map Map::composition(const Map& other) const
{
  Set result_domain = _domain.intersection(other.image());
  if (!result_domain.isEmpty()) {
    result_domain = other.preImage(result_domain);
  }
  Expression result_law = _law.composition(other._law);

  return Map{std::move(result_domain), std::move(result_law)};
}

Set Map::fixedPoints() const
{
  FixedPointsInfo fixed_points_info = _law.fixedPoints();
  Set universal_fixed = SET_FACT.createSet(fixed_points_info);
  return _domain.intersection(std::move(universal_fixed));
}

// Extra operations ------------------------------------------------------------

bool Map::isId() const
{
  if (_domain.cardinal() == 1) {
    return _domain == image();
  }

  return _law.isId();
}

Set Map::lessImage(const Map& other) const
{
  Set cap_dom = _domain.intersection(other._domain);
  if (cap_dom.isEmpty()) {
    return cap_dom;
  }

  Set result = detail::MapDetail::lessImage(_law, other._law);
  return result.intersection(cap_dom);
}

Map Map::minAdj(const Map& other) const
{
  Set this_other_dom = _domain.intersection(other._domain);
  if (this_other_dom.isEmpty()) {
    return Map{};
  }

  Set result_domain = image(this_other_dom);
  Expression result_expr;
  if (_law.isInjective()) {
    result_expr = other._law.composition(_law.inverse());
  } else {
    Set image2 = other.image(this_other_dom);
    result_expr = Expression{image2.minElem()};
  }

  return Map{result_domain, result_expr};
}

std::vector<Map> Map::reduce() const
{
  return detail::MapDetail::reduce(*this);
}

std::vector<Map> Map::imageMultiplicity() const
{
  return detail::MapDetail::imageMultiplicity(*this);
}

MaybeMap Map::compact(const Map& other) const
{
  Set result_domain = SET_FACT.createSet();
  if (_law == other.law()) {
    Set result_dom = _domain.disjointCup(other.domain());
    result_dom.compact();
    return Map{std::move(result_dom), Expression{_law}};
  }

  return {};
}

// Non-member functions --------------------------------------------------------

rapidjson::Value toJSON(Map m, rapidjson::Document::AllocatorType& alloc)
{
  rapidjson::Value result{rapidjson::kObjectType};

  result.AddMember("domain", m.domain().toJSON(alloc), alloc);
  result.AddMember("law", m.law().toJSON(alloc), alloc);

  return result;
}

} // namespace LIB

} // namespace SBG
