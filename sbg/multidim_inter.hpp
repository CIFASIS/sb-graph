/** @file multidim_inter.hpp

 @brief <b>Multi-dimensional interval implementation</b>

 A Multi-dimensional interval (mdi) i1 x ... x ik is the resulting set of the
 cartesian product of intervals. Thus, the implementation consists of an
 ordered collection of intervals.

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

#ifndef SBGRAPH_SBG_MULTIDIM_INTER_HPP_
#define SBGRAPH_SBG_MULTIDIM_INTER_HPP_

#include "sbg/interval.hpp"
#include "sbg/natural.hpp"
#include "sbg/perimeter.hpp"

#include "rapidjson/document.h"

#include <iosfwd>
#include <optional>

namespace SBG {

namespace LIB {

namespace detail {

class MultiDimInter;

using MaybeMDI = std::optional<MultiDimInter>;

class MultiDimInter {
public:
  using InterVector = std::vector<Interval>;
  using ConstIt = InterVector::const_iterator;

  /**
   * @brief Construct zero-dimensional mdi.
   */
  MultiDimInter();

  /**
   * @brief Construct a mdi with a single element \p x. 
   */
  MultiDimInter(const MD_NAT& x);

  /**
   * @brief Construct a one-dimensional mdi with the same elements as \p i.
   */
  MultiDimInter(const Interval& i);

  /**
   * @brief Construct a mdi that is the result of \p i ^ \p n.
   */
  MultiDimInter(const std::size_t k, const Interval& i);

  MultiDimInter(InterVector iv);

  ConstIt begin() const;
  ConstIt end() const;

  template<typename... Args>
  void emplaceBack(Args&&... args);
  void pushBack(const Interval& i);

  Interval& operator[](std::size_t n);
  const Interval& operator[](std::size_t n) const;
  bool operator==(const MultiDimInter& other) const;
  bool operator!=(const MultiDimInter& other) const;

  /**
   * @brief A mdi mdi1 is less than another mdi2 iff min(mdi1) < min(mdi2).
   * This operation is later needed to implement ordered sets.
   */ 
  bool operator<(const MultiDimInter& other) const;
  bool operator>(const MultiDimInter& other) const;

  // Traditional set operations ------------------------------------------------

  /**
   * @brief Number of elements contained in the mdi, i.e.
   * cardinal([1:1:10]x[1:1:10]) = 100. 
   */
  unsigned int cardinal() const;
  bool isEmpty() const;
  MD_NAT minElem() const;
  MD_NAT maxElem() const;
  MultiDimInter intersection(const MultiDimInter& other) const;
  MultiDimInter cartesianProduct(const MultiDimInter& other) const;

  // Extra operations ----------------------------------------------------------

  /**
   * @brief Number of dimensions of the elements that compose the mdi. For
   * example, arity([1:1:10]x[1:1:10]) = 2.
   */
  std::size_t arity() const;

  /**
   * @brief Sum a constant value to every element of the mdi.
   */
  MultiDimInter offset(const MD_NAT& off) const;

  /**
   * @brief Operation that given two disjoint mdis returns the lesser one.
   * It will be used by ordered sets operations.
   */
  MultiDimInter least(const MultiDimInter& other) const;

  Perimeter perimeter() const;

  /**
   * @brief Merge two contiguous mdis if possible. If not, then the result is
   * not an mdi, so no value is returned.
   */
  MaybeMDI compact(const MultiDimInter& other) const;

private: 
  InterVector _intervals;
};
std::ostream& operator<<(std::ostream& out, const MultiDimInter& mdi);

// Template definitions --------------------------------------------------------

template<typename... Args>
inline void MultiDimInter::emplaceBack(Args&&... args)
{
  _intervals.emplace_back(std::forward<Args>(args)...);
}

// Non-member functions --------------------------------------------------------

rapidjson::Value toJSON(MultiDimInter mdi
  , rapidjson::Document::AllocatorType& alloc);

} // namespace detail

} // namespace LIB

}  // namespace SBG

#endif // SBGRAPH_SBG_MULTIDIM_INTER_HPP_
