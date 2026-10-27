/** @file bm_exec.hpp

 @brief <b>Executor for benchmarks</b>

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

#ifndef SBGRAPH_TEST_PERFORMANCE_BM_EXEC_HPP_
#define SBGRAPH_TEST_PERFORMANCE_BM_EXEC_HPP_

#include "util/user_input_handler.hpp"

namespace SBG {

namespace perf {

namespace detail {

class BMExecutor : public Util::UserInputHandler {
public:
  BMExecutor();

  void execute(int arg_count, char* args[]) override;

private:
  boost::optional<int> _benchmark;
  boost::optional<int> _set_impl;
  boost::optional<int> _pw_impl;
  boost::optional<int> _scc_impl;
};

} // namespace detail

} // namespace perf

} // namespace SBG

#endif // SBGRAPH_TEST_PERFORMANCE_BM_EXEC_HPP_
