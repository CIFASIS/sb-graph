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

#include <iostream>

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
 * @brief This function is defined to be used by the MDMDInterOrdSet definition. The
 * two inputs are disjoint (that's why the operator<, that compares any sort of
 * SetPieces, is not used).
 */
struct LTMDInter {
  LTMDInter();
   
  bool operator()(const SetPiece &x, const SetPiece &y) const; 
};

typedef boost::container::flat_set<SetPiece, LTMDInter, boost::container::new_allocator<SetPiece>> MDInterOrdSet;
typedef MDInterOrdSet::iterator MDInterOrdSetIt;
typedef MDInterOrdSet::const_iterator MDInterOrdSetConstIt;
std::ostream &operator<<(std::ostream &out, const MDInterOrdSet &ii);

struct OrdPWMDInter {
  typedef MDInterOrdSetIt iterator;
  typedef MDInterOrdSetConstIt const_iterator;

  member_class(MDInterOrdSet, pieces);

  /**
   * @brief Constructors don't check if intervals are disjoint (performance).
   */
  OrdPWMDInter();
  OrdPWMDInter(Interval i);
  OrdPWMDInter(SetPiece mdi);
  OrdPWMDInter(MDInterOrdSet pieces);

  std::size_t size();
  void emplace(SetPiece mdi);
  void emplace_hint(MDInterOrdSetIt it, SetPiece mdi);
  iterator begin();
  iterator end();
  SetPiece operator[](std::size_t n);

  eq_class(OrdPWMDInter);
  lt_class(OrdPWMDInter);
};
std::ostream &operator<<(std::ostream &out, const OrdPWMDInter &i);

/**
 * @brief Traditional set operations.
 */

unsigned int cardinal(OrdPWMDInter i);
bool isEmpty(OrdPWMDInter i);
bool isMember(MD_NAT x, OrdPWMDInter i);
Util::MD_NAT minElem(OrdPWMDInter pwi);
Util::MD_NAT maxElem(OrdPWMDInter pwi);
OrdPWMDInter intersection(OrdPWMDInter pwi1, OrdPWMDInter pwi2);
OrdPWMDInter cup(OrdPWMDInter i1, OrdPWMDInter i2);

/** @function complementAtom
 *
 * @brief This function calculates the complement for an OrdPWMDInter with only
 * one piece.
 *
 * !!! Both complementAtom and complement return a collection of non-disjoint
 * pieces, because they are only used by difference. These functions should
 * NOT be called by any other function.
 */

OrdPWMDInter complementAtom(OrdPWMDInter pwi);
OrdPWMDInter complement(OrdPWMDInter i);

OrdPWMDInter difference(OrdPWMDInter i1, OrdPWMDInter i2);

/**
 * @brief Extra operations.
 */

/** @function isCompact
 *
 * @brief This function determines if a pwi is compact (composed only by compact 
 * intervals).
 */
bool isCompact(MDInterOrdSet ii);

/** @function optCond
 *
 * @brief Check if the MDInterOrdSet satisfies the conditions to use optimizations.
 */
bool optConds(MDInterOrdSet ii);

/* @function canonize
 *
 * @brief Tries to convert the argument to canonical form. Currently works only
 * with compact MDInterOrdSet. 
 */
MDInterOrdSet canonize(MDInterOrdSet ii);

/** @function concatenation
 *
 * @brief Function useful to unite two pwis in the case these are known to be
 * disjoint.
 */
OrdPWMDInter concatenation(OrdPWMDInter pwi1, OrdPWMDInter pwi2);

OrdPWMDInter filterSet(bool (*f)(SetPiece), OrdPWMDInter pwi);

/** @function boundedTraverse
 *
 * @brief Traverse pwis in order until one reaches its end, obtaining an ordered
 * result.
 */
MDInterOrdSet boundedTraverse(OrdPWMDInter pwi1, OrdPWMDInter pwi2, SetPiece (*func)(SetPiece, SetPiece));

/** @function traverse
 *
 * @brief Traverse pwis in order reaching both ends, obtaining an ordered result.
 *   !!! Both pwis should be compact (not checked in this function).
 */
MDInterOrdSet traverse(OrdPWMDInter pwi1, OrdPWMDInter pwi2, SetPiece (*func)(SetPiece, SetPiece));

std::size_t hash_value(const OrdPWMDInter &pwi);

typedef OrdPWMDInter OrdSet;

} // namespace LIB

}  // namespace SBG

#endif
