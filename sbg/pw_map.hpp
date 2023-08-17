/** @file pw_map.hpp

 @brief <b>Piecewise map implementation</b>

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

#ifndef SBG_PWMAP_HPP
#define SBG_PWMAP_HPP

#include <boost/container/flat_set.hpp>

#include "sbg/map.hpp"

namespace SBG {

namespace LIB {

/**
 * @brief Unordered collection of maps.
 */

struct LTMap {
  LTMap();

  bool operator()(const SBGMap &x, const SBGMap &y) const;
};

typedef boost::container::flat_set<SBGMap, LTMap, boost::container::new_allocator<SBGMap>> MapSet;
typedef MapSet::iterator MapSetIt;
std::ostream &operator<<(std::ostream &out, const MapSet &ms);

struct PWMap {
  member_class(MapSet, maps);

  PWMap();
  PWMap(SBGMap m);
  PWMap(MapSet maps);

  // Two maps are equal iff they satisfy the extensional principle
  eq_class(PWMap);
  neq_class(PWMap);
};
std::ostream &operator<<(std::ostream &out, const PWMap &pw);

/**
 * @brief Traditional map operations.
 */
Set dom(PWMap pw);
PWMap restrict(Set subdom, PWMap pw);
Set image(PWMap pw);
Set image(Set subdom, PWMap pw);
Set preImage(PWMap pw);
Set preImage(Set subcodom, PWMap pw);
PWMap mapInf(PWMap pw);

/** @function composition
 *
 * @brief Apply first pw2, then pw1 (i.e. pw1(pw2(x)) is calculated).
 */
PWMap composition(PWMap pw1, PWMap pw2);

/**
 * @brief Extra operations.
 */

bool isCompact(MapSet mm);
bool optConds(MapSet mm);
MapSet canonize(MapSet mm);

/** @function concat
 *
 * @brief Concatenation of two pwis.
 * !!! Result is correct iff pw1 and pw2 are domain-disjoint.
 */
PWMap concat(PWMap pw1, PWMap pw2);

PWMap minMap(SetPiece dom_piece, Exp e1, Exp e2);
PWMap minMap(Set dom, Exp e1, Exp e2);
PWMap minMap(PWMap pw1, PWMap pw2);

/** @function reduce
 *
 * @brief The reduce function calculates (if possible) the resulting map of
 * composing the argument with itself up to convergence.
 *
 * Currently, the only expressions that can be efficiently reduced are:
 *   - x+h
 *   - x-h
 *   - h
 */
PWMap reduce(SetPiece dom_piece, Exp e);
PWMap reduce(SBGMap sbgmap);
PWMap reduce(PWMap pw);

PWMap minAdjMap(PWMap pw1, PWMap pw2);

} // namespace LIB

}  // namespace SBG

#endif
