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

template<typename Set>
struct LTMap {
  LTMap();

  bool operator()(const SBGMap<Set> &x, const SBGMap<Set> &y) const;
};


template<typename Set>
using MapSet = boost::container::flat_set<
  SBGMap<Set>, LTMap<Set>, boost::container::new_allocator<SBGMap<Set>>
>;
template<typename Set>
using MapSetIt = typename MapSet<Set>::iterator;
template<typename Set>
using MapSetConstIt = typename MapSet<Set>::const_iterator;
template<typename Set>
std::ostream &operator<<(std::ostream &out, const MapSet<Set> &ms);

template<typename Set>
struct PWMap {
  typedef MapSetIt<Set> iterator;
  typedef MapSetConstIt<Set> const_iterator;

  member_class(MapSet<Set>, maps);

  PWMap();
  PWMap(Set s); // Create id with s as domain
  PWMap(SBGMap<Set> m);
  PWMap(MapSet<Set> maps);

  iterator begin();
  iterator end();

  std::size_t size();
  void emplace(SBGMap<Set> m);
  void emplace_hint(MapSetIt<Set> it, SBGMap<Set> m);
  void emplaceBack(SBGMap<Set> m);
  SBGMap<Set> &operator[](std::size_t n);
  const SBGMap<Set> &operator[](std::size_t n) const;

  PWMap operator+(const PWMap &pw);

  // Two maps are equal iff they satisfy the extensional principle
  eq_class(PWMap);
  neq_class(PWMap);
};
template<typename Set>
std::ostream &operator<<(std::ostream &out, const PWMap<Set> &pw);

/**
 * @brief Traditional map operations.
 */
template<typename Set>
bool isEmpty(PWMap<Set> pw);

template<typename Set>
Set dom(PWMap<Set> pw);

template<typename Set>
PWMap<Set> restrict(Set subdom, PWMap<Set> pw);

template<typename Set>
Set image(PWMap<Set> pw);

template<typename Set>
Set image(Set subdom, PWMap<Set> pw);

template<typename Set>
Set preImage(PWMap<Set> pw);

template<typename Set>
Set preImage(Set subcodom, PWMap<Set> pw);

template<typename Set>
PWMap<Set> mapInf(PWMap<Set> pw, unsigned int n);
template<typename Set>
PWMap<Set> mapInf(PWMap<Set> pw);

/** @function composition
 *
 * @brief Apply first pw2, then pw1 (i.e. pw1(pw2(x)) is calculated).
 */
template<typename Set>
PWMap<Set> composition(PWMap<Set> pw1, PWMap<Set> pw2);

/**
 * @brief Extra operations.
 */

/** @function concatenation
 *
 * @brief Concatenation of two pwis.
 * !!! Result is correct iff pw1 and pw2 are domain-disjoint.
 */
template<typename Set>
PWMap<Set> concatenation(PWMap<Set> pw1, PWMap<Set> pw2);
template<typename Set>
PWMap<Set> combine(PWMap<Set> pw1, PWMap<Set> pw2);

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
template<typename Set>
PWMap<Set> reduce(Interval i, LExp e);
template<typename Set>
PWMap<Set> reduce(SBGMap<Set> sbgmap);
template<typename Set>
PWMap<Set> reduce(PWMap<Set> pw);

template<typename Set>
PWMap<Set> minMap(Interval i, LExp le1, LExp le2, LExp le3, LExp le4);
template<typename Set>
PWMap<Set> minMap(Interval i, LExp le1, LExp le2);
template<typename Set>
PWMap<Set> minMap(SetPiece dom_piece, Exp e1, Exp e2, Exp e3, Exp e4);
template<typename Set>
PWMap<Set> minMap(SetPiece dom_piece, Exp e1, Exp e2);
template<typename Set>
PWMap<Set> minMap(Set dom, Exp e1, Exp e2, Exp e3, Exp e4);
template<typename Set>
PWMap<Set> minMap(Set dom, Exp e1, Exp e2);
template<typename Set>
PWMap<Set> minMap(PWMap<Set> pw1, PWMap<Set> pw2
                  , PWMap<Set> pw3, PWMap<Set> pw4);
template<typename Set>
PWMap<Set> minMap(PWMap<Set> pw1, PWMap<Set> pw2);

template<typename Set>
PWMap<Set> minAdjMap(PWMap<Set> pw1, PWMap<Set> pw2, PWMap<Set> pw3);
template<typename Set>
PWMap<Set> minAdjMap(PWMap<Set> pw1, PWMap<Set> pw2);

template<typename Set>
PWMap<Set> minInv(Set im, PWMap<Set> pw);
template<typename Set>
PWMap<Set> minInv(PWMap<Set> pw);

template<typename Set>
PWMap<Set> filterMap(bool (*f)(SBGMap<Set>), PWMap<Set> pw);

// Returns elements in both doms, that have the same image in both maps
template<typename Set>
Set equalImage(PWMap<Set> pw1, PWMap<Set> pw2);

template<typename Set>
PWMap<Set> offsetDom(Util::MD_NAT off, PWMap<Set> pw);
template<typename Set>
PWMap<Set> offsetDom(PWMap<Set> off, PWMap<Set> pw);
template<typename Set>
PWMap<Set> offsetImage(Util::MD_NAT off, PWMap<Set> pw);
template<typename Set>
PWMap<Set> offsetImage(Exp off, PWMap<Set> pw);

template<typename Set>
unsigned int nmbrDims(PWMap<Set> pw);

template<typename Set>
PWMap<Set> normalize(PWMap<Set> pw);

template<typename Set>
std::size_t hash_value(const PWMap<Set> &pw);

typedef PWMap<UnordSet> BasePWMap;
typedef PWMap<OrdSet> CanonPWMap;

} // namespace LIB

}  // namespace SBG

#endif
