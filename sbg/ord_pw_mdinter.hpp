/** @file ord_pw_mdinter.hpp

 @brief <b>Piecewise uni-dimensional interval implementation</b>

 A piecewise unidim interval is a collection of non-empty disjoint intervals, and its 
 corresponding set is the union of all composing intervals. The constructors 
 aren't safe as they don't check if intervals are disjoint and non-empty.

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

#ifndef SBG_ORD_PW_MDINTERVAL_HPP
#define SBG_ORD_PW_MDINTERVAL_HPP

#include <boost/container/flat_set.hpp>

#include "sbg/multidim_inter.hpp"

namespace SBG {

namespace LIB {

// Container -------------------------------------------------------------------

/**
 * @brief Ordered collection of multi-dimensional intervals.
 */

/* @struct LTMDInter
 *
 * @brief This function is defined to be used by the MDMDInterOrdSet definition.
 * The two inputs are disjoint (that's why the operator<, that compares any sort
 * of SetPieces, is not used).
 */
struct LTMDInter {
  LTMDInter();
   
  bool operator()(const SetPiece &x, const SetPiece &y) const; 
};

typedef boost::container::flat_set<
  SetPiece, LTMDInter, boost::container::new_allocator<SetPiece>
> MDInterOrdSet;
typedef MDInterOrdSet::iterator MDInterOrdSetIt;
typedef MDInterOrdSet::const_iterator MDInterOrdSetConstIt;
std::ostream &operator<<(std::ostream &out, const MDInterOrdSet &ii);

struct OrdPWMDInter {
  member_class(MDInterOrdSet, pieces);

  /**
   * @brief Constructors don't check if intervals are disjoint (performance).
   */
  OrdPWMDInter();
  OrdPWMDInter(Util::MD_NAT x);
  OrdPWMDInter(Interval i);
  OrdPWMDInter(SetPiece mdi);
  OrdPWMDInter(MDInterOrdSet container);

  typedef MDInterOrdSetIt iterator;
  typedef MDInterOrdSetConstIt const_iterator;
  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;
  std::size_t size() const;
  void emplace(SetPiece mdi);
  void emplaceBack(SetPiece mdi);

  bool operator==(const OrdPWMDInter &other) const;
  bool operator!=(const OrdPWMDInter &other) const;
  bool operator<(const OrdPWMDInter &other) const;

  /**
   * @brief Traditional set operations.
   */
  unsigned int cardinal() const;
  bool isEmpty() const;
  Util::MD_NAT minElem() const;
  Util::MD_NAT maxElem() const;
  OrdPWMDInter intersection(const OrdPWMDInter &other) const;
  OrdPWMDInter cup(const OrdPWMDInter &other) const;
  OrdPWMDInter difference(const OrdPWMDInter &other) const;

  /**
   * @brief Extra operations.
   */

  /** @function concatenation
   *
   * @brief Function useful to unite two pwis in the case these are known to be
   * disjoint.
   */
  OrdPWMDInter concatenation(const OrdPWMDInter &other) const;
  OrdPWMDInter filterSet(bool (*f)(const SetPiece &mdi)) const;
  OrdPWMDInter offset(Util::MD_NAT off) const;
  OrdPWMDInter compact() const;

  private:
  /** @function complementAtom
   *
   * @brief This function calculates the complement for an OrdPWMDInter with only
   * one piece.
   *
   * !!! Both complementAtom and complement return a collection of non-disjoint
   * pieces, because they are only used by difference. These functions should
   * NOT be called by any other function.
   */
  OrdPWMDInter complementAtom() const;
  OrdPWMDInter complement() const;

  /** @function isDense
   *
   * @brief This function determines if a pwi is compact (composed only by compact
   * intervals).
   */
  bool isDense() const;

  /** @function optCond
   *
   * @brief Check if the MDInterOrdSet satisfies the conditions to use
   * optimizations.
   */
  bool optConds() const;

  /** @function boundedTraverse
   *
   * @brief Traverse pwis in order until one reaches its end, obtaining an ordered
   * result.
   */
  MDInterOrdSet boundedTraverse(SetPiece (SetPiece::*f)(const SetPiece &) const
                                , const OrdPWMDInter &other) const;

  /** @function traverse
   *
   * @brief Traverse pwis in order reaching both ends, obtaining an ordered result.
   *   !!! Both pwis should be compact (not checked in this function).
   */
  MDInterOrdSet traverse(SetPiece (SetPiece::*f)(const SetPiece &) const
                         , const OrdPWMDInter &other) const;
};
std::ostream &operator<<(std::ostream &out, const OrdPWMDInter &i);

std::size_t hash_value(const OrdPWMDInter &pwi);

typedef OrdPWMDInter OrdSet;

} // namespace LIB

}  // namespace SBG

#endif
