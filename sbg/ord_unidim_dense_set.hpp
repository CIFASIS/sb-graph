/** @file ord_unidim_dense_set.hpp

 @brief <b>Ordered Unidimensional Dense Set Implementation</b>

 A dense set is composed only by dense intervals, i.e. intervals whose step is
 equal to one.

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

#ifndef SBGRAPH_SBG_ORD_UNIDIM_DENSE_SET_HPP_
#define SBGRAPH_SBG_ORD_UNIDIM_DENSE_SET_HPP_

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
// Ordered Unidimensional Dense Set Implementation -----------------------------
////////////////////////////////////////////////////////////////////////////////

class OrdUnidimDenseSet {
public:
  using OrdIntervalCollection = std::vector<Interval>;
  using ConstIt = OrdIntervalCollection::const_iterator;

  OrdUnidimDenseSet();
  OrdUnidimDenseSet(const Int x);
  OrdUnidimDenseSet(const detail::Interval& i);
  OrdUnidimDenseSet(const OrdIntervalCollection& pieces);
  OrdUnidimDenseSet(OrdIntervalCollection&& pieces);
  OrdUnidimDenseSet(const FixedPointsInfo& info);

  ConstIt begin() const;
  ConstIt end() const;

  void pushBack(const Interval& mdi);

  bool operator==(const OrdUnidimDenseSet& other) const;
  bool operator!=(const OrdUnidimDenseSet& other) const;
  std::ostream& print(std::ostream& out) const;

  // Traditional set operations ------------------------------------------------

  unsigned int cardinal() const;
  bool isEmpty() const;
  IntTuple minElem() const;
  IntTuple maxElem() const;
  OrdUnidimDenseSet intersection(const OrdUnidimDenseSet& other) const;
  OrdUnidimDenseSet cup(const OrdUnidimDenseSet& other) const &;
  OrdUnidimDenseSet cup(const OrdUnidimDenseSet& other) &&;
  OrdUnidimDenseSet cup(OrdUnidimDenseSet&& other) const &;
  OrdUnidimDenseSet cup(OrdUnidimDenseSet&& other) &&;
  OrdUnidimDenseSet complement() const;
  OrdUnidimDenseSet difference(const OrdUnidimDenseSet& other) const;
  OrdUnidimDenseSet cartesianProduct(const OrdUnidimDenseSet& other) const;

  // Extra operations ----------------------------------------------------------

  std::size_t arity() const;
  OrdUnidimDenseSet disjointCup(const OrdUnidimDenseSet& other) const &;
  OrdUnidimDenseSet disjointCup(const OrdUnidimDenseSet& other) &&;
  OrdUnidimDenseSet disjointCup(OrdUnidimDenseSet&& other) const &;
  OrdUnidimDenseSet disjointCup(OrdUnidimDenseSet&& other) &&;
  OrdUnidimDenseSet translate(const IntTuple& t) const;
  Perimeter perimeter() const;
  void compact();

private:
  /**
   * @brief Performs operation f between a piece of s1 and a piece of s2. At the
   * start it begins with both minimum elements, and advances the iterator of
   * the set with the piece that has the minimum end. This is repeated until one
   * of the two collections is consumed.
   */
   OrdUnidimDenseSet boundedTraverse(Interval f(const Interval&, const Interval&)
     , const OrdUnidimDenseSet& other) const;

  /**
   * @brief Performs operation f between a piece of s1 and a piece of s2. At the
   * start it begins with both minimum elements, and advances the iterator of
   * the set with the piece that has the minimum end. This is repeated until one
   * of the two collections is consumed. Then, all the remaining pieces of the
   * other set are also inserted.
   */
  OrdUnidimDenseSet traverse(Interval f(const Interval&, const Interval&)
    , const OrdUnidimDenseSet& other) const;

  OrdIntervalCollection _pieces;

  friend class SetAccessKey;
};

// Non-member definitions ------------------------------------------------------

rapidjson::Value toJSON(OrdUnidimDenseSet s
  , rapidjson::Document::AllocatorType& alloc);

} // namespace detail

} // namespace LIB

}  // namespace SBG

#endif // SBGRAPH_SBG_ORD_UNIDIM_DENSE_SET_HPP_
