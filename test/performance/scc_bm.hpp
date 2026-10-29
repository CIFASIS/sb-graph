/** @file scc_bm.hpp

 @brief <b>SCC Benchmark</b>

 Executes the SBG version of the matching algorithm for TestRL1.test
 , TestRL2.test and TestRL3.test. It is used to showcase the constant
 execution time when the repetitive patterns increase its size.

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

#ifndef SBGRAPH_TEST_PERFORMANCE_SCC_BM_HPP_
#define SBGRAPH_TEST_PERFORMANCE_SCC_BM_HPP_

#include <string>

namespace SBG {

namespace perf {

namespace detail {

/**
 * @brief Registers for execution the SBG SCC benchmark of the desired test
 * file. The benchmark first increaseas the value of N, which is the size
 * of repetitive patterns in the original SBG. Then, taking the original value
 * of N, it copies the SBG  to increase the number of repetitive patterns.
 */
void registerSCCBenchmarks(std::string filename);

/**
 * @brief Registers for execution the SBG SCC benchmark of TestRL1.test
 * , TestRL2.test and TestRL3.test, using the previous function.
 */
void registerSCCBenchmarks();

} // namespace detail

} // namespace perf

} // namespace SBG

#endif // SBGRAPH_TEST_PERFORMANCE_SCC_BM_HPP_
