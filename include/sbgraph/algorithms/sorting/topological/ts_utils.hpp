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

class TSInput {
public:
  TSInput() = default;

  const DirectedSBG& dsbg() const;
  const PWMap& rmap() const;
  const PWMap& rmap_starts() const;
  const PWMap& rmap_ends() const;

  void set_dsbg(DirectedSBG dsbg);
  void set_rmap(PWMap rmap);
  void set_rmap_starts(PWMap rmap_starts);
  void set_rmap_ends(PWMap rmap_ends);

private:
  DirectedSBG _dsbg;
  PWMap _rmap;
  PWMap _rmap_starts;
  PWMap _rmap_ends;
};

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
  void dropLast(int n);

  Set accumulated() const;
  Set last(int n) const;

  int repetitionDepth() const;

private:
  std::vector<Set> _sets; 
};

/**
 * @class SortedMaps
 * @brief Auxiliary structure to keep track of calculated successors during the
 * topological sorting.
 */
class SortedMaps {
public:
  SortedMaps() = default;
  SortedMaps(PWMap Vmap);

  std::size_t size() const;
  PWMap& operator[](std::size_t k);
  const PWMap& operator[](std::size_t k) const;

  void pushBack(PWMap pw);
  void dropLast(int n);

  Set accumulatedDomain() const;
  PWMap last(int n) const;

  int repetitionDepth() const;

private:
  std::vector<PWMap> _maps; 
  PWMap _Vmap;
};

/**
 * @brief Check that the calculated map is an actual sort.
 */
void checkSort(PWMap& smap, const DirectedSBG& dsbg);

} // namespace LIB

}  // namespace SBG

#endif // SBGRAPH_ALGORITHMS_SORTING_TOPOLOGICAL_TS_UTILS_HPP_
