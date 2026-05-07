/** @file unord_set.hpp

 @brief <b>Unordered Set Implementation</b>

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

#ifndef SBGRAPH_SBG_UNORD_SET_HPP_
#define SBGRAPH_SBG_UNORD_SET_HPP_

#include "sbg/expression.hpp"
#include "sbg/fixed_points.hpp"
#include "sbg/interval.hpp"
#include "sbg/multidim_inter.hpp"
#include "sbg/natural.hpp"
#include "sbg/perimeter.hpp"

#include "rapidjson/document.h"

#include <iosfwd>
#include <memory>
#include <vector>

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Unordered Set Implementation ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

class UnorderedSet {
public:
  using MDIUnordCollection = std::vector<detail::MultiDimInter>;
  using ConstIt = MDIUnordCollection::const_iterator;

  UnorderedSet();
  UnorderedSet(const MD_NAT& x);
  UnorderedSet(const detail::Interval& i);
  UnorderedSet(const detail::MultiDimInter& mdi);
  UnorderedSet(const MDIUnordCollection& pieces);
  UnorderedSet(MDIUnordCollection&& pieces);
  UnorderedSet(const FixedPointsInfo& info);

  ConstIt begin() const;
  ConstIt end() const;

  void pushBack(const MultiDimInter& mdi);

  bool operator==(const UnorderedSet& other) const;
  bool operator!=(const UnorderedSet& other) const;
  std::ostream& print(std::ostream& out) const;

  // Traditional set operations ------------------------------------------------

  unsigned int cardinal() const;
  bool isEmpty() const;
  MD_NAT minElem() const;
  MD_NAT maxElem() const;
  UnorderedSet intersection(const UnorderedSet& other) const;
  UnorderedSet cup(const UnorderedSet& other) const &;
  UnorderedSet cup(UnorderedSet&& other) const &;
  UnorderedSet cup(const UnorderedSet& other) &&;
  UnorderedSet cup(UnorderedSet&& other) &&;
  UnorderedSet complement() const;
  UnorderedSet difference(const UnorderedSet& other) const;
  UnorderedSet cartesianProduct(const UnorderedSet& other) const;

  // Extra operations ----------------------------------------------------------

  std::size_t arity() const;
  UnorderedSet disjointCup(const UnorderedSet& other) const &;
  UnorderedSet disjointCup(const UnorderedSet& other) &&;
  UnorderedSet disjointCup(UnorderedSet&& other) const &;
  UnorderedSet disjointCup(UnorderedSet&& other) &&;
  UnorderedSet offset(const MD_NAT& off) const;
  Perimeter perimeter() const;
  void compact();

private:
  /**
   * @brief Calculate the complement of an unordered set with a single piece.
   */
  UnorderedSet complementAtom() const;

  MDIUnordCollection _pieces;

  friend class SetAccessKey;
};

// Non-member functions --------------------------------------------------------

rapidjson::Value toJSON(UnorderedSet s
  , rapidjson::Document::AllocatorType& alloc);

} // namespace detail

} // namespace LIB

}  // namespace SBG

#endif // SBGRAPH_SBG_UNORD_SET_HPP_
