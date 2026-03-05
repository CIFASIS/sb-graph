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
#include "sbg/unord_pwmap.hpp"

#include <forward_list>
#include <set>
#include <iostream>

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Unordered PWMap Implementation ----------------------------------------------
////////////////////////////////////////////////////////////////////////////////

// Auxliary definitions --------------------------------------------------------

class MapLess {
public:
  bool operator()(const Map& a, const Map& b) const {
    return a.domain().minElem() < b.domain().minElem();
  }
};

// Constructors/Destructors ----------------------------------------------------

UnordPWMap::UnordPWMap() {}

UnordPWMap::UnordPWMap(const Set& s) : _pieces() {
  if (!s.isEmpty()) {
    _pieces.emplace_back(s, Expression{s.arity(), 1, 0});
  }
}

UnordPWMap::UnordPWMap(const Map& m) : _pieces() {
  if (!m.domain().isEmpty()) {
    _pieces.push_back(m);
  }
}

UnordPWMap::UnordPWMap(UnordPWMap::UnordMapCollection pieces)
  : _pieces(std::move(pieces)) {}

// Getters ---------------------------------------------------------------------

UnordPWMap::ConstIt UnordPWMap::begin() const { return _pieces.begin(); }

UnordPWMap::ConstIt UnordPWMap::end() const { return _pieces.end(); }

// Setters ---------------------------------------------------------------------

void UnordPWMap::pushBack(const Map& m)
{
  if (!m.isEmpty()) {
    _pieces.push_back(m);
  }
}

void UnordPWMap::pushBack(Map&& m)
{
  if (!m.isEmpty()) {
    _pieces.push_back(std::move(m));
  }
}

// Operators -------------------------------------------------------------------

bool UnordPWMap::operator==(const UnordPWMap& other) const 
{
  if (domain() != other.domain()) {
    return false;
  }

  if (_pieces == other._pieces) {
    return true;
  }

  for (const Map& m1 : _pieces) {
    for (const Map& m2 : other._pieces) {
      Set cap_domain = m1.domain().intersection(m2.domain());
      if (!cap_domain.isEmpty()) {
        Expression expr1 = m1.law();
        Expression expr2 = m2.law();
        if (expr1 != expr2) {
          return false;
        }

        Map cap_m1{cap_domain, expr1};
        Map cap_m2{cap_domain, expr2};
        if (cap_m1 != cap_m2) {
          return false;
        }
      }
    }
  }

  return true;
}

bool UnordPWMap::operator!=(const UnordPWMap& other) const
{ 
  return !(*this == other);
}

UnordPWMap UnordPWMap::operator+(const UnordPWMap& other) const
{
  UnordPWMap result;
  
  for (const Map& m1 : _pieces) { 
    for (const Map& m2 : other._pieces) { 
      result.pushBack(m1 + m2);
    }
  }

  return result;
}

std::ostream& UnordPWMap::print(std::ostream& out) const
{
  int sz = _pieces.size();

  out << "<<";
  if (sz > 0) {
    int i = 0;
    for (; i < sz - 1; ++i) {
      out << _pieces[i] << ", ";
    }
    out << _pieces[i];
  }
  out << ">>";

  return out;
}

// PWMap functions -------------------------------------------------------------

std::size_t UnordPWMap::arity() const
{
  if (isEmpty()) {
    return 0;
  }

  return _pieces.begin()->domain().arity();
}

bool UnordPWMap::isEmpty() const { return _pieces.empty(); }

Set UnordPWMap::domain() const &
{
  Set result = SET_FACT.createSet();

  for (const Map& m : _pieces) {
    result = std::move(result).disjointCup(m.domain());
  }

  return result;
}

Set UnordPWMap::domain() &&
{
  Set result = SET_FACT.createSet();

  for (Map& m : _pieces) {
    result = std::move(result).disjointCup(std::move(m).domain());
  }

  return result;
}

UnordPWMap UnordPWMap::restrict(const Set& subdom) const
{
  UnordPWMap result;

  for (const Map& m : _pieces) {
    result.pushBack(m.restrict(subdom));
  }

  return result;
}

Set UnordPWMap::image() const
{
  Set result = SET_FACT.createSet();

  for (const Map& m : _pieces) {
    result = std::move(result).cup(m.image());
  }

  return result;
}

Set UnordPWMap::image(const Set& subdom) const
{
  return restrict(subdom).image();
}

Set UnordPWMap::preImage(const Set& subcodom) const
{
  Set result = SET_FACT.createSet();

  for (const Map& m : _pieces) {
    result = std::move(result).disjointCup(m.preImage(subcodom));
  }

  return result;
}

UnordPWMap UnordPWMap::inverse() const
{
  UnordPWMap result;

  for (const Map& m : _pieces) {
    result.emplaceBack(m.image(), m.law().inverse());
  }

  return result;
}

UnordPWMap UnordPWMap::composition(const UnordPWMap& other) const
{
  UnordPWMap result;

  for (const Map& m1 : _pieces) { 
    for (const Map& m2 : other._pieces) {
      result.pushBack(m1.composition(m2));
    }
  }

  return result;
}

UnordPWMap UnordPWMap::mapInf(unsigned int n) const
{
  UnordPWMap result{_pieces};

  if (!domain().isEmpty()) {
    for (unsigned int j = 0; j < n; ++j) {
      result = composition(result);
    }

    result = result.reduce();
    UnordPWMap old_result{result};
    do {
      old_result = result;

      result = result.composition(result).reduce();
    } while (old_result != result);
  }

  return result;
}

UnordPWMap UnordPWMap::mapInf() const { return mapInf(0); }

Set UnordPWMap::fixedPoints() const
{
  Set result = SET_FACT.createSet();

  for (const Map& m : _pieces) {
    result = std::move(result).disjointCup(m.fixedPoints());
  }

  return result;
}

// Extra operations ------------------------------------------------------------

UnordPWMap UnordPWMap::concatenation(const UnordPWMap& other) const &
{
  return UnordPWMap{*this}.concatenation(other);
}

UnordPWMap UnordPWMap::concatenation(const UnordPWMap& other) &&
{
  return std::move(*this).concatenation(UnordPWMap{other});
}

UnordPWMap UnordPWMap::concatenation(UnordPWMap&& other) const &
{
  return UnordPWMap{*this}.concatenation(std::move(other));
}

UnordPWMap UnordPWMap::concatenation(UnordPWMap&& other) &&
{
  if (isEmpty()) {
    return std::move(other);
  }

  if (other.isEmpty()) {
    return std::move(*this);
  }

  UnordMapCollection result;
  result.insert(result.end(), std::make_move_iterator(_pieces.begin())
    , std::make_move_iterator(_pieces.end()));
  result.insert(result.end(), std::make_move_iterator(other._pieces.begin())
    , std::make_move_iterator(other._pieces.end()));

  return UnordPWMap{std::move(result)};
}

UnordPWMap UnordPWMap::combine(const UnordPWMap& other) const &
{
  return UnordPWMap{*this}.combine(other);
}

UnordPWMap UnordPWMap::combine(const UnordPWMap& other) &&
{
  return std::move(*this).combine(UnordPWMap{other});
}

UnordPWMap UnordPWMap::combine(UnordPWMap&& other) const &
{
  return UnordPWMap{*this}.combine(std::move(other));
}

UnordPWMap UnordPWMap::combine(UnordPWMap&& other) && 
{
  if (isEmpty()) {
    return std::move(other);
  }

  if (other.isEmpty()) {
    return std::move(*this);
  }

  if (_pieces == other._pieces) {
    return std::move(*this);
  }

  Set exclusive_other = other.domain().difference(domain());
  return std::move(*this).concatenation(other.restrict(exclusive_other));
}

UnordPWMap UnordPWMap::reduce() const
{
  UnordPWMap result;

  for (const Map& m : _pieces) {
    std::vector<Map> reduced = m.reduce();
    for(Map& reduced_map : reduced) {
      result._pieces.push_back(std::move(reduced_map));
    }
  }

  return result;
}

UnordPWMap UnordPWMap::min(const UnordPWMap& other) const
{
  if (isEmpty() || other.isEmpty()) {
    return UnordPWMap{};
  }

  Set min_in_pw1 = lessImage(other);
  return restrict(min_in_pw1).combine(other.restrict(domain())); 
}

UnordPWMap UnordPWMap::minAdj(const UnordPWMap& other) const
{
  UnordPWMap result;

  Set visited = SET_FACT.createSet();
  for (const Map& m1 : _pieces) {
    for (const Map& m2 : other._pieces) {
      Map min_adj = m1.minAdj(m2);
      if (!min_adj.isEmpty()) {
        Set min_adj_domain = min_adj.domain();
        Set repeated = min_adj_domain.intersection(visited);
        if (!repeated.isEmpty()) {
          UnordPWMap min_adj_pw{std::move(min_adj)};
          UnordPWMap min_adj_repeated = min_adj_pw.restrict(repeated); 
          UnordPWMap result_repeated = result.restrict(repeated);
          UnordPWMap min_map = min_adj_repeated.min(result_repeated);
          UnordPWMap min_adj_result = min_map.combine(std::move(min_adj_pw));
          result = min_adj_result.combine(std::move(result));
          visited = visited.cup(min_adj_domain);
        } else {
          result.pushBack(std::move(min_adj));
          visited = std::move(visited).disjointCup(std::move(min_adj_domain));
        }
      }
    }
  }

  return result;
}

Set UnordPWMap::sharedImage() const
{
  Set repeated_image = SET_FACT.createSet();
  Set visited = SET_FACT.createSet();
  for (const Map& m : _pieces) {
    Set image_in_visited = m.image().intersection(visited);
    if (!image_in_visited.isEmpty()) {
      repeated_image = std::move(repeated_image).cup(std::move(
        image_in_visited));
    }
    visited = std::move(visited).cup(m.image());
  }

  return preImage(repeated_image);
}

Set UnordPWMap::equalImage(const UnordPWMap& other) const
{
  Set result = SET_FACT.createSet();

  if (_pieces == other._pieces) {
    return domain();
  }

  for (const Map& m1 : _pieces) {
    for (const Map& m2 : other._pieces) {
      Set cap_dom = m1.domain().intersection(m2.domain());
      if (!cap_dom.isEmpty()) {
        Map m1_cap{cap_dom, m1.law()};
        Map m2_cap{cap_dom, m2.law()};
        if (m1_cap == m2_cap) {
          result = std::move(result).disjointCup(std::move(cap_dom));
        }
      }
    }
  }

  return result;
}

Set UnordPWMap::lessImage(const UnordPWMap& other) const
{
  if (isEmpty() || other.isEmpty()) {
    return SET_FACT.createSet();
  }

  Set min_in_pw1 = SET_FACT.createSet();
  for (const Map& m1 : _pieces) {
    for (const Map& m2 : other._pieces) {
      min_in_pw1 = std::move(min_in_pw1).disjointCup(m1.lessImage(m2));
    }
  }

  return min_in_pw1; 
}  

UnordPWMap UnordPWMap::offsetDom(const MD_NAT& offset) const
{
  UnordPWMap result;

  for (const Map& m : _pieces) {
    result._pieces.emplace_back(m.domain().offset(offset), m.law());
  }

  return result;
}

bool operator<(const Map& m1, const Map& m2)
{
  return true;
}

void UnordPWMap::compact()
{
  using MapSet = std::set<Map, MapLess>;

  UnordMapCollection result;

  if (!isEmpty()) {
    MapSet prev{std::make_move_iterator(_pieces.begin())
      , std::make_move_iterator(_pieces.end())};
    MapSet actual = prev;
    do {
      prev = actual;
      actual = MapSet{};

      MapSet::iterator ith = prev.begin();
      MapSet::iterator last = prev.end();
      MapSet to_erase;
      for (; ith != last; ++ith) {
        Map ith_compact = *ith;
        MapSet::iterator next = ith;
        ++next;
        for (; next != last; ++next) {
          MaybeMap new_compact = ith_compact.compact(*next);
          if (new_compact) {
            ith_compact = new_compact.value();
            to_erase.insert(*next);
          }
        }

        if (to_erase.find(ith_compact) == to_erase.end()) {
          actual.insert(ith_compact);
        }
      }
    } while (actual != prev);

    for (const Map& mdi : actual) {
      result.push_back(mdi);
    }
  }

  _pieces = std::move(result);
}

} // namespace detail

} // namespace LIB

} // namespace SBG;

