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

#include "eval/eval_exec.hpp"
#include "eval/file_evaluator.hpp"
#include "eval/input_translator.hpp"
#include "eval/file_evaluator.hpp"
#include "util/debug.hpp"

namespace SBG {

namespace Eval {

////////////////////////////////////////////////////////////////////////////////
// Evaluation Executor ---------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

EvalExecutor::EvalExecutor() : set_impl_(0)
{
  config_.add_options()
    ("set_impl,s", Util::prog_opts::value(&set_impl_),
      " Desired set implementation:"
      "\n  - 0 for unordered sets (default option)"
      "\n  - 1 for ordered sets"
      "\n  - 2 for unidimensional ordered dense sets");

  cmd_line_opts_.add(generic_).add(config_).add(hidden_);
  cfg_file_opts_.add(config_).add(hidden_);
  visible_.add(generic_).add(config_);
}

EvalUserInput EvalExecutor::gatherUserInput()
{
  EvalUserInput result;

  result.set_set_impl(set_impl_);

  return result;
}

void EvalExecutor::execute(int arg_count, char* args[])
{
  // Command line options handling ---------------------------------------------

  Util::prog_opts::variables_map vm;
  store(Util::prog_opts::command_line_parser(arg_count, args)
    .options(cmd_line_opts_).positional(positional_).run(), vm);
  notify(vm);

  // Help handling -------------------------------------------------------------
 
  if (vm.count("help")) {
    std::cout << "Usage: filename [options]\n";
    std::cout << "Command line options are prioritized over configuration file"
      " options.";
    std::cout << visible_ << "\n";
    return;
  }

  // Version handling ----------------------------------------------------------

  if (vm.count("version")) {
    Util::version();
    return;
  }

  // Optional configuration file handling --------------------------------------
 
  if (config_file_) { 
    std::ifstream config_fs((*config_file_).c_str());
    if (config_fs) {
      store(parse_config_file(config_fs, cfg_file_opts_), vm);
      notify(vm);
    }
  }

  // Input SBG program file handling -------------------------------------------

  if (input_file_) {
    EvalUserInput input = gatherUserInput();
    InputTranslator input_translator;
    input_translator.translate(input);
    parseEvalFile(*input_file_); 
  }
  else {
    std::cout << "Usage: filename [options]\n";
    Util::ERROR("EvalExecutor: must provide a SBG program filename\n");
  }
}

} // namespace Eval

} // namespace SBG
