/** @file boost_exec.hpp

 @brief <b>Executor of the Boost testing</b>

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

#ifndef BOOST_EXEC_HPP
#define BOOST_EXEC_HPP

#include "util/user_input_handler.hpp"

namespace Boost {

class BoostExecutor : public SBG::Util::UserInputHandler {
  public:
  BoostExecutor();

  void execute(int arg_count, char* args[]) override;

  private:
  boost::optional<int> stage_;
};

} // namespace Boost

#endif
