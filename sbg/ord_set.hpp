/** @file ord_set.hpp

 @brief <b>Ordered Set Implementation</b>

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

#ifndef SBGRAPH_SBG_ORD_SET_HPP_
#define SBGRAPH_SBG_ORD_SET_HPP_

#include "sbg/fixed_points.hpp"
#include "sbg/integer.hpp"
#include "sbg/interval.hpp"
#include "sbg/multidim_inter.hpp"
#include "sbg/perimeter.hpp"

#include "rapidjson/document.h"

#include <iosfwd>
#include <memory>
#include <vector>

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Ordered Set Implementation --------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

class OrderedSet {
public:
  using OrdMDICollection = std::vector<detail::MultiDimInter>;
  using ConstIt = OrdMDICollection::const_iterator;

  OrderedSet();
  OrderedSet(const IntTuple& x);
  OrderedSet(const detail::Interval& i);
  OrderedSet(const detail::MultiDimInter& mdi);
  OrderedSet(const OrdMDICollection& pieces);
  OrderedSet(OrdMDICollection&& pieces);
  OrderedSet(const FixedPointsInfo& info);

  ConstIt begin() const;
  ConstIt end() const;

  void pushBack(const MultiDimInter& mdi);

  bool operator==(const OrderedSet& other) const;
  bool operator!=(const OrderedSet& other) const;
  std::ostream& print(std::ostream& out) const;

  // Traditional set operations ------------------------------------------------

  unsigned int cardinal() const;
  bool isEmpty() const;
  IntTuple minElem() const;
  IntTuple maxElem() const;
  OrderedSet intersection(const OrderedSet& other) const;
  OrderedSet cup(const OrderedSet& other) const &;
  OrderedSet cup(OrderedSet&& other) const &;
  OrderedSet cup(const OrderedSet& other) &&;
  OrderedSet cup(OrderedSet&& other) &&;
  OrderedSet complement() const;
  OrderedSet difference(const OrderedSet& other) const;
  OrderedSet cartesianProduct(const OrderedSet& other) const;

  // Extra operations ----------------------------------------------------------

  std::size_t arity() const;
  OrderedSet disjointCup(const OrderedSet& other) const &;
  OrderedSet disjointCup(const OrderedSet& other) &&;
  OrderedSet disjointCup(OrderedSet&& other) const &;
  OrderedSet disjointCup(OrderedSet&& other) &&;
  OrderedSet translate(const IntTuple& t) const;
  Perimeter perimeter() const;
  void compact();

private:
  Int advanceHint(Int hint, const MultiDimInter& mdi);
  void insertHint(const Int hint, const MultiDimInter& mdi);

  OrderedSet intersectionEpilogue(const OrderedSet& lhs, const OrderedSet& rhs)
    const;

  /**
   * @brief Computes the accumulated complement between an ordered set (this), 
   * which represents the complement of an ordered set, and an ordered set
   * (other), which represents the complement of an atomic ordered set.
   */
  void intersectionComplement(const OrderedSet& other
    , const MultiDimInter& mdi);

  /**
   * @brief Calculate the complement of an unordered set with a single piece.
   */
  OrderedSet complementAtom() const;

  OrdMDICollection _pieces;

  friend class SetAccessKey;
};

// Non-member functions --------------------------------------------------------

rapidjson::Value toJSON(OrderedSet s
  , rapidjson::Document::AllocatorType& alloc);

} // namespace detail

} // namespace LIB

}  // namespace SBG

#endif // SBGRAPH_SBG_ORD_SET_HPP_
