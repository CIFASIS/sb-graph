/** @file lexp.hpp

 @brief <b>Linear expressions implementation</b>

 A linear expression m*x+h is determined by its slope and offset. It will be
 used as the law of maps.

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

#ifndef SBG_LEXP_HPP
#define SBG_LEXP_HPP

#include <iostream>

#include <util/defs.hpp>

namespace SBG {

using RAT = Util::RATIONAL;

struct LExp {
  member_class(RAT, slope);
  member_class(RAT, offset);

  LExp();
  LExp(RAT slope, RAT offset);
};
std::ostream &operator<<(std::ostream &out, const LExp &le);

/**
 * @brief Traditional expression operations.
 */

/**
 * @brief Extra operations.
 */

}  // namespace SBG

#endif
