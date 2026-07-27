/** @file causalization_builders.hpp

 @brief <b>Causalization SBG builders</b>

 These builders are not part of the library per se, but are included here to
 test faster the causalization of Modelica models using the SBG approach. In
 the future it should belong to ModelicaCC instead.

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

#ifndef SBGRAPH_ALGORITHMS_MISC_CAUSALIZATION_BUILDERS_HPP_
#define SBGRAPH_ALGORITHMS_MISC_CAUSALIZATION_BUILDERS_HPP_

#include "algorithms/matching/match_data.hpp"
#include "algorithms/scc/scc_data.hpp"
#include "sbg/directed_sbg.hpp"

namespace misc {

/**
 * @brief Builds the directed SBG used to detect algebraic loops. To do so,
 * it merges the matched edges of the input SBG of \p data, adding them
 * as vertices of the new graphs. Then, it adds an edge (u, v) if in the
 * input bipartite SBG there was an unmatched edge between the matched edges
 * represented by u and v. The direction of (u, v) is from right to left
 * according to the input bipartite SBG, representing that the equation
 * referenced by u must be solved before the equation referenced by v.
 */
SBG::LIB::DirectedSBG buildLoopDetectionSBG(const SBG::LIB::MatchData& data);

/**
 * @brief Builds the directed SBG used to identify tearing variables.
 */
SBG::LIB::DirectedSBG buildTearingSBG(const SBG::LIB::SCCData& data);

/**
 * @brief Builds the directed acyclic SBG used to order vertically equations.
 */
SBG::LIB::SCCData buildVerticalSortingSBG(const SBG::LIB::SCCData& data
  , const SBG::LIB::Set& mfvs);

}  // namespace misc

#endif // SBGRAPH_ALGORITHMS_MISC_CAUSALIZATION_BUILDERS_HPP_
