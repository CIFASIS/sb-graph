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

#include "sbg/lexp.hpp"
#include "sbg/pw_inter.hpp"
#include "util/debug.hpp"

namespace SBG {

namespace LIB {

struct SBGMap {
  member_class(Set, dom);
  member_class(Exp, exp);

  SBGMap();
  SBGMap(Set dom, Exp exp);

  eq_class(SBGMap);
  neq_class(SBGMap);
};
std::ostream &operator<<(std::ostream &out, const SBGMap &sbgmap);

/**
 * @brief Traditional map operations.
 */

SBGMap restrict(Set subdom, SBGMap sbgmap);
Set image(SBGMap sbgmap);
Set image(Set subdom, SBGMap sbgmap);
Set preImage(SBGMap sbgmap);
Set preImage(Set subcodom, SBGMap sbgmap);
SBGMap composition(SBGMap sbgmap1, SBGMap sbgmap2);

/**
 * @brief Extra operations.
 */

typedef std::optional<SBGMap> MaybeMap;
MaybeMap canonize(SBGMap sbgmap1, SBGMap sbgmap2);

std::size_t hash_value(const SBGMap &sbgmap);

} // namespace LIB

}  // namespace SBG

#endif
