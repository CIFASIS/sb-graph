/** @file set.hpp

 @brief <b>SBG Set</b>

 A SBG Set is a structure that represents sets of multi-dimensional naturals
 (all with the same number of dimensions).
 Currently three compact implementations are supported:
   - UnorderedSet that keeps no order, but supports multi-dimensional values.
   - OrderedSet that supports multi-dimensional values while also keeping an
     internal order that enhances performance.
   - OrdUnidimDenseSet that can represent sets of one dimensional naturals and
     that is optimized by keeping an ordered collection of MDIs.

 <hr>

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

#ifndef SBGRAPH_SBG_SET_HPP_
#define SBGRAPH_SBG_SET_HPP_

#include "sbg/expression.hpp"
#include "sbg/fixed_points.hpp"
#include "sbg/interval.hpp"
#include "sbg/multidim_inter.hpp"
#include "sbg/natural.hpp"
#include "sbg/ord_set.hpp"
#include "sbg/ord_unidim_dense_set.hpp"
#include "sbg/set.hpp"
#include "sbg/unord_set.hpp"

#include <iosfwd>
#include <memory>
#include <variant>

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Set implementations ---------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

using SetImpl = std::variant<detail::UnorderedSet
  , detail::OrdUnidimDenseSet
  , detail::OrderedSet>;

class MapDetail;

}

enum class SetKind { kUnordered, kOrdered, kOrdUnidimDense };

std::ostream& operator<<(std::ostream& out, const SetKind kind);

////////////////////////////////////////////////////////////////////////////////
// Set -------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

class Set {
public:
  Set(const SetKind kind);
  Set(const SetKind kind, const MD_NAT& x);
  Set(const SetKind kind, MD_NAT&& x);
  Set(const SetKind kind, const NAT lo, const NAT st, const NAT hi);
  Set(const SetKind kind, const FixedPointsInfo& info);

  bool operator==(const Set& other) const;
  bool operator!=(const Set& other) const;
  std::ostream& print(std::ostream& out) const;

  // Traditional set operations ------------------------------------------------

  unsigned int cardinal() const;
  bool isEmpty() const;
  MD_NAT minElem() const;
  MD_NAT maxElem() const;
  Set intersection(const Set& other) const &;
  Set cup(const Set& other) const &;
  Set cup(const Set& other) &&;
  Set cup(Set&& other) const &;
  Set cup(Set&& other) &&;
  Set complement() const;
  Set difference(const Set& other) const;
  Set cartesianProduct(const Set& other) const;

  // Extra operations ----------------------------------------------------------

  std::size_t arity() const;
  Set disjointCup(const Set& other) const &;
  Set disjointCup(const Set& other) &&;
  Set disjointCup(Set&& other) const &;
  Set disjointCup(Set&& other) &&;
  Set offset(const MD_NAT& off) const;
  void compact();

private:
  Set(const detail::SetImpl& impl);
  Set(detail::SetImpl&& impl);

  detail::SetImpl _impl;

  friend class detail::MapDetail;
};

std::ostream& operator<<(std::ostream& out, const Set& s);

} // namespace lib

} // namespace sbg

#endif // SBGRAPH_SBG_SET_HPP_
