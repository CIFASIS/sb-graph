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
#include <fstream>

#include "boost/optional.hpp"

#include "parser/file_parser.hpp"
#include "parser/parser_exec.hpp"
#include "util/debug.hpp"

namespace SBG {

namespace Parser {

////////////////////////////////////////////////////////////////////////////////
// Parser Executor -------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

ParserExecutor::ParserExecutor() : UserInputHandler()
{
  _cmd_line_opts.add(_generic).add(_config).add(_hidden);
  _cfg_file_opts.add(_config).add(_hidden);
  _visible.add(_generic).add(_config);
}

void ParserExecutor::execute(int arg_count, char* args[])
{
  // Command line options handling ---------------------------------------------

  Util::prog_opts::variables_map vm;
  store(Util::prog_opts::command_line_parser(arg_count, args)
    .options(_cmd_line_opts).positional(_positional).run(), vm);
  notify(vm);

  // Help handling -------------------------------------------------------------
 
  if (vm.count("help")) {
    std::cout << "Usage: filename [options]\n";
    std::cout << "Command line options are prioritized over configuration file"
      " options.";
    std::cout << _visible << "\n";
    return;
  }

  // Version handling ----------------------------------------------------------

  if (vm.count("version")) {
    Util::version();
    return;
  }

  // Optional configuration file handling --------------------------------------
 
  if (_config_file) { 
    std::ifstream config_fs((*_config_file).c_str());
    if (config_fs) {
      store(parse_config_file(config_fs, _cfg_file_opts), vm);
      notify(vm);
    }
  }

  // Input SBG program file handling -------------------------------------------

  if (_input_file) {
    parseFile(*_input_file);
  }
  else {
    std::cout << "Usage: filename [options]";
    Util::ERROR("ParserExecutor: must provide a SBG program filename\n");
  }
}

} // namespace Parser

} // namespace SBG
