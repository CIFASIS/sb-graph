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

#include "sbg/unord_set.hpp"

#include <iostream>
#include <optional>
#include <set>

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Auxiliary functions ---------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

bool overlap(const UnorderedSet& lhs, const UnorderedSet& rhs)
{
  return rhs.minElem() < lhs.maxElem() || lhs.minElem() < rhs.maxElem();
}

////////////////////////////////////////////////////////////////////////////////
// Unordered Set Implementation ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

// Constructors/Destructors ----------------------------------------------------

UnorderedSet::UnorderedSet() : _pieces() {}

UnorderedSet::UnorderedSet(const IntTuple& x) : _pieces()
{
  _pieces.emplace_back(MultiDimInter{x});
}

UnorderedSet::UnorderedSet(const Interval& i) : _pieces()
{
  if (!i.isEmpty()) {
    _pieces.emplace_back(MultiDimInter{i});
  }
}

UnorderedSet::UnorderedSet(const MultiDimInter& mdi) : _pieces()
{
  if (!mdi.isEmpty()) {
    _pieces.push_back(mdi);
  }
}

UnorderedSet::UnorderedSet(const UnorderedSet::MDIUnordCollection& pieces)
  : _pieces(pieces) {}

UnorderedSet::UnorderedSet(UnorderedSet::MDIUnordCollection&& pieces)
  : _pieces(std::move(pieces)) {}

UnorderedSet::UnorderedSet(const FixedPointsInfo& info) : _pieces()
{
  if (info) {
    std::vector<Solution> solutions = info.value();
    MultiDimInter result_mdi;
    for (const Solution& jth_solution : solutions) {
      if (jth_solution.kind() == SolutionKind::kFixed) {
        result_mdi.pushBack(jth_solution.value().value());
      } else if (jth_solution.kind() == SolutionKind::kFree) {
        result_mdi.pushBack(kOneDimUniverse);
      }
    }
    pushBack(result_mdi);
  }
}

// Getters ---------------------------------------------------------------------

UnorderedSet::ConstIt UnorderedSet::begin() const { return _pieces.begin(); }

UnorderedSet::ConstIt UnorderedSet::end() const { return _pieces.end(); }

// Setters ---------------------------------------------------------------------

void UnorderedSet::pushBack(const MultiDimInter& mdi)
{
  if (mdi.isEmpty()) {
    return;
  }

  _pieces.push_back(mdi);
}

// Operators -------------------------------------------------------------------

bool UnorderedSet::operator==(const UnorderedSet& other) const
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

bool UnorderedSet::operator!=(const UnorderedSet& other) const
{
  return !(*this == other);
}

std::ostream& UnorderedSet::print(std::ostream& out) const
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

unsigned int UnorderedSet::cardinal() const
{
  unsigned int result = 0;

  for (const MultiDimInter& mdi : _pieces) {
    result += mdi.cardinal();
  }

  return result;
}

bool UnorderedSet::isEmpty() const { return _pieces.empty(); }

IntTuple UnorderedSet::minElem() const
{
  IntTuple result = _pieces.begin()->minElem();

  for (const MultiDimInter& mdi : _pieces) {
    result = std::min(result, mdi.minElem());
  }
  
  return result;
}

IntTuple UnorderedSet::maxElem() const
{
  IntTuple result = _pieces.begin()->maxElem();

  for (const MultiDimInter& mdi : _pieces) {
    result = std::max(result, mdi.maxElem());
  }
  
  return result;
}

UnorderedSet intersectionEpilogue(const UnorderedSet::MDIUnordCollection& lhs
  , const UnorderedSet::MDIUnordCollection& rhs)
{
  // General case
  UnorderedSet result;
  for (const MultiDimInter& mdi1 : lhs) {
    for (const MultiDimInter& mdi2 : rhs) {
      result.pushBack(mdi1.intersection(mdi2));
    }
  }

  return result;
}

UnorderedSet UnorderedSet::intersection(const UnorderedSet& other) const
{
  if (isEmpty() || other.isEmpty()) {
    return UnorderedSet{};
  }

  if (_pieces == other._pieces) {
    return UnorderedSet{_pieces};
  }

  return intersectionEpilogue(_pieces, other._pieces);
}

UnorderedSet UnorderedSet::cup(const UnorderedSet& other) const &
{
  return UnorderedSet{*this}.cup(other);
}

UnorderedSet UnorderedSet::cup(const UnorderedSet& other) &&
{
  return std::move(*this).cup(UnorderedSet{other});
}

UnorderedSet UnorderedSet::cup(UnorderedSet&& other) const &
{
  return UnorderedSet{*this}.cup(std::move(other));
}

UnorderedSet UnorderedSet::cup(UnorderedSet&& other) &&
{
  if (isEmpty()) { 
    return std::move(other);
  }

  if (other.isEmpty() || _pieces == other._pieces) {
    return std::move(*this);
  }

  if (maxElem() < other.minElem() || other.maxElem() < minElem()) {
    MDIUnordCollection result = std::move(_pieces);
    result.insert(result.end(), std::make_move_iterator(other._pieces.begin())
      , std::make_move_iterator(other._pieces.end()));
    return UnorderedSet{std::move(result)};
  }

  // General case
  return std::move(other).disjointCup(difference(other));
}

UnorderedSet UnorderedSet::complementAtom() const
{
  MDIUnordCollection result;

  MultiDimInter mdi = *_pieces.begin();
  MultiDimInter dense_mdi;
  for (const Interval& i : mdi) {
    dense_mdi.emplaceBack(i.begin(), 1, i.end());
  }
  MultiDimInter during_mdi = dense_mdi;

  MultiDimInter univ{mdi.arity(), kOneDimUniverse};

  std::size_t dim = 0;
  for (const Interval& i : mdi) {
    // Before interval
    if (i.begin() != kNegInf) {
      Interval i_res{0, 1, i.begin() - 1};
      if (!i_res.isEmpty()) {
        univ[dim] = i_res;
        result.push_back(univ);
        univ[dim] = kOneDimUniverse;
      }
    }

    // "During" interval
    if (i.begin() < kPosInf && i.step() > 1) {
      for (unsigned int j = 0; j < i.step() - 1; ++j) {
        Interval i_res{i.begin() + j + 1, i.step(), i.end()};
        if (!i_res.isEmpty()) {
          during_mdi[dim] = i_res;
          result.push_back(during_mdi);
        }
      }
    }

    // After interval
    if (i.end() < kPosInf) {
      Interval i_res{i.end() + 1, 1, kPosInf};
      if (!i_res.isEmpty()) {
        univ[dim] = i_res;
        result.push_back(univ);
        univ[dim] = kOneDimUniverse;
      }
    }
    univ[dim] = dense_mdi[dim];
    during_mdi[dim] = i;
    ++dim;
  }

  return UnorderedSet{std::move(result)};
}

UnorderedSet UnorderedSet::complement() const
{
  if (isEmpty()) {
    return UnorderedSet{};
  }

  UnorderedSet result = UnorderedSet{_pieces.front()}.complementAtom();
  std::size_t j = 0;
  for (const MultiDimInter& mdi : _pieces) {
    if (j > 0) {
      UnorderedSet c = UnorderedSet{mdi}.complementAtom();
      result = result.intersection(c);
    }
    ++j;
  }

  return result;
}

UnorderedSet UnorderedSet::difference(const UnorderedSet& other) const
{
  // Special cases
  if (isEmpty() || other.isEmpty()) {
    return UnorderedSet{_pieces};
  }

  if (_pieces == other._pieces) {
    return UnorderedSet{};
  }

  // General case
  return intersection(other.complement());
}

UnorderedSet UnorderedSet::cartesianProduct(const UnorderedSet& other) const
{
  UnorderedSet result;

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

std::size_t UnorderedSet::arity() const
{
  if (isEmpty()) {
    return 0;
  }

  return _pieces.begin()->arity();
}

UnorderedSet UnorderedSet::disjointCup(const UnorderedSet& other) const &
{
  return UnorderedSet{*this}.disjointCup(other);
}

UnorderedSet UnorderedSet::disjointCup(const UnorderedSet& other) &&
{
  return std::move(*this).disjointCup(UnorderedSet{other});
}

UnorderedSet UnorderedSet::disjointCup(UnorderedSet&& other) const &
{
  return UnorderedSet{*this}.disjointCup(std::move(other));
}

UnorderedSet UnorderedSet::disjointCup(UnorderedSet&& other) &&
{
  if (isEmpty()) { 
    return std::move(other);
  }

  if (other.isEmpty()) {
    return std::move(*this);
  }

  MDIUnordCollection result;
  result.insert(result.end(), std::make_move_iterator(_pieces.begin())
    , std::make_move_iterator(_pieces.end()));
  result.insert(result.end(), std::make_move_iterator(other._pieces.begin())
    , std::make_move_iterator(other._pieces.end()));

  return UnorderedSet{std::move(result)};
}

UnorderedSet UnorderedSet::translate(const IntTuple& t) const
{
  UnorderedSet result;

  for (const MultiDimInter& mdi : _pieces) {
    result.pushBack(mdi.translate(t));
  }

  return result;
}

Perimeter UnorderedSet::perimeter() const
{
  IntTuple min;
  IntTuple max;

  if (!isEmpty()) {
    std::size_t arity = this->arity();
    min = IntTuple{arity, kPosInf};
    max = IntTuple{arity, kNegInf};
    for (const MultiDimInter& mdi : _pieces) {
      IntTuple candidate_min = mdi.minElem();
      IntTuple candidate_max = mdi.maxElem();
      for (size_t i = 0; i < arity; ++i) {
        min[i] = std::min(min[i], candidate_min[i]);
        max[i] = std::max(max[i], candidate_max[i]);
      }
    }
  }

  return Perimeter{min, max};
}

void UnorderedSet::compact()
{
  using MDISet = std::set<MultiDimInter>;

  MDIUnordCollection result;

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

// Non-member functions --------------------------------------------------------

rapidjson::Value toJSON(UnorderedSet s
  , rapidjson::Document::AllocatorType& alloc) 
{
  rapidjson::Value mdi_array{rapidjson::kArrayType};
  for (const MultiDimInter& mdi : s) {
    rapidjson::Value jth = detail::toJSON(mdi, alloc);
    mdi_array.PushBack(jth, alloc);
  }
  rapidjson::Value result{rapidjson::kObjectType};
  result.AddMember("pieces", mdi_array, alloc);

  return result;
}

} // namespace detail

} // namespace LIB

} // namespace SBG
