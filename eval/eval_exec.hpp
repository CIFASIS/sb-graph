/** @file eval_exec.hpp

 @brief <b>Executor of the evaluation of a program</b>

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

#ifndef SBGRAPH_EVAL_EXEC_HPP_
#define SBGRAPH_EVAL_EXEC_HPP_

#include "eval/user_input.hpp"
#include "util/user_input_handler.hpp"

namespace SBG {

namespace Eval {

class EvalExecutor : public Util::UserInputHandler {
public:
  EvalExecutor();

  void execute(int arg_count, char* args[]) override;

private:
  detail::EvalUserInput chooseImplementation();

  boost::optional<int> set_impl_;
  boost::optional<int> pw_impl_;
  boost::optional<int> scc_impl_;
};

} // namespace Eval

} // namespace SBG

#endif
