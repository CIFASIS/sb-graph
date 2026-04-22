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

#include "sbg/dom_ord_pwmap.hpp"
#include "sbg/interval.hpp"
#include "sbg/ord_set.hpp"
#include "sbg/ord_unidim_dense_set.hpp"
#include "sbg/perimeter.hpp"
#include "sbg/pwmap_detail.hpp"
#include "sbg/set_detail.hpp"
#include "sbg/set_fact.hpp"
#include "util/defs.hpp"
#include "util/debug.hpp"

#include <forward_list>
#include <set>
#include <iostream>

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Domain Ordered PWMap Implementation -----------------------------------------
////////////////////////////////////////////////////////////////////////////////

// Auxiliary definitions -------------------------------------------------------

class MapLess {
public:
  bool operator()(const Map& a, const Map& b) const {
    return a.domain().minElem() < b.domain().minElem();
  }
};

// Constructors/Destructors ----------------------------------------------------

DomOrdPWMap::DomOrdPWMap() : _pieces() {}

DomOrdPWMap::DomOrdPWMap(const Set& s) : _pieces() {
  if (!s.isEmpty()) {
    _pieces.emplace_back(Map{s, Expression{s.arity(), 1, 0}});
  }
}

DomOrdPWMap::DomOrdPWMap(const Map& m) : _pieces() {
  if (!m.domain().isEmpty()) {
    _pieces.emplace_back(m);
  }
}

DomOrdPWMap::DomOrdPWMap(const std::vector<Map>& pieces) : _pieces() {
  for (const Map& m : pieces) {
    insert(m);
  }
}

DomOrdPWMap::DomOrdPWMap(const DomOrdPWMap::OrdMapCollection& pieces)
  : _pieces(pieces) {}

DomOrdPWMap::DomOrdPWMap(DomOrdPWMap::OrdMapCollection&& pieces)
  : _pieces(std::move(pieces)) {}

// Getters ---------------------------------------------------------------------

DomOrdPWMap::ConstIt DomOrdPWMap::begin() const { return _pieces.begin(); }

DomOrdPWMap::ConstIt DomOrdPWMap::end() const { return _pieces.end(); }

// Setters ---------------------------------------------------------------------

void DomOrdPWMap::insert(const Map& m)
{
  insert(Map{m});
}

void DomOrdPWMap::insert(Map&& m)
{
  if (!m.isEmpty()) {
    MapEntry entry{m};
    if (isEmpty() || _pieces.back() < entry) { 
      _pieces.push_back(entry);
    } else {
      insertHint(0, std::move(m));
    }
  }
}

void DomOrdPWMap::pushBack(const Map& m)
{
  pushBack(Map{m});
}

void DomOrdPWMap::pushBack(Map&& m)
{
  if (!m.isEmpty()) {
    _pieces.emplace_back(std::move(m));
  }
}

void DomOrdPWMap::pushBack(const MapEntry& entry)
{
  pushBack(MapEntry{entry});
}

void DomOrdPWMap::pushBack(MapEntry&& entry)
{
  if (!entry.map().isEmpty()) {
    _pieces.push_back(std::move(entry));
  }
}

void DomOrdPWMap::insertHint(std::size_t hint, const Map& m)
{
  insertHint(hint, Map{m});
}

void DomOrdPWMap::insertHint(std::size_t hint, Map&& m)
{
  if (!m.isEmpty()) {
    auto it = _pieces.begin();
    std::advance(it, hint);
    auto end = _pieces.end();
    MapEntry map_entry{m};
    while (it != end) {
      if (*it < map_entry) {
        ++it;
      } else {
        break;
      }
    }
    _pieces.insert(it, map_entry);
  }
}

std::size_t DomOrdPWMap::advanceHint(std::size_t hint, const MapEntry& jth_entry)
{
  auto it = _pieces.begin();
  std::advance(it, hint);
  auto end = _pieces.end();
  while (it != end) {
    if (*it < jth_entry) {
      ++it;
      ++hint;
    } else {
      break;
    }
  }

  return hint;
}

// Traverse --------------------------------------------------------------------

template<typename OrdCollection1, typename OrdCollection2, typename Core>
Core traverse(const OrdCollection1& lhs, const OrdCollection2& rhs
  , Core core_op)
{
  OrdCollection1 short_collection = lhs;
  OrdCollection2 long_collection = rhs;
  //if (!core_op.orderMatters()
  //  && long_collection.size() < short_collection.size()) {
  //  short_collection = rhs;
  //  long_collection  = lhs;
  //}

  // Indexes list corresponding to remaining maps in short_collection
  std::forward_list<size_t> indexes;
  int short_size = short_collection.size();
  for (int i = short_size - 1; i >= 0; --i) {
    indexes.push_front(i);
  }

  auto short_begin = short_collection.begin();
  for (const auto& long_elem : long_collection) {
    const Perimeter& long_perimeter = long_elem.perimeter();
    
    auto prev_index = indexes.before_begin();
    auto curr_index = indexes.begin();
    while (curr_index != indexes.end()) {
      size_t idx = *curr_index;
      const auto& short_elem = *(short_begin + idx);
      const Perimeter& short_perimeter = short_elem.perimeter(); 
      
      // Here short_elem is "before" long_elem, so it is also "before" all the
      // remaining elements in long_collection, thus it can be discarded. 
      if (short_perimeter.max() < long_perimeter.min()) {
        curr_index = indexes.erase_after(prev_index);
        continue;
      }

      // Here short_elem is "after" long_elem, so no comparison is needed, and
      // the loop of long_collection continues to check if this short_map interacts
      // with the following elements of long_collection. 
      if (long_perimeter.max() < short_perimeter.min()) {
        break;
      }

      if (short_perimeter.overlap(long_perimeter)) {
        if (!core_op(short_elem, long_elem)) {
          return core_op;
        }
      }

      ++prev_index;
      ++curr_index;
    }

    if (indexes.empty()) {
      break;
    }
  }

  return core_op;
}

// Operators -------------------------------------------------------------------

class EqualCore {
public:
  bool operator()(const MapEntry& entry1, const MapEntry& entry2) {
    Map m1 = entry1.map();
    Map m2 = entry2.map();
    Set cap_domain = m1.domain().intersection(m2.domain());
    if (!cap_domain.isEmpty()) {
      Expression expr1 = m1.law();
      Expression expr2 = m2.law();
      if (expr1 != expr2) {
        _are_equal = false;
        return false;
      }

      Map cap_m1{cap_domain, expr1};
      Map cap_m2{cap_domain, expr2};
      if (cap_m1 != cap_m2) {
        _are_equal = false;
        return false;
      }
    }

    _are_equal = true;
    return true;
  }

  bool orderMatters() const { return false; }

  bool result() const { return _are_equal; }

private:
  bool _are_equal = true;
};

bool DomOrdPWMap::operator==(const DomOrdPWMap& other) const 
{
  if (domain() != other.domain()) {
    return false;
  }

  if (_pieces == other._pieces) {
    return true;
  }

  return traverse(_pieces, other._pieces, EqualCore{}).result();
}

bool DomOrdPWMap::operator!=(const DomOrdPWMap& other) const
{ 
  return !(*this == other);
}

class AddCore {
public:
  bool operator()(const MapEntry& entry1, const MapEntry& entry2) {
    Map added = entry1.map() + entry2.map();
    _global_pos = _result.advanceHint(_global_pos, entry2);
    _result.insertHint(_global_pos, added);
    return true;
  }

  bool orderMatters() const { return false; }

  DomOrdPWMap result() const { return _result; }

private:
  DomOrdPWMap _result;
  std::size_t _global_pos = 0; 
};

DomOrdPWMap DomOrdPWMap::operator+(const DomOrdPWMap& other) const
{
  if (isEmpty() || other.isEmpty()) {
    return DomOrdPWMap{};
  }

  return traverse(_pieces, other._pieces, AddCore{}).result();
}

std::ostream& DomOrdPWMap::print(std::ostream& out) const
{
  int sz = _pieces.size();

  out << "<<";
  if (sz > 0) {
    int i = 0;
    for (; i < sz - 1; ++i) {
      out << _pieces[i].map() << ", ";
    }
    out << _pieces[i].map();
  }
  out << ">>";

  return out;
}

// PWMap functions -------------------------------------------------------------

std::size_t DomOrdPWMap::arity() const
{
  if (isEmpty()) {
    return 0;
  }

  return _pieces.begin()->map().domain().arity();
}

bool DomOrdPWMap::isEmpty() const { return _pieces.empty(); }

Set DomOrdPWMap::domain() const &
{
  Set result = SET_FACT.createSet();

  for (const MapEntry& entry : _pieces) {
    result = std::move(result).disjointCup(entry.map().domain());
  }

  return result;
}

Set DomOrdPWMap::domain() &&
{
  Set result = SET_FACT.createSet();

  for (MapEntry& entry : _pieces) {
    result = std::move(result).disjointCup(std::move(entry.map()).domain());
  }

  return result;
}

template<typename T, typename SetImplT>
class RestrictCore {
public:
  bool operator()(const MapEntry& entry, const T& subdom) {
    SetAccessKey key = SetAccess::key();
    Set set_subdom = key.createSet(SetImplT{subdom});
    _result.insert(entry.map().restrict(set_subdom));

    return true;
  }

  bool orderMatters() const { return true; }

  DomOrdPWMap result() const { return DomOrdPWMap(std::move(_result)); }

private:
  DomOrdPWMap _result;
};

DomOrdPWMap DomOrdPWMap::restrict(const Set& subdom) const
{
  if (isEmpty() || subdom.isEmpty()) {
    return DomOrdPWMap{};
  }

  SetAccessKey key = SetAccess::key();
  auto restrict_evaluator = Util::Overload {
    [&](const OrderedSet& impl)
    {
      return traverse(_pieces, key.pieces(impl)
        , RestrictCore<MultiDimInter, OrderedSet>{}).result();
    },
    [&](const OrdUnidimDenseSet& impl)
    {
      return traverse(_pieces, key.pieces(impl)
        , RestrictCore<Interval, OrdUnidimDenseSet>{}).result();
    },
    [&](const auto& impl)
    {
      Util::ERROR("DomOrdPWMap::restrict: unsupported ", SET_FACT.kind()
        , " set implementation");
      return DomOrdPWMap{};
    }
  };

  //print(std::cout);
  //std::cout << "\n";
  //std::cout << subdom << "\n";
  //std::visit(restrict_evaluator, key.impl(subdom)).print(std::cout);
  //std::cout << "\n\n";

  return std::visit(restrict_evaluator, key.impl(subdom));
}

Set DomOrdPWMap::image() const
{
  Set result = SET_FACT.createSet();

  for (const MapEntry& entry : _pieces) {
    result = std::move(result).cup(entry.map().image());
  }

  return result;
}

Set DomOrdPWMap::image(const Set& subdom) const
{
  return restrict(subdom).image();
}

Set DomOrdPWMap::preImage(const Set& subcodom) const
{
  Set result = SET_FACT.createSet();

  for (const MapEntry& entry : _pieces) {
    result = std::move(result).disjointCup(entry.map().preImage(subcodom));
  }

  return result;
}

DomOrdPWMap DomOrdPWMap::inverse() const
{
  DomOrdPWMap result;

  for (const MapEntry& entry : _pieces) {
    Map m = entry.map();
    result.emplace(Map{m.image(), m.law().inverse()});
  }

  return result;
}

DomOrdPWMap DomOrdPWMap::composition(const DomOrdPWMap& other) const
{
  DomOrdPWMap result;

  NAT global_pos = 0;
  for (const MapEntry& other_entry : other._pieces) {
    Map other_map = other_entry.map();
    Set img = other_map.image();
    
    Perimeter img_perimeter = img.perimeter();
    result.advanceHint(global_pos, other_entry);
    MD_NAT img_max_perimeter = img_perimeter.max();

    for (const MapEntry& entry : _pieces) {
      const Perimeter& entry_perimeter = entry.perimeter();
      if (entry_perimeter.overlap(img_perimeter)) {
        Map composed = entry.map().composition(other_map);
        result.insertHint(global_pos, composed);
        continue;
      }

      // No possible intersection between the current image and the remaining
      // elements of _pieces. 
      if (img_max_perimeter < entry_perimeter.min()) {
        break;
      }
    }
  }

  return result;
}

DomOrdPWMap DomOrdPWMap::mapInf(unsigned int n) const
{
  DomOrdPWMap result{_pieces};

  if (!domain().isEmpty()) {
    for (unsigned int j = 0; j < n; ++j) {
      result = composition(result);
    }

    result = result.reduce();
    DomOrdPWMap old_result{result};
    do {
      old_result = result;

      result = result.composition(result).reduce();
    } while (old_result != result);
  }

  return result;
}

DomOrdPWMap DomOrdPWMap::mapInf() const { return mapInf(0); }

Set DomOrdPWMap::fixedPoints() const
{
  Set result = SET_FACT.createSet();

  for (const MapEntry& entry : _pieces) {
    result = std::move(result).disjointCup(entry.map().fixedPoints());
  }

  return result;
}

// Extra operations ------------------------------------------------------------

DomOrdPWMap DomOrdPWMap::concatenation(const DomOrdPWMap& other) const &
{
  return DomOrdPWMap{*this}.concatenation(other);
}

DomOrdPWMap DomOrdPWMap::concatenation(const DomOrdPWMap& other) &&
{
  return std::move(*this).concatenation(DomOrdPWMap{other});
}

DomOrdPWMap DomOrdPWMap::concatenation(DomOrdPWMap&& other) const &
{
  return DomOrdPWMap{*this}.concatenation(std::move(other));
}

DomOrdPWMap DomOrdPWMap::concatenation(DomOrdPWMap&& other) &&
{
  // Special cases
  if (isEmpty()) { 
    return std::move(other);
  }

  if (other.isEmpty()) {
    return std::move(*this);
  }

  OrdMapCollection result;
  if (_pieces.back() < other._pieces.front()) {
    result.insert(result.end(), std::make_move_iterator(_pieces.begin())
      , std::make_move_iterator(_pieces.end()));
    result.insert(result.end(), std::make_move_iterator(other._pieces.begin())
      , std::make_move_iterator(other._pieces.end()));
  } else if (other._pieces.back() < _pieces.front()) {
    result.insert(result.end(), std::make_move_iterator(other._pieces.begin())
      , std::make_move_iterator(other._pieces.end()));
    result.insert(result.end(), std::make_move_iterator(_pieces.begin())
      , std::make_move_iterator(_pieces.end()));
  } else { 
    // General case
    auto it1 = _pieces.begin();
    auto end1 = _pieces.end();
    auto it2 = other._pieces.begin();
    auto end2 = other._pieces.end();
    while (it1 != end1 && it2 != end2) {
      if (*it1 < *it2) {
        result.push_back(*it1);
        ++it1;
      } else {
        result.push_back(*it2);
        ++it2;
      }
    }
    result.insert(result.end(), std::make_move_iterator(it1)
      , std::make_move_iterator(end1));
    result.insert(result.end(), std::make_move_iterator(it2)
      , std::make_move_iterator(end2));
  }

  return DomOrdPWMap{std::move(result)};
}

DomOrdPWMap DomOrdPWMap::combine(const DomOrdPWMap& other) const &
{
  return DomOrdPWMap{*this}.combine(other);
}

DomOrdPWMap DomOrdPWMap::combine(const DomOrdPWMap& other) &&
{
  return std::move(*this).combine(DomOrdPWMap{other});
}

DomOrdPWMap DomOrdPWMap::combine(DomOrdPWMap&& other) const &
{
  return DomOrdPWMap{*this}.combine(std::move(other));
}

DomOrdPWMap DomOrdPWMap::combine(DomOrdPWMap&& other) && 
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

DomOrdPWMap DomOrdPWMap::reduce() const
{
  DomOrdPWMap result;

  for (const MapEntry& entry : _pieces) {
    std::vector<Map> reduced = entry.map().reduce();
    for (Map& reduced_map : reduced) {
      result.insert(std::move(reduced_map));
    }
  }

  return result;
}

DomOrdPWMap DomOrdPWMap::min(const DomOrdPWMap& other) const
{
  if (isEmpty() || other.isEmpty()) {
    return DomOrdPWMap{};
  }

  Set min_in_pw1 = lessImage(other);
  return restrict(min_in_pw1).combine(other.restrict(domain())); 
}

class MinAdjCore {
public:
  bool operator()(const MapEntry& entry1, const MapEntry& entry2) {
    Map min_adj = entry1.map().minAdj(entry2.map());
    if (!min_adj.isEmpty()) {
      Set min_adj_domain = min_adj.domain();
      Set repeated = min_adj_domain.intersection(_visited);
      if (!repeated.isEmpty()) {
        DomOrdPWMap min_adj_pw{std::move(min_adj)};
        DomOrdPWMap min_adj_repeated = min_adj_pw.restrict(repeated); 
        DomOrdPWMap result_repeated = _result.restrict(repeated);
        DomOrdPWMap min_map = min_adj_repeated.min(result_repeated);
        DomOrdPWMap min_adj_result = std::move(min_map).combine(
          std::move(min_adj_pw));
        _result = std::move(min_adj_result).combine(std::move(_result));
        _visited = std::move(_visited).cup(std::move(min_adj_domain));
      } else {
        _result.insert(std::move(min_adj));
        _visited = std::move(_visited).disjointCup(std::move(min_adj_domain));
      }
    }

    return true;
  }

  bool orderMatters() const { return true; }

  DomOrdPWMap result() const { return _result; }

private:
  DomOrdPWMap _result;
  Set _visited = SET_FACT.createSet(); 
};

DomOrdPWMap DomOrdPWMap::minAdj(const DomOrdPWMap& other) const
{
  if (isEmpty() || other.isEmpty()) {
    return DomOrdPWMap{};
  }

  return traverse(_pieces, other._pieces, MinAdjCore{}).result();
}

Set DomOrdPWMap::sharedImage() const
{
  Set repeated_image = SET_FACT.createSet();
  Set visited = SET_FACT.createSet();
  for (const MapEntry& entry : _pieces) {
    Set m_image = entry.map().image();
    Set image_in_visited = m_image.intersection(visited);
    if (!image_in_visited.isEmpty()) {
      repeated_image = std::move(repeated_image).cup(std::move(
        image_in_visited));
    }
    visited = std::move(visited).cup(m_image);
  }

  return preImage(repeated_image);
}

class EqualImageCore {
public:
  bool operator()(const MapEntry& entry1, const MapEntry& entry2) {
    Map m1 = entry1.map();
    Map m2 = entry2.map();
    Set cap_dom = m1.domain().intersection(m2.domain());
    if (!cap_dom.isEmpty()) {
      Map m1_cap{cap_dom, m1.law()};
      Map m2_cap{cap_dom, m2.law()};
      if (m1_cap == m2_cap) {
        _result = std::move(_result).disjointCup(std::move(cap_dom));
      }
    }

    return true;
  }

  bool orderMatters() const { return false; }

  Set result() const { return _result; }

private:
  Set _result = SET_FACT.createSet();
};

Set DomOrdPWMap::equalImage(const DomOrdPWMap& other) const
{
  if (isEmpty() || other.isEmpty()) {
    return SET_FACT.createSet();
  }

  if (_pieces == other._pieces) {
    return domain();
  }

  return traverse(_pieces, other._pieces, EqualImageCore{}).result();
}

class LessImageCore {
public:
  bool operator()(const MapEntry& entry1, const MapEntry& entry2) {
    Set less_map = entry1.map().lessImage(entry2.map());
    _result = std::move(_result).disjointCup(std::move(less_map));

    return true;
  }

  bool orderMatters() const { return true; }

  Set result() const { return _result; }

private:
  Set _result = SET_FACT.createSet();
};

Set DomOrdPWMap::lessImage(const DomOrdPWMap& other) const
{
  if (isEmpty() || other.isEmpty()) {
    return SET_FACT.createSet();
  }

  if (_pieces == other._pieces) {
    return SET_FACT.createSet();
  }

  return traverse(_pieces, other._pieces, LessImageCore{}).result();
} 

DomOrdPWMap DomOrdPWMap::imageMultiplicity() const
{
  Map initial_result{image(), Expression{arity(), 0, 0}};
  DomOrdPWMap result{initial_result};

  for (const MapEntry& entry : _pieces) {
    DomOrdPWMap jth_mult{entry.map().imageMultiplicity()};
    DomOrdPWMap sum_mult = jth_mult + result;
    result = std::move(sum_mult).combine(std::move(result));
  }

  return result;
} 

void DomOrdPWMap::compact()
{
  using MapSet = std::set<Map, MapLess>;

  DomOrdPWMap result;

  if (!isEmpty()) {
    MapSet set_result;
    for (const MapEntry& entry : _pieces) {
      const Map& m = entry.map();
      Set new_domain = m.domain();
      new_domain.compact();
      set_result.emplace(new_domain, m.law());
    }

    MapSet to_erase;
    do {
      MapSet new_set_result;
      to_erase.clear();

      MapSet::iterator ith = set_result.begin();
      MapSet::iterator last = set_result.end();
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
          new_set_result.insert(ith_compact);
        }
      }

      std::swap(set_result, new_set_result);
    } while (!to_erase.empty());

    for (const Map& m : set_result) {
      result.insert(m);
    }
  }

  _pieces = std::move(result._pieces);
}

} // namespace detail

} // namespace LIB

} // namespace SBG;

