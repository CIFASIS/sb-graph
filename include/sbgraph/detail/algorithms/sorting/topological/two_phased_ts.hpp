/** @file two_phased_ts.hpp

 @brief <b>Concrete SBG Two Phased Topological Sorting Algorithm
 implementation</b>

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

#ifndef SBGRAPH_ALGORITHMS_SORTING_TOPOLOGICAL_TWO_PHASED_TS_HPP_
#define SBGRAPH_ALGORITHMS_SORTING_TOPOLOGICAL_TWO_PHASED_TS_HPP_

#include <sbgraph/algorithms/sorting/topological/ts_utils.hpp>
#include <sbgraph/sbg/directed_sbg.hpp>
#include <sbgraph/sbg/pw_map.hpp>
#include <sbgraph/sbg/set.hpp>

#include <tuple>

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Two Phased Topological Sorting Algorithm Implementation ---------------------
////////////////////////////////////////////////////////////////////////////////

/**
 * @class TwoPhasedTS
 * @brief Concrete implementation that starts sorting sets of vertices inside
 * each SCC taking advantage of rmap, and then combines this result with a
 * representatives sorting.
 */
class TwoPhasedTS {
public:
  TwoPhasedTS();

  PWMap calculate(TSInput input);

private:
  PWMap sortInside(const PWMap& Emap);

  PWMap sortOutside(const PWMap& Emap);

  PWMap repetition(const PWMap& Emap);

  PWMap extendRepetition() const;

  DirectedSBG _dsbg;
  PWMap _smap;
  PWMap _rmap;
  SortedSets _repetitionSE; ///< Set-edges deleted up to repetition.
  SortedMaps _repetition_sort; ///< Sorting up to repetition.
  int _depth; ///< Repetition depth.
  PWMap _original_Vmap;
  PWMap _original_Emap;
};

} // namespace detail

} // namespace LIB

} // namespace SBG

#endif // SBGRAPH_ALGORITHMS_SORTING_TOPOLOGICAL_TWO_PHASED_TS_HPP_
