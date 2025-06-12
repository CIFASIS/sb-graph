/** @file multidim_inter.hpp

 @brief <b>Multi-dimensional interval implementation</b>

 A Multi-dimensional interval (mdi) i1 x ... x ik is the resulting set of the
 cartesian product of intervals. As such the implementations an ordered
 collection of intervals. 

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

#ifndef SBG_MULTIDIM_INTERVAL_HPP
#define SBG_MULTIDIM_INTERVAL_HPP

#include <iostream>
#include "sbg/interval.hpp"



namespace SBG {

namespace LIB {

typedef std::vector<Interval> InterVector;
typedef InterVector::iterator InterVectorIt;
typedef InterVector::const_iterator InterVectorConstIt;

struct MultiDimInter;

typedef std::optional<MultiDimInter> MaybeMDI;

struct MultiDimInter {
  member_class(InterVector, intervals);

  /**
   * @brief Construct zero-dimensional mdi.
   */
  MultiDimInter();

  /**
   * @brief Construct a mdi with a single element \p x. 
   */
  MultiDimInter(const MD_NAT &x);

  /**
   * @brief Construct a one-dimensional mdi with the same elements as \p i.
   */
  MultiDimInter(const Interval &i);

  /**
   * @brief Construct a mdi that is the result of \p i ^ \p nmbr_copies.
   */
  MultiDimInter(const unsigned int &nmbr_copies, const Interval &i);

  /**
   * @brief Collection move constructor.
   */
  MultiDimInter(const InterVector &iv);

  typedef InterVectorIt iterator;
  typedef InterVectorConstIt const_iterator;
  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;
  void emplaceBack(Interval i);
  Interval &operator[](std::size_t n);
  const Interval &operator[](std::size_t n) const;

  bool operator==(const MultiDimInter &other) const;
  bool operator!=(const MultiDimInter &other) const;

  /**
   * @brief A mdi mdi1 is less than another mdi2 iff min(mdi1) < min(mdi2).
   * This operation is later needed to implement ordered sets.
   */ 
  bool operator<(const MultiDimInter &other) const;

  // Traditional set operations ------------------------------------------------

  /**
   * @brief Number of elements contained in the mdi, i.e.
   * cardinal([1:1:10]x[1:1:10]) = 100. 
   */
  unsigned int cardinal() const;
  bool isEmpty() const;
  MD_NAT minElem() const;
  MD_NAT maxElem() const;
  MultiDimInter intersection(const MultiDimInter &other) const;

  // Extra operations ----------------------------------------------------------

  /**
   * @brief Number of dimensions of the elements that compose the mdi. For
   * example, arity([1:1:10]x[1:1:10]) = 2.
   */
  std::size_t arity() const;

  /**
   * @brief Sum a constant value to every element of the mdi.
   */
  MultiDimInter offset(const MD_NAT &off) const;

  /**
   * @brief Operation that given two disjoint mdis returns the lesser one.
   * It will be used by ordered sets operations.
   */
  MultiDimInter least(const MultiDimInter &other) const;

  /**
   * @brief Merge two contiguous mdis if possible. If not, then the result is
   * not an mdi, so no value is returned.
   */
  MaybeMDI compact(const MultiDimInter &other) const;
  
};
std::ostream &operator<<(std::ostream &out, const MultiDimInter &mi);

typedef MultiDimInter SetPiece;

} // namespace LIB

}  // namespace SBG

#endif
