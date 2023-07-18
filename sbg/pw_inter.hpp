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

#include <boost/container/set.hpp>

#include <sbg/interval.hpp>

namespace SBG {

/**
 * @brief Ordered collection of intervals.
 */

typedef boost::container::set<Interval> InterSet;
typedef boost::container::set<Interval>::iterator InterSetIt;
std::ostream &operator<<(std::ostream &out, const InterSet &ii);

struct PWInterval {
  member_class(InterSet, pieces);

  /**
   * @brief Constructors don't check if intervals are disjoint (performance).
   */
  PWInterval();
  PWInterval(Interval i);
  PWInterval(InterSet pieces);

  eq_class(PWInterval);
};
std::ostream &operator<<(std::ostream &out, const PWInterval &i);

/**
 * @brief Traditional set operations.
 */

unsigned int cardinal(PWInterval i);
bool isEmpty(PWInterval i);
bool isMember(INT x, PWInterval i);
Util::INT minElem(PWInterval pwi);
Util::INT maxElem(PWInterval pwi);
PWInterval intersection(PWInterval i1, PWInterval i2);
PWInterval cup(PWInterval i1, PWInterval i2);
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

bool isCompact(PWInterval pwi);

/** @function boundedTraverse
 *
 * @brief Traverse pwis in order until one reaches its end, obtaining an ordered
 * result.
 */
InterSet boundedTraverse(PWInterval pwi1, PWInterval pwi2, Interval (*func)(Interval, Interval));

/** @function traverse
 *
 * @brief Traverse pwis in order reaching both ends, obtaining an orderedresult.
 *   !!! Both pwis should be compact (not checked in this function).
 */
InterSet traverse(PWInterval pwi1, PWInterval pwi2, Interval (*func)(Interval, Interval));

typedef PWInterval Set;

}  // namespace SBG

#endif
