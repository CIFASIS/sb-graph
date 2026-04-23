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
 * @brief Builds the directed SBG used to detect algebraic loops.
 */
SBG::LIB::DirectedSBG buildLoopDetectionSBG(const SBG::LIB::MatchData& data);

/**
 * @brief Builds the directed SBG used to identify tearing variables.
 */
SBG::LIB::DirectedSBG buildTearingSBG(const SBG::LIB::SCCData& data);

SBG::LIB::DirectedSBG buildVerticalSortingSBG(const SBG::LIB::SCCData& data);

}  // namespace misc

#endif // SBGRAPH_ALGORITHMS_MISC_CAUSALIZATION_BUILDERS_HPP_
