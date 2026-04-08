/** @file utils.hpp

 @brief <b>Utils</b>

 Declares useful functions for all benchmarks.

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

#ifndef SBGRAPH_TEST_PERFORMANCE_UTILS_HPP_
#define SBGRAPH_TEST_PERFORMANCE_UTILS_HPP_

#include "algorithms/matching/match_data.hpp"
#include "sbg/bipartite_sbg.hpp"
#include "sbg/natural.hpp"
#include "sbg/pw_map.hpp"
#include "sbg/set.hpp"

#include <string>

namespace SBG {

namespace perf {

namespace detail {

using SBG::LIB::NAT;
using SBG::LIB::Set;
using SBG::LIB::PWMap;
using SBG::LIB::BipartiteSBG;
using SBG::LIB::MatchData;

////////////////////////////////////////////////////////////////////////////////
// File Manipulation -----------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Replaces the value of N with the desired value in the designated
 * .test file.
 */
bool updateN(const std::string& filename, int N);

/**
 * @brief Reads a .test file to search for an SBG that will be the input for
 * the causalization process, modifying the size of set-vertices and set-edges
 * or the number of copies of the SBG, that is, the number of set-vertices
 * and set-edges.
 * @param N Size of set-vertices and set-edges. 
 * @param copies Copies of the SBG, i.e. number of set-vertices and set-edges.
 */
BipartiteSBG generateSBG(std::string filename, int N, int copies);

/**
 * @brief Reads a .test file to search for an SBG that will be the input for
 * the causalization process, modifying the size of set-vertices and set-edges
 * or the number of copies of the SBG, that is, the number of set-vertices
 * and set-edges. It then applies the matching SBG algorithm for the modified
 * SBG.
 * @param N Size of set-vertices and set-edges. 
 * @param copies Copies of the SBG, i.e. number of set-vertices and set-edges.
 */
MatchData calculateMatching(std::string filename, int N, int copies);

////////////////////////////////////////////////////////////////////////////////
// Set Construction ------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

/*
 * @brief Creates two dense sets, where the first one is:
 * {[0:99]x[0:99], [100:199]x[0:99], ..., [N-99:N]x[0:99]}
 * and the second PW:
 * {[50:149]x[0:99], [150:249]x[0:99], ..., [N-49:N+50]x[0:99]}
 * where N = set_sz*100-1.
 */
std::pair<Set, Set> nonDisjointPieces(NAT set_sz);

/*
 * @brief Creates two dense sets, where the first one is:
 * {[0:99], [200:199], ..., [N-199:N-100]}
 * and the second PW:
 * {[100:199], [300:399], ..., [N-99:N]}
 * where N = set_sz*100-1.
 */
std::pair<Set, Set> interlacedPieces(NAT set_sz);

////////////////////////////////////////////////////////////////////////////////
// PWMap Construction ----------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

/*
 * @brief Creates a PW with a dense domain of the form:
 * <<{[0:99], [100:199], ..., [900:999]} -> |x|
 *   , {[1000:1099], ..., [1900:1999]} -> |x|
 *   , ..., {[N-999:N-900], ..., [N-99:N]} -> |x|>>.
 * where N = map_sz*1e3-1.
 */
PWMap denseDom(NAT map_sz);

/*
 * @brief Creates two PWs with a dense domain, where the first one is:
 * <<{[0:99], [100:199], ..., [900:999]} -> |x|
 *   , {[1000:1099], ..., [1900:1999]} -> |x|
 *   , ..., {[N-999:N-900], ..., [N-99:N]} -> |x|>>.
 * and the second PW:
 * <<{[50:149], [150:249], ..., [950:1049]} -> |x-1|
 *   , {[1050:1149], ..., [1950:2049]} -> |x-1|
 *   , ..., {[N-949:N-850], ..., [N-49:N+50]} -> |x-1|>>
 * where N = map_sz*1e3-1.
 */
std::pair<PWMap, PWMap> minAdjMaps(NAT map_sz);

/*
 * @brief Creates two PWs with a dense domain, where the first one is:
 * <<{[0:99]x[0:99], [200:299]x[0:99], ..., [800:899]x[0:99]} -> |x|x|
 *   , {[1000:1099]x[0:99], ..., [1800:1899]x[0:99]} -> |x|x|
 *   , ..., {[N-999:N-900]x[0:99], ..., [N-199:N-100]x[0:99]} -> |x|x|>>.
 * and the second PW:
 * <<{[100:199]x[0:99], [300:399]x[0:99], ..., [900:999]x[0:99]} -> |x|x|
 *   , {[1100:1199]x[0:99], ..., [1300:1399]x[0:99]} -> |x|x|
 *   , ..., {[N-899:N-800]x[0:99], ..., [N-99:N]x[0:99]} -> |x|x|>>
 * where N = map_sz*1e3-1.
 */
std::pair<PWMap, PWMap> interlacedMaps(NAT map_sz);

/*
 * @brief Creates two PWs with a dense domain, where the first one is:
 * <<{[0:99]x[0:99], [100:199]x[0:99], ..., [900:999]x[0:99]} -> |x|x|
 *   , {[1000:1099]x[0:99]x, ..., [1900:1999]x[0:99]} -> |x|x|
 *   , ..., {[N-999:N-900]x[0:99], ..., [N-99:N]x[0:99]} -> |x|x|>>.
 * and the second PW:
 * <<{[50:149]x[0:99], [150:249]x[0:99], ..., [950:1049]x[0:99]} -> |x|x|
 *   , {[1050:1149]x[0:99], ..., [1950:2049]x[0:99]} -> |x|x|
 *   , ..., {[N-949:N-850]x[0:99], ..., [N-49:N+50]x[0:99]} -> |x|x|>>
 * where N = map_sz*1e3-1.
 */
std::pair<PWMap, PWMap> nonDisjointMaps(NAT map_sz);

/**
 * @brief TODO
 */
PWMap reducibleMaps(NAT map_sz);

} // namespace detail

} // namespace perf

} // namespace SBG

#endif // SBGRAPH_TEST_PERFORMANCE_UTILS_HPP_
