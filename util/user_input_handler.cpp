/*******************************************************************************

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

#include <iostream>

#include "boost/optional.hpp"

#include "util/user_input_handler.hpp"

namespace SBG {

namespace Util {

////////////////////////////////////////////////////////////////////////////////
// Auxiliary functions ---------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

void version()
{
  std::cout << "SBG library v4.0.0" << std::endl;
  std::cout << "License GPLv3+: GNU GPL version 3 or later"
    << " <http://gnu.org/licenses/gpl.html>\n";
  std::cout << "This is free software: you are free to change and redistribute" 
    << "it.\n";
  std::cout << "There is NO WARRANTY, to the extent permitted by law.\n";
}

////////////////////////////////////////////////////////////////////////////////
// User Input Handler ----------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

UserInputHandler::UserInputHandler() : _generic("Generic options")
  , _config("Configuration options"), _hidden("Hidden options")
  , _cmd_line_opts("Command line options")
  , _cfg_file_opts("Configuration file options"), _positional()
{
  _generic.add_options()
    ("help,h", "Prints all available options")
    ("version,v", "Displays version information")
    ("debug,d", "Activates debug messages")
    ("config,c", prog_opts::value(&_config_file), "Configuration filename");

  _hidden.add_options()
    ("input-file", prog_opts::value(&_input_file)
      , "Input SBG program");

  // First option without name is the input file
  _positional.add("input-file", 1);
}

} // namespace Util

} // namespace SBG
