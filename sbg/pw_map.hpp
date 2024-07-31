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

#include "sbg/map.hpp"

namespace SBG {

namespace LIB {

/**
 * @brief Unordered collection of maps.
 */

template<typename Set>
using MapSet = std::vector<SBGMap<Set>>;
template<typename Set>
std::ostream &operator<<(std::ostream &out, const MapSet<Set> &ms);

template<typename Set>
struct PWMap {
  using MS = MapSet<Set>;
  using Map = SBGMap<Set>;

  member_class(MS, maps);

  PWMap();
  PWMap(Set s); // Create id with s as domain
  PWMap(SBGMap<Set> m);
  PWMap(MapSet<Set> maps);

  typedef typename MS::iterator iterator;
  typedef typename MS::const_iterator const_iterator;
  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;
  std::size_t size() const;
  void emplace(SBGMap<Set> m);
  void emplaceBack(SBGMap<Set> m);

  // Two maps are equal iff they satisfy the extensional principle
  bool operator==(const PWMap &other) const;
  bool operator!=(const PWMap &other) const;

  PWMap operator+(const PWMap &pw) const;
  PWMap operator-(const PWMap &pw) const;

  /**
   * @brief Traditional map operations.
   */
  std::size_t arity() const;
  bool isEmpty() const;
  Set dom() const;
  PWMap restrict(const Set &subdom) const;
  Set image() const;
  Set image(const Set &subdom) const;
  Set preImage() const;
  Set preImage(const Set &subcodom) const;
  /** @function inverse
   *
   * @brief Calculate the inverse of a bijective pw_map. Throws error if it is
   * not bijective.
   */
  PWMap inverse() const;
  /** @function composition
   *
   * @brief Apply first pw2, then pw1 (i.e. pw1(pw2(x)) is calculated).
   */
  PWMap composition(const PWMap &pw2) const;
  PWMap mapInf(unsigned int n) const;
  PWMap mapInf() const;

  /**
   * @brief Extra operations.
   */

  /** @function concatenation
   *
   * @brief Concatenation of two pwis.
   * !!! Result is correct iff pw1 and pw2 are domain-disjoint.
   */
  PWMap concatenation(const PWMap &other) const;
  /** @function combine
   *
   * @brief For each element in the dom of pw1 applies the corresponding
   * expression of pw1, and applies the corresponding pw2 expression for all
   * remaining elements.
   */
  PWMap combine(const PWMap &other) const;

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
  PWMap reduce(const Interval &i, const LExp &e) const;
  PWMap reduce(const Map &sbgmap) const;
  PWMap reduce() const;

  PWMap minMap(
    const Interval &i, const LExp &le1, const LExp &le2
    , const LExp &le3, const LExp &le4
  ) const;
  PWMap minMap(const Interval &i, const LExp &le1, const LExp &le2) const;
  PWMap minMap(
    const SetPiece &dom_piece, const Exp &e1, const Exp &e2
    , const Exp &e3, const Exp &e4
  ) const;
  PWMap minMap(const SetPiece &dom_piece, const Exp &e1, const Exp &e2) const;
  PWMap minMap(
    const Set &dom, const Exp &e1, const Exp &e2, const Exp &e3, const Exp &e4
  ) const;
  PWMap minMap(const Set &dom, const Exp &e1, const Exp &e2) const;
  PWMap minMap(
    const PWMap &other2, const PWMap &other3, const PWMap &other4
  ) const;
  PWMap minMap(const PWMap &other) const;

  PWMap minAdjMap(const PWMap &other1, const PWMap &other2) const;
  PWMap minAdjMap(const PWMap &other) const;

  /** @function firstInv
   *
   * @brief Calculate the inverse of a pw_map. If it is not injective,
   * for the elements with more than one preImage, it will assign the first
   * appearing inverted expression that maps to that element.
   *
   * @param allowed 
   */
  PWMap firstInv(const Set &allowed) const;
  PWMap firstInv() const;

  PWMap filterMap(bool (*f)(const SBGMap<Set> &)) const;

  /** @function equalImage
   *
   * @brief Return elements in both doms, that have the same image in both maps
   */
  Set equalImage(const PWMap &other) const;
  Set gtImage(const PWMap &other) const;

  PWMap offsetDom(const Util::MD_NAT &off) const;
  PWMap offsetDom(const PWMap &off) const;
  PWMap offsetImage(const Util::MD_NAT &off) const;
  PWMap offsetImage(const Exp &off) const;

  PWMap compact() const;

  /** @function sharedImage
   *
   * @brief Given a map, return elements that have the same image.
   */
  Set sharedImage();
};
template<typename Set>
std::ostream &operator<<(std::ostream &out, const PWMap<Set> &pw);

typedef PWMap<UnordSet> BasePWMap;
typedef PWMap<OrdSet> CanonPWMap;

} // namespace LIB

}  // namespace SBG

#endif
