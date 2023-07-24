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

#include <boost/container/set.hpp>

#include <sbg/lexp.hpp>
#include <sbg/pw_inter.hpp>
#include <util/debug.hpp>

namespace SBG {

struct SBGMap {
  member_class(SetPiece, dom);
  member_class(LExp, exp);

  SBGMap();
  SBGMap(Interval dom, LExp exp);

  eq_class(SBGMap);
  lt_class(SBGMap);
};

/**
 * @brief Traditional map operations.
 */
SetPiece image(SBGMap sbgmap);
SetPiece image(SetPiece subdom, SBGMap sbgmap);
SetPiece preImage(SBGMap sbgmap);
SetPiece preImage(SetPiece subcodom, SBGMap sbgmap);

}  // namespace SBG

#endif
