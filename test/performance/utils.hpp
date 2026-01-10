/** @file interval.hpp

 @brief <b>Utils</b>

 Declares useful functions that all benchmarks can use

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

#ifndef TEST_PERF_UTILS
#define TEST_PERF_UTILS

#include "algorithms/misc/causalization_builders.hpp"

namespace Test {

namespace Internal {

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
SBG::LIB::BipartiteSBG generateSBG(std::string filename, int N, int copies);

/**
 * @brief
 */
SBG::LIB::MatchData calculateMatching(std::string filename, int N, int copies);

} // namespace Internal

} // namespace Test

#endif
