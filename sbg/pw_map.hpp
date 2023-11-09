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

#include <boost/unordered/unordered_flat_set.hpp>

#include "sbg/map.hpp"

namespace SBG {

namespace LIB {

/**
 * @brief Unordered collection of maps.
 */

using MapSet = boost::unordered::unordered_flat_set<SBGMap>;
std::ostream &operator<<(std::ostream &out, const MapSet &ms);

struct PWMap {
  typedef MapSet::iterator iterator;
  typedef MapSet::const_iterator const_iterator;

  member_class(MapSet, maps);

  PWMap();
  PWMap(Set s); // Create id with s as domain
  PWMap(SBGMap m);
  PWMap(MapSet maps);

  iterator begin();
  iterator end();

  std::size_t size();
  void emplace(SBGMap m);
  void emplace_hint(iterator it, SBGMap m);
  void emplaceBack(SBGMap m);

  PWMap operator+(const PWMap &pw);

  // Two maps are equal iff they satisfy the extensional principle
  eq_class(PWMap);
  neq_class(PWMap);
};
std::ostream &operator<<(std::ostream &out, const PWMap &pw);

/**
 * @brief Traditional map operations.
 */
bool isEmpty(PWMap pw);
Set dom(PWMap pw);
PWMap restrict(Set subdom, PWMap pw);

Set image(PWMap pw);
Set image(Set subdom, PWMap pw);

Set preImage(PWMap pw);
Set preImage(Set subcodom, PWMap pw);

PWMap mapInf(PWMap pw, unsigned int n);
PWMap mapInf(PWMap pw);

/** @function composition
 *
 * @brief Apply first pw2, then pw1 (i.e. pw1(pw2(x)) is calculated).
 */
PWMap composition(PWMap pw1, PWMap pw2);

/**
 * @brief Extra operations.
 */

/** @function concatenation
 *
 * @brief Concatenation of two pwis.
 * !!! Result is correct iff pw1 and pw2 are domain-disjoint.
 */
PWMap concatenation(PWMap pw1, PWMap pw2);
PWMap combine(PWMap pw1, PWMap pw2);

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
PWMap reduce(Interval i, LExp e);
PWMap reduce(SBGMap sbgmap);
PWMap reduce(PWMap pw);

PWMap minMap(Interval i, LExp le1, LExp le2, LExp le3, LExp le4);
PWMap minMap(Interval i, LExp le1, LExp le2);
PWMap minMap(SetPiece dom_piece, Exp e1, Exp e2, Exp e3, Exp e4);
PWMap minMap(SetPiece dom_piece, Exp e1, Exp e2);
PWMap minMap(Set dom, Exp e1, Exp e2, Exp e3, Exp e4);
PWMap minMap(Set dom, Exp e1, Exp e2);
PWMap minMap(PWMap pw1, PWMap pw2, PWMap pw3, PWMap pw4);
PWMap minMap(PWMap pw1, PWMap pw2);

PWMap minAdjMap(PWMap pw1, PWMap pw2, PWMap pw3);
PWMap minAdjMap(PWMap pw1, PWMap pw2);

PWMap minInv(Set im, PWMap pw);
PWMap minInv(PWMap pw);

PWMap filterMap(bool (*f)(SBGMap), PWMap pw);

// Returns elements in both doms, that have the same image in both maps
Set equalImage(PWMap pw1, PWMap pw2);

PWMap offsetDom(Util::MD_NAT off, PWMap pw);
PWMap offsetDom(PWMap off, PWMap pw);
PWMap offsetImage(Util::MD_NAT off, PWMap pw);
PWMap offsetImage(Exp off, PWMap pw);

unsigned int nmbrDims(PWMap pw);

PWMap normalize(PWMap pw);

} // namespace LIB

}  // namespace SBG

#endif
