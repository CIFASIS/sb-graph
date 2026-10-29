/** @file ts_utils.hpp

 @brief <b>Auxiliary functions for topological sorting</b>

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

#ifndef SBGRAPH_ALGORITHMS_SORTING_TOPOLOGICAL_TS_UTILS_HPP_
#define SBGRAPH_ALGORITHMS_SORTING_TOPOLOGICAL_TS_UTILS_HPP_

#include <sbgraph/sbg/directed_sbg.hpp>
#include <sbgraph/sbg/pw_map.hpp>

namespace SBG {

namespace LIB {

/**
 * @class SortedSets
 * @brief Auxiliary structure to keep track of visited set-vertices and
 * set-edges during the topological sorting.
 */
class SortedSets {
public:
  SortedSets() = default;

  std::size_t size() const;
  Set& operator[](std::size_t k);
  const Set& operator[](std::size_t k) const;

  void pushBack(Set s);

  Set accumulated() const;
  Set accumulated(int n) const;

  int repetitionDepth() const;

private:
  std::vector<Set> _sets; 
};

/**
 * @brief Check that the calculated map is an actual sort.
 */
void checkSort(PWMap& smap, const DirectedSBG& dsbg);

} // namespace LIB

}  // namespace SBG

#endif // SBGRAPH_ALGORITHMS_SORTING_TOPOLOGICAL_TS_UTILS_HPP_
