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

#include "sbg/ord_unidim_dense_set.hpp"
#include "util/debug.hpp"

#include <iostream>

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Auxiliary functions ---------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

Interval least(const Interval& lhs, const Interval& rhs)
{
  return lhs < rhs ? lhs : rhs;
}

bool overlap(const OrdUnidimDenseSet::OrdIntervalCollection& lhs,
  const OrdUnidimDenseSet::OrdIntervalCollection& rhs)
{
  return lhs.back().end() >= rhs.front().begin()
    || rhs.back().end() >= lhs.front().begin();
}

Interval intersection(const Interval& lhs, const Interval& rhs)
{
  return lhs.intersection(rhs);
}

OrdUnidimDenseSet::OrdIntervalCollection
  compact(const OrdUnidimDenseSet::OrdIntervalCollection& c)
{
  OrdUnidimDenseSet::OrdIntervalCollection result;

  if (c.empty()) {
    return result;
  }

  auto next_it = c.begin();
  ++next_it;
  Interval compacted = c.front();
  for (auto it = c.begin(); next_it != c.end(); ++it) {
    MaybeInterval ith = compacted.compact(*next_it);
    if (!ith) {
      result.push_back(compacted);
      compacted = *next_it;
    } else {
      compacted = ith.value();
    }

    ++next_it;
  }
  result.push_back(compacted);

  return result;
}

////////////////////////////////////////////////////////////////////////////////
// Ordered Unidimensional Dense Set Implementation -----------------------------
////////////////////////////////////////////////////////////////////////////////

// Constructors/Destructors ----------------------------------------------------

OrdUnidimDenseSet::OrdUnidimDenseSet() : _pieces() {}

OrdUnidimDenseSet::OrdUnidimDenseSet(const NAT x) : _pieces()
{
  _pieces.emplace_back(x);
}

OrdUnidimDenseSet::OrdUnidimDenseSet(const Interval& i) : _pieces()
{
  if (!i.isEmpty()) {
    _pieces.push_back(i);
  }
}

OrdUnidimDenseSet::OrdUnidimDenseSet(
  const OrdUnidimDenseSet::OrdIntervalCollection& pieces)
  : _pieces(pieces) {}

OrdUnidimDenseSet::OrdUnidimDenseSet(
  OrdUnidimDenseSet::OrdIntervalCollection&& pieces)
  : _pieces(std::move(pieces)) {}

OrdUnidimDenseSet::OrdUnidimDenseSet(const FixedPointsInfo& info)
{
  if (info) {
    std::vector<Solution> solutions = info.value();
    for (const Solution& jth_solution : solutions) {
      if (jth_solution.kind() == SolutionKind::kFixed) {
        _pieces.emplace_back(jth_solution.value().value());
      } else {
        _pieces.emplace_back(0, 1, Inf);
      }
    }
  }
}

// Getters ---------------------------------------------------------------------

OrdUnidimDenseSet::ConstIt OrdUnidimDenseSet::begin() const
{
  return _pieces.begin();
}

OrdUnidimDenseSet::ConstIt OrdUnidimDenseSet::end() const
{
  return _pieces.end();
}

// Setters ---------------------------------------------------------------------

void OrdUnidimDenseSet::pushBack(const Interval& i)
{
  if (i.isEmpty()) {
    return;
  }

  if (_pieces.empty() || _pieces.back() < i) {
    _pieces.push_back(i);
    return;
  }

  if (i < _pieces.front()) {
    _pieces.insert(_pieces.begin(), i);
    return;
  }

  auto it = _pieces.rbegin();
  auto rend = _pieces.rend();
  for (; it != rend; ++it) {
    if (*it < i) {
      break;
    }
  }
  _pieces.insert(it.base(), i);
}

// Operators -------------------------------------------------------------------

bool OrdUnidimDenseSet::operator==(const OrdUnidimDenseSet& other) const
{
  if (isEmpty() && other.isEmpty()) {
    return true;
  }

  if (isEmpty() != other.isEmpty()) {
    return false;
  }

  if (_pieces == other._pieces) {
    return true;
  }

  return detail::compact(_pieces) == detail::compact(other._pieces);
}

bool OrdUnidimDenseSet::operator!=(const OrdUnidimDenseSet& other) const
{
  return !(*this == other);
}

std::ostream& OrdUnidimDenseSet::print(std::ostream& out) const
{
  std::size_t sz = _pieces.size();

  out << "{";
  if (sz > 0) {
    unsigned int j = 0;
    for (const Interval& i : _pieces) { 
      if (j < sz - 1) {
        out << i << ", ";
      } else {
        out << i;
      }

      ++j;
    }
  }
  out << "}";

  return out;
}

// Set operations --------------------------------------------------------------

unsigned int OrdUnidimDenseSet::cardinal() const
{
  unsigned int result = 0;

  for (const Interval& i : _pieces) {
    result += i.cardinal();
  }

  return result;
}

bool OrdUnidimDenseSet::isEmpty() const { return _pieces.empty(); }

MD_NAT OrdUnidimDenseSet::minElem() const
{
  return MD_NAT(_pieces.front().begin());
}

MD_NAT OrdUnidimDenseSet::maxElem() const
{
  return MD_NAT(_pieces.back().end());
}

OrdUnidimDenseSet OrdUnidimDenseSet::intersection(const OrdUnidimDenseSet&
  other) const
{
  // Special cases to enhance performance
  if (isEmpty() || other.isEmpty()) {
    return OrdUnidimDenseSet{};
  }

  if (!overlap(_pieces, other._pieces)) {
    return OrdUnidimDenseSet{};
  }

  if (_pieces == other._pieces) {
    return OrdUnidimDenseSet{_pieces};
  }

  return boundedTraverse(detail::intersection, other);
}

OrdUnidimDenseSet OrdUnidimDenseSet::cup(const OrdUnidimDenseSet& other) const &
{
  return OrdUnidimDenseSet{*this}.cup(other);
}

OrdUnidimDenseSet OrdUnidimDenseSet::cup(const OrdUnidimDenseSet& other) &&
{
  return std::move(*this).cup(OrdUnidimDenseSet{other});
}

OrdUnidimDenseSet OrdUnidimDenseSet::cup(OrdUnidimDenseSet&& other) const &
{
  return OrdUnidimDenseSet{*this}.cup(std::move(other));
}

OrdUnidimDenseSet OrdUnidimDenseSet::cup(OrdUnidimDenseSet&& other) && 
{ 
  // Special cases
  if (other.isEmpty() || _pieces == other._pieces) {
    return OrdUnidimDenseSet{std::move(_pieces)};
  }

  if (isEmpty()) { 
    return OrdUnidimDenseSet{std::move(other._pieces)};
  }

  OrdIntervalCollection result;
  if (!overlap(_pieces, other._pieces)) {
    return std::move(*this).disjointCup(std::move(other));
  }

  // General case
  OrdUnidimDenseSet exclusive_this = difference(other);
  return std::move(other).disjointCup(std::move(exclusive_this));
}

OrdUnidimDenseSet OrdUnidimDenseSet::complement() const
{
  OrdUnidimDenseSet result;

  if (isEmpty()) {
    result._pieces.emplace_back(0, 1, Inf); 
    return result;
  }

  // Complement before minimum element of the set
  result._pieces.emplace_back(0, 1, _pieces.front().minElem() - 1);

  // Complement between pieces of the set
  NAT last_interval_end = _pieces.front().maxElem();
  unsigned int j = 0;
  for (const Interval& i : _pieces) {
    if (j != 0) {
      if (i.begin() - 1 >= last_interval_end + 1) {
        result._pieces.emplace_back(last_interval_end + 1, 1, i.begin() - 1);
      }
      last_interval_end = i.maxElem();
    }
    ++j;
  }

  // Complement after maximum element of the set
  result._pieces.emplace_back(last_interval_end + 1, 1, Inf);

  return result;
}

OrdUnidimDenseSet OrdUnidimDenseSet::difference(const OrdUnidimDenseSet& other)
  const
{
  if (isEmpty() || other.isEmpty()) {
    return OrdUnidimDenseSet{_pieces};
  }

  if (!overlap(_pieces, other._pieces)) {
    return OrdUnidimDenseSet{_pieces};
  }

  if (_pieces == other._pieces) {
    return OrdUnidimDenseSet{};
  }

  return intersection(other.complement());
}

OrdUnidimDenseSet OrdUnidimDenseSet::cartesianProduct(const OrdUnidimDenseSet&
  other) const
{
  Util::ERROR("OrdUnidimDenseSet::cartesianProduct: operation not supported\n");

  return OrdUnidimDenseSet{};
}

// Extra operations ------------------------------------------------------------

std::size_t OrdUnidimDenseSet::arity() const
{
  if (isEmpty()) {
    return 0;
  }

  return 1;
}

OrdUnidimDenseSet OrdUnidimDenseSet::disjointCup(const OrdUnidimDenseSet& other)
  const &
{
  return OrdUnidimDenseSet{*this}.disjointCup(other);
}

OrdUnidimDenseSet OrdUnidimDenseSet::disjointCup(const OrdUnidimDenseSet& other)
 &&
{
  return std::move(*this).disjointCup(OrdUnidimDenseSet{other});
}

OrdUnidimDenseSet OrdUnidimDenseSet::disjointCup(OrdUnidimDenseSet&& other)
  const &
{
  return OrdUnidimDenseSet{*this}.disjointCup(std::move(other));
}

OrdUnidimDenseSet OrdUnidimDenseSet::disjointCup(OrdUnidimDenseSet&& other) &&
{
  // Special cases
  if (isEmpty()) {
    return OrdUnidimDenseSet{std::move(other._pieces)};
  }

  if (other.isEmpty() || _pieces == other._pieces) {
    return OrdUnidimDenseSet{std::move(_pieces)};
  }

  if (!overlap(_pieces, other._pieces)) {
    OrdIntervalCollection result;
    result.insert(result.end(), std::make_move_iterator(_pieces.begin())
      , std::make_move_iterator(_pieces.end()));
    result.insert(result.end(), std::make_move_iterator(other._pieces.begin())
      , std::make_move_iterator(other._pieces.end()));
    return result;
  }
  
  return traverse(least, other);
}

OrdUnidimDenseSet OrdUnidimDenseSet::offset(const MD_NAT& offset) const
{
  OrdUnidimDenseSet result;

  for (const Interval& i : _pieces) {
    result.pushBack(i.offset(offset[0]));
  }

  return result;
}

Perimeter OrdUnidimDenseSet::perimeter() const
{
  return Perimeter{minElem(), maxElem()};
}

void OrdUnidimDenseSet::compact()
{
  if (isEmpty()) {
    return;
  }

  OrdIntervalCollection result;
  auto next_it = _pieces.begin();
  ++next_it;
  Interval compacted = _pieces.front();
  for (auto it = _pieces.begin(); next_it != _pieces.end(); ++it) {
    MaybeInterval ith = compacted.compact(*next_it);
    if (!ith) {
      result.push_back(compacted);
      compacted = *next_it;
    } else {
      compacted = ith.value();
    }

    ++next_it;
  }
  result.push_back(compacted);

  _pieces = std::move(result);
}

OrdUnidimDenseSet OrdUnidimDenseSet::boundedTraverse(
  Interval f(const Interval&, const Interval&), const OrdUnidimDenseSet& other
) const
{
  OrdUnidimDenseSet result;
  auto it1 = _pieces.begin();
  auto end1 = _pieces.end();
  auto it2 = other._pieces.begin();
  auto end2 = other._pieces.end();
  for (int j = 0; it1 != end1 && it2 != end2; ++j) {
    const Interval i1 = *it1;
    const Interval i2 = *it2;
    result.pushBack(f(i1, i2));

    if (i1.maxElem() < i2.maxElem()) {
      ++it1;
    } else { 
      ++it2;
    }
  }

  return result;
}

OrdUnidimDenseSet OrdUnidimDenseSet::traverse(
  Interval f(const Interval&, const Interval&) , const OrdUnidimDenseSet& other)
  const
{
  OrdUnidimDenseSet result;

  auto it1 = _pieces.begin();
  auto end1 = _pieces.end();
  auto it2 = other._pieces.begin();
  auto end2 = other._pieces.end();
  for (; it1 != end1 && it2 != end2;) {
    const Interval i1 = *it1;
    const Interval i2 = *it2;
    result.pushBack(f(i1, i2));

    if (i1.maxElem() < i2.maxElem()) {
      ++it1;
    } else {
      ++it2;
    }
  }
  result._pieces.insert(result.end(), it1, end1);
  result._pieces.insert(result.end(), it2, end2);

  return result;
}

// Non-member functions --------------------------------------------------------

rapidjson::Value toJSON(OrdUnidimDenseSet s
  , rapidjson::Document::AllocatorType& alloc)
{
  rapidjson::Value result{rapidjson::kArrayType};

  rapidjson::Value interval_array{rapidjson::kArrayType};
  for (const Interval& i : s) {
    rapidjson::Value jth = detail::toJSON(i, alloc);
    interval_array.PushBack(jth, alloc);
  }
  rapidjson::Value mdi_obj{rapidjson::kObjectType};
  mdi_obj.AddMember("pieces", interval_array, alloc);
  result.PushBack(mdi_obj, alloc);

  return result;
}

} // namespace detail

} // namespace LIB

} // namespace SBG
