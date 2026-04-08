/** @file user_input_handler.hpp

 @brief <b></b>

 This class will receive the user input configuration, and store it, so then
 the different stages of the SBG library can be executed.

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

#ifndef INPUT_HANDLER_HPP
#define INPUT_HANDLER_HPP

#include "boost/program_options.hpp"
#include "boost/optional.hpp"

namespace SBG {

namespace Util {

namespace prog_opts = boost::program_options;

void version();

class UserInputHandler {
  public:
  UserInputHandler();
  virtual ~UserInputHandler() = default;

  virtual void execute(int argc, char* argv[]) = 0;

  protected:
  prog_opts::options_description _generic; ///< Descriptive info (version, etc.) 
  prog_opts::options_description _config;  ///< SBG Program configuration 
  prog_opts::options_description _hidden;  ///< Options hidden to the user
  prog_opts::options_description _cmd_line_opts; ///< Command line options
  prog_opts::options_description _cfg_file_opts; ///< Options in config files
  prog_opts::options_description _visible; ///< All visible options for the user
  prog_opts::positional_options_description _positional;
  boost::optional<std::string> _config_file;
  boost::optional<std::string> _input_file;
};

} // namespace Util

} // namespace SBG

#endif
