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

#include <sbg/map.hpp>

namespace SBG {

namespace LIB {

/**
 * @brief Ordered collection of maps.
 */

/* @struct LTMap
 *
 * @brief This function is defined to be used by the MapSet definition. The
 * two inputs are disjoint (that's why the operator<, that compares any sort of
 * Maps, is not used).
 */
struct LTMap {
  bool operator()(const SBGMap &x, const SBGMap &y) const;
  typedef SBGMap first_argument_type;
  typedef SBGMap second_argument_type;
  typedef bool result_type;
};

typedef boost::container::set<SBGMap, LTMap, boost::container::new_allocator<SBGMap>, void> MapSet;
typedef boost::container::set<SBGMap>::iterator MapSetIt;
std::ostream &operator<<(std::ostream &out, const MapSet &ii);

struct PWMap {
  member_class(MapSet, maps);

  PWMap();
  PWMap(SBGMap m);
  PWMap(MapSet maps);

  eq_class(PWMap);
};
std::ostream &operator<<(std::ostream &out, const PWMap &pw);

/**
 * @brief Traditional map operations.
 */

} // namespace LIB

}  // namespace SBG

#endif
