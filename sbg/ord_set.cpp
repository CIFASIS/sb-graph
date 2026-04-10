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

#include "sbg/ord_set.hpp"

#include <forward_list>
#include <iostream>
#include <optional>
#include <set>

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Auxiliary functions ---------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Checks if the both set pieces overlap.
 */
bool overlap(const MultiDimInter& mdi1, const MultiDimInter& mdi2)
{
  const auto max1 = mdi1.maxElem();
  const auto min1 = mdi1.minElem();
  const auto max2 = mdi2.maxElem();
  const auto min2 = mdi2.minElem();
  const unsigned int arity = mdi1.arity();

  for (unsigned int j = 0; j < arity; ++j) {
    if (max1[j] < min2[j] || max2[j] < min1[j]) {
      return false;
    }
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////
// Ordered Set Implementation --------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

// Constructors/Destructors ----------------------------------------------------

OrderedSet::OrderedSet() : _pieces() {}

OrderedSet::OrderedSet(const MD_NAT& x) : _pieces()
{
  _pieces.push_back(MultiDimInter{x});
}

OrderedSet::OrderedSet(const Interval& i) : _pieces()
{
  if (!i.isEmpty()) {
    _pieces.push_back(MultiDimInter{i});
  }
}

OrderedSet::OrderedSet(const MultiDimInter& mdi) : _pieces()
{
  if (!mdi.isEmpty()) {
    _pieces.push_back(mdi);
  }
}

OrderedSet::OrderedSet(const OrderedSet::OrdMDICollection& pieces)
  : _pieces(pieces) {}

OrderedSet::OrderedSet(OrderedSet::OrdMDICollection&& pieces)
  : _pieces(std::move(pieces)) {}

OrderedSet::OrderedSet(const FixedPointsInfo& info) : _pieces()
{
  if (info) {
    std::vector<Solution> solutions = info.value();
    Interval universe_one_dim{0, 1, Inf};
    MultiDimInter result_mdi;
    for (const Solution& jth_solution : solutions) {
      if (jth_solution.kind() == SolutionKind::kFixed) {
        result_mdi.pushBack(jth_solution.value().value());
      } else if (jth_solution.kind() == SolutionKind::kFree) {
        result_mdi.pushBack(universe_one_dim);
      }
    }
    pushBack(result_mdi);
  }
}

// Getters ---------------------------------------------------------------------

OrderedSet::ConstIt OrderedSet::begin() const { return _pieces.begin(); }

OrderedSet::ConstIt OrderedSet::end() const { return _pieces.end(); }

// Setters ---------------------------------------------------------------------

void OrderedSet::pushBack(const MultiDimInter& mdi)
{
  if (mdi.isEmpty()) {
    return;
  }

  if (_pieces.empty() || _pieces.back() < mdi) {
    _pieces.push_back(mdi);
    return;
  }

  auto it = _pieces.rbegin();
  for (; it != _pieces.rend(); ++it) {
    if (*it < mdi) {
      break;
    }
  }
  _pieces.insert(it.base(), mdi);
}

NAT OrderedSet::advanceHint(NAT hint, const MultiDimInter& mdi)
{
  auto it = _pieces.begin();
  auto end = _pieces.end();
  std::advance(it, hint);
  for (; it != end; ++it) {
    if (mdi < *it) {
      break;
    }
    ++hint;
  }
  
  return hint;
}

void OrderedSet::insertHint(const NAT hint, const MultiDimInter& mdi)
{
  if (mdi.isEmpty()) {
    return;
  }

  if (_pieces.empty() || _pieces.back() < mdi) {
    _pieces.emplace_back(mdi);
    return;
  }

  auto it = _pieces.begin();
  auto end = _pieces.end();
  std::advance(it, hint);
  for (; it != end; ++it) { 
    if (mdi < *it) {
      break;
    }
  }
  _pieces.emplace(it, mdi);
}

// Operators -------------------------------------------------------------------

bool OrderedSet::operator==(const OrderedSet& other) const
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

  return difference(other).isEmpty() && other.difference(*this).isEmpty();
}

bool OrderedSet::operator!=(const OrderedSet& other) const
{
  return !(*this == other);
}

std::ostream& OrderedSet::print(std::ostream& out) const
{
  std::size_t sz = _pieces.size();

  out << "{";
  if (sz > 0) {
    unsigned int j = 0;
    for (const MultiDimInter& mdi : _pieces) { 
      if (j < sz - 1) {
        out << mdi << ", "; 
      } else {
        out << mdi;
      }

      ++j;
    }
  }
  out << "}";

  return out;
}

// Set operations --------------------------------------------------------------

unsigned int OrderedSet::cardinal() const
{
  unsigned int result = 0;

  for (const MultiDimInter& mdi : _pieces) {
    result += mdi.cardinal();
  }

  return result;
}

bool OrderedSet::isEmpty() const { return _pieces.empty(); }

MD_NAT OrderedSet::minElem() const
{
  return _pieces.begin()->minElem();
}

MD_NAT OrderedSet::maxElem() const
{
  MD_NAT result = _pieces.begin()->maxElem();

  for (const MultiDimInter& mdi : _pieces) {
    result = std::max(result, mdi.maxElem());
  }
  
  return result;
}

OrderedSet OrderedSet::intersectionEpilogue(const OrderedSet& lhs
  , const OrderedSet& rhs) const
{
  OrderedSet result;

  // General case
  OrderedSet::OrdMDICollection short_set = lhs._pieces;
  OrderedSet::OrdMDICollection long_set = rhs._pieces;
  if (rhs._pieces.size() < lhs._pieces.size()) {
    short_set = rhs._pieces;
    long_set  = lhs._pieces;
  }
 
  // Indexes list corresponding to remaining pieces in short_set 
  std::forward_list<size_t> indexes;
  int short_size = short_set.size();
  for (int i = short_size - 1; i >= 0; --i) {
    indexes.push_front(i);
  }

  NAT global_position = 0;
  auto short_begin = short_set.begin();
  for (const MultiDimInter& long_elem : long_set) {
    const MD_NAT long_min = long_elem.minElem();
    const MD_NAT long_max = long_elem.maxElem();

    auto prev_index = indexes.before_begin();
    auto curr_index = indexes.begin();
    global_position = result.advanceHint(global_position, long_elem);
    while (curr_index != indexes.end()) {
      const size_t idx = *curr_index;
      const MultiDimInter short_elem = *(short_begin + idx);
      const auto short_min = short_elem.minElem();
      const auto short_max = short_elem.maxElem();

      // Here short_elem is "before" long_elem, so it is also "before" all the
      // remaining sets in long_set, thus it can be discarded. 
      if (short_max < long_min) {
        curr_index = indexes.erase_after(prev_index);
        continue;
      }

      // Here short_elem is "after" long_elem, so no comparison is needed, and
      // the loop of long_set continues to check if this short_elem interacts
      // with the following elements of long_set. 
      if (long_max < short_min) {
        break;
      }

      if (overlap(short_elem, long_elem)) {
        const MultiDimInter inter = long_elem.intersection(short_elem);
        result.insertHint(global_position, inter);
      }

      ++prev_index;
      ++curr_index;
    }

    if (indexes.empty()) {
      break;
    }
  }

  return result;
}

OrderedSet OrderedSet::intersection(const OrderedSet& other) const
{
  if (isEmpty() || other.isEmpty()) {
    return OrderedSet{};
  }

  if (_pieces == other._pieces) {
    return OrderedSet{_pieces};
  }

  return intersectionEpilogue(*this, other);
}

OrderedSet OrderedSet::cup(const OrderedSet& other) const &
{
  return OrderedSet{*this}.cup(other);
}

OrderedSet OrderedSet::cup(const OrderedSet& other) &&
{
  return std::move(*this).cup(OrderedSet{other});
}

OrderedSet OrderedSet::cup(OrderedSet&& other) const &
{
  return OrderedSet{*this}.cup(std::move(other));
}

OrderedSet OrderedSet::cup(OrderedSet&& other) &&
{
  if (isEmpty()) { 
    return std::move(other);
  }

  if (other.isEmpty() || _pieces == other._pieces) {
    return std::move(*this);
  }

  if (maxElem() < other.minElem()) {
    OrdMDICollection result = std::move(_pieces);
    result.insert(result.end(), std::make_move_iterator(other._pieces.begin())
      , std::make_move_iterator(other._pieces.end()));
    return OrderedSet{std::move(result)};
  }

  if (other.maxElem() < minElem()) {
    OrdMDICollection result = std::move(other._pieces);
    result.insert(result.end(), std::make_move_iterator(_pieces.begin())
      , std::make_move_iterator(_pieces.end()));
    return OrderedSet{std::move(result)};
  }

  // General case
  return std::move(other).disjointCup(difference(other));
}

OrderedSet OrderedSet::complementAtom() const
{
  OrdMDICollection result;

  MultiDimInter mdi = *_pieces.begin();
  MultiDimInter dense_mdi;
  for (const Interval& i : mdi) {
    dense_mdi.emplaceBack(i.begin(), 1, i.end());
  }
  MultiDimInter during_mdi = dense_mdi;

  Interval universe_one_dim{0, 1, Inf};
  MultiDimInter univ{mdi.arity(), universe_one_dim};

  std::size_t dim = 0;
  std::size_t global_position = 0;
  for (const Interval& i : mdi) {
    std::size_t local_pos = global_position; 
    // Before interval
    if (i.begin() != 0) {
      Interval i_res{0, 1, i.begin() - 1};
      if (!i_res.isEmpty()) {
        univ[dim] = i_res;
        result.insert(result.begin() + local_pos, univ);
        ++local_pos;
        ++global_position;
        univ[dim] = universe_one_dim;
      }
    }

    // "During" interval
    if (i.begin() < Inf && i.step() > 1) {
      for (unsigned int j = 0; j < i.step() - 1; ++j) {
        Interval i_res{i.begin() + j + 1, i.step(), i.end()};
        if (!i_res.isEmpty()) {
          during_mdi[dim] = i_res;
          result.insert(result.begin() + local_pos, during_mdi);
          ++local_pos;
        }
      }
    }

    // After interval
    if (i.end() < Inf) {
      Interval i_res{i.end() + 1, 1, Inf};
      if (!i_res.isEmpty()) {
        univ[dim] = i_res;
        result.insert(result.begin() + local_pos, univ);
        ++local_pos;
        univ[dim] = universe_one_dim;
      }
    }
    univ[dim] = dense_mdi[dim];
    during_mdi[dim] = i;
    ++dim;
  }

  return OrderedSet{std::move(result)};
}

void OrderedSet::intersectionComplement(const OrderedSet&
  local_complement, const MultiDimInter& mdi) 
{
  // Special cases
  if (isEmpty()) {
    return;
  }

  // General case

  // Values in the partial result "before" the current MDI all belong to the
  // complement of the processed MDIs, the current MDI and also to the remaining
  // MDIs, so there's no need to keep calculating with them.
  OrderedSet result;
  auto it = _pieces.begin();
  for (const MultiDimInter& elem : _pieces) {
    if (!(elem.maxElem() < mdi.minElem())) {
      break;
    }
    ++it;
  } 
  result._pieces.insert(result.end(), std::make_move_iterator(_pieces.begin())
    , std::make_move_iterator(it));

  NAT global_position = 0;
  for (; it != _pieces.end(); ++it) {
    const MultiDimInter& elem = *it;
    global_position = result.advanceHint(global_position, elem);
    // Discard elements in the complement of processed MDIs that belong to the
    // current MDI.
    if (overlap(elem, mdi)) {
      for (const MultiDimInter& local_elem : local_complement._pieces) {
        if (overlap(elem, local_elem)) {
          result.insertHint(global_position, elem.intersection(local_elem));
        }
      }
    } else {
      // Values in the partial result "after" the current MDI all belong to the
      // complement of the processed MDIs and the current MDI, but might not be
      // in the complement of the remaining MDIs, so they are kept for
      // subsequent calculations.
      result.insertHint(global_position, elem);
    }
  }
  _pieces = std::move(result._pieces);
}

OrderedSet OrderedSet::complement() const
{
  if (isEmpty()) {
    return OrderedSet{};
  }

  OrderedSet result = OrderedSet{_pieces.front()}.complementAtom();
  std::size_t j = 0;
  for (const MultiDimInter& mdi : _pieces) {
    if (j > 0) {
      OrderedSet c = OrderedSet{mdi}.complementAtom();
      result.intersectionComplement(c, mdi);
    }
    ++j;
  }

  return result;
}

OrderedSet OrderedSet::difference(const OrderedSet& other) const
{
  // Special cases
  if (isEmpty() || other.isEmpty()) {
    return OrderedSet{_pieces};
  }

  if (_pieces == other._pieces) {
    return OrderedSet{};
  }

  // General case
  return intersection(other.complement());
}

OrderedSet OrderedSet::cartesianProduct(const OrderedSet& other) const
{
  OrderedSet result;

  if (isEmpty() || other.isEmpty()) {
    return result;
  }

  for (const MultiDimInter& mdi : _pieces) {
    MultiDimInter mdi_copy = mdi;
    for (const MultiDimInter& other_mdi : other._pieces) {
      result.pushBack(mdi.cartesianProduct(other_mdi));
    }
  }

  return result;
}

// Extra operations ------------------------------------------------------------

std::size_t OrderedSet::arity() const
{
  if (isEmpty()) {
    return 0;
  }

  return _pieces.begin()->arity();
}

OrderedSet OrderedSet::disjointCup(const OrderedSet& other) const &
{
  return OrderedSet{*this}.disjointCup(other);
}

OrderedSet OrderedSet::disjointCup(const OrderedSet& other) &&
{
  return std::move(*this).disjointCup(OrderedSet{other});
}

OrderedSet OrderedSet::disjointCup(OrderedSet&& other) const &
{
  return OrderedSet{*this}.disjointCup(std::move(other));
}

OrderedSet OrderedSet::disjointCup(OrderedSet&& other) &&
{
  // Special cases
  if (isEmpty()) { 
    return std::move(other);
  }

  if (other.isEmpty()) {
    return std::move(*this);
  }

  OrdMDICollection result;
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

  return OrderedSet{std::move(result)};
}

OrderedSet OrderedSet::offset(const MD_NAT& off) const
{
  OrderedSet result;

  for (const MultiDimInter& mdi : _pieces) {
    result.pushBack(mdi.offset(off));
  }

  return result;
}

Perimeter OrderedSet::perimeter() const
{
  MD_NAT min;
  MD_NAT max;

  if (!isEmpty()) {
    std::size_t arity = this->arity();
    min = MD_NAT{arity, Inf};
    max = MD_NAT{arity, 0};
    for (const MultiDimInter& mdi : _pieces) {
      MD_NAT candidate_min = mdi.minElem();
      MD_NAT candidate_max = mdi.maxElem();
      for (size_t i = 0; i < arity; ++i) {
        min[i] = std::min(min[i], candidate_min[i]);
        max[i] = std::max(max[i], candidate_max[i]);
      }
    }
  }

  return Perimeter{min, max};
}

void OrderedSet::compact()
{
  using MDISet = std::set<MultiDimInter>;

  OrdMDICollection result;

  if (!isEmpty()) {
    MDISet set_result{std::make_move_iterator(_pieces.begin())
      , std::make_move_iterator(_pieces.end())};
    MDISet to_erase;
    do {
      MDISet new_set_result;
      to_erase.clear();

      MDISet::iterator ith = set_result.begin();
      MDISet::iterator last = set_result.end();
      for (; ith != last; ++ith) {
        MultiDimInter ith_compact = *ith;
        MDISet::iterator next = ith;
        ++next;
        for (; next != last; ++next) {
          MaybeMDI new_compact = ith_compact.compact(*next);
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

    for (const MultiDimInter& m : set_result) {
      result.push_back(m);
    }
  }

  _pieces = std::move(result);
}

} // namespace detail

} // namespace LIB

} // namespace SBG
