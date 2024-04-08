/** @file map.hpp

 @brief <b>SBG map implementation</b>

 An SBG map is a map consisting of an interval (domain) and a linear expression.
 The codomain should also be an interval (checked in the constructor).

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

#ifndef SBG_MAP_HPP
#define SBG_MAP_HPP

#include "sbg/multidim_lexp.hpp"
#include "sbg/ord_pw_mdinter.hpp"
#include "sbg/unord_pw_mdinter.hpp"
#include "util/debug.hpp"

namespace SBG {

namespace LIB {

Interval image(Interval i, LExp le);
SetPiece image(SetPiece mdi, Exp le);

template<typename Set>
struct SBGMap {
  using MaybeMap = std::optional<SBGMap<Set>>;

  member_class(Set, dom);
  member_class(Exp, exp);

  SBGMap();
  SBGMap(Util::MD_NAT x, Exp exp);
  SBGMap(Interval i, LExp le);
  SBGMap(SetPiece mdi, Exp exp);
  SBGMap(Set dom, Exp exp);

  bool operator==(const SBGMap &other) const;
  bool operator!=(const SBGMap &other) const;

  /**
   * @brief Traditional map operations.
   */
  SBGMap restrict(const Set &subdom) const;
  Set image() const;
  Set image(const Set &subdom) const;
  Set preImage() const;
  Set preImage(const Set &subcodom) const;
  SBGMap composition(const SBGMap &sbgmap2) const;

  /**
   * @brief Extra operations.
   */
  SBGMap minInv(const Set &allowed) const;
  SBGMap minInv() const;
  bool isId() const;
  unsigned int nmbrDims() const;
  MaybeMap compact(const SBGMap<Set> &other) const; 
};
template<typename Set>
std::ostream &operator<<(std::ostream &out, const SBGMap<Set> &sbgmap);

template<typename Set>
std::size_t hash_value(const SBGMap<Set> &sbgmap);

template<typename Set>
using SBGMap = SBGMap<Set>;

typedef SBGMap<UnordSet> BaseMap;
typedef SBGMap<OrdSet> CanonMap;

} // namespace LIB

}  // namespace SBG

#endif
