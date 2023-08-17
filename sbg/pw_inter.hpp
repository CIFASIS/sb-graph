/** @file pw_inter.hpp

 @brief <b>Piecewise interval implementation</b>

 A piecewise interval is a collection of non-empty disjoint intervals, and its 
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

#ifndef SBG_PW_INTERVAL_HPP
#define SBG_PW_INTERVAL_HPP

#include <boost/container/flat_set.hpp>

#include "sbg/interval.hpp"

namespace SBG {

namespace LIB {

/**
 * @brief Ordered collection of intervals.
 */

/* @struct LTInter
 *
 * @brief This function is defined to be used by the InterSet definition. The
 * two inputs are disjoint (that's why the operator<, that compares any sort of
 * SetPieces, is not used).
 */
struct LTInter {
  LTInter();

  bool operator()(const SetPiece &x, const SetPiece &y) const;
};

typedef boost::container::flat_set<SetPiece, LTInter, boost::container::new_allocator<SetPiece>> InterSet;
typedef InterSet::iterator InterSetIt;
std::ostream &operator<<(std::ostream &out, const InterSet &ii);

struct PWInterval {
  member_class(InterSet, pieces);

  /**
   * @brief Constructors don't check if intervals are disjoint (performance).
   */
  PWInterval();
  PWInterval(SetPiece i);
  PWInterval(InterSet pieces);

  eq_class(PWInterval);
  lt_class(PWInterval);
};
std::ostream &operator<<(std::ostream &out, const PWInterval &i);

/**
 * @brief Traditional set operations.
 */

unsigned int cardinal(PWInterval i);
bool isEmpty(PWInterval i);
bool isMember(NAT x, PWInterval i);
Util::NAT minElem(PWInterval pwi);
Util::NAT maxElem(PWInterval pwi);
PWInterval intersection(PWInterval pwi1, PWInterval pwi2);
PWInterval cup(PWInterval i1, PWInterval i2);
PWInterval complement(Interval i);
PWInterval complement(PWInterval i);
PWInterval difference(PWInterval i1, PWInterval i2);

/**
 * @brief Extra operations.
 */

/** @function isCompact
 *
 * @brief This function determines if a pwi is compact (composed only by compact 
 * intervals).
 */
bool isCompact(InterSet ii);

/** @function optCond
 *
 * @brief Check if the InterSet satisfies the conditions to use optimizations.
 */
bool optConds(InterSet ii);

/* @function canonize
 *
 * @brief Tries to convert the argument to canonical form. Currently works only
 * with compact InterSet. 
 */
InterSet canonize(InterSet ii);

/** @function concatenation
 *
 * @brief Function useful to unite two pwis in the case these are known to be
 * disjoint.
 */
PWInterval concatenation(PWInterval pwi1, PWInterval pwi2);

/** @function boundedTraverse
 *
 * @brief Traverse pwis in order until one reaches its end, obtaining an ordered
 * result.
 */
InterSet boundedTraverse(PWInterval pwi1, PWInterval pwi2, SetPiece (*func)(SetPiece, SetPiece));

/** @function traverse
 *
 * @brief Traverse pwis in order reaching both ends, obtaining an ordered result.
 *   !!! Both pwis should be compact (not checked in this function).
 */
InterSet traverse(PWInterval pwi1, PWInterval pwi2, SetPiece (*func)(SetPiece, SetPiece));

std::size_t hash_value(const PWInterval &pwi);

typedef PWInterval Set;

} // namespace LIB

}  // namespace SBG

#endif
