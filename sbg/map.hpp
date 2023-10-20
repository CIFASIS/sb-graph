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

#include <iostream>

#include "sbg/multidim_inter.hpp"
#include "sbg/multidim_lexp.hpp"
#include "sbg/ord_pw_mdinter.hpp"
#include "sbg/unord_pw_mdinter.hpp"
#include "util/debug.hpp"

namespace SBG {

namespace LIB {

bool compatible(SetPiece mdi, Exp mdle);

// Non-optimized implementation ------------------------------------------------

template<typename Set>
struct SBGMap {
  typedef typename Set::iterator SetIt;

  member_class(Set, dom);
  member_class(Exp, exp);

  SBGMap();
  SBGMap(Interval i, LExp le);
  SBGMap(Set dom, Exp exp);

  eq_class(SBGMap);
  neq_class(SBGMap);
};
template<typename Set>
std::ostream &operator<<(std::ostream &out, const SBGMap<Set> &sbgmap);

/**
 * @brief Traditional map operations.
 */

template<typename Set>
SBGMap<Set> restrict(Set subdom, SBGMap<Set> sbgmap);

Interval image(Interval i, LExp le);
SetPiece image(SetPiece mdi, Exp le);

template<typename Set>
Set image(SBGMap<Set> sbgmap);

template<typename Set>
Set image(Set subdom, SBGMap<Set> sbgmap);

template<typename Set>
Set preImage(SBGMap<Set> sbgmap);

template<typename Set>
Set preImage(Set subcodom, SBGMap<Set> sbgmap);

template<typename Set>
SBGMap<Set> composition(SBGMap<Set> sbgmap1, SBGMap<Set> sbgmap2);

/**
 * @brief Extra operations.
 */

template<typename Set>
SBGMap<Set> minInv(SBGMap<Set> sbgmap);

template <typename Set>
std::size_t hash_value(const SBGMap<Set> &sbgmap);

typedef SBGMap<UnordSet> BaseMap;
typedef SBGMap<OrdSet> CanonMap;

} // namespace LIB

}  // namespace SBG

#endif
