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

#include "algorithms/matching/matching_fact.hpp"
#include "algorithms/mfvs/mfvs_fact.hpp"
#include "algorithms/scc/scc_fact.hpp"
//#include "algorithms/toposort/ts_fact.hpp"
#include "eval/eval_exec.hpp"
#include "eval/file_evaluator.hpp"
#include "eval/input_translator.hpp"
#include "eval/file_evaluator.hpp"
#include "eval/visitors/autom_impl_visitor.hpp"
#include "parser/file_parser.hpp"
#include "sbg/set_fact.hpp"
#include "sbg/pwmap_fact.hpp"
#include "util/debug.hpp"
#include "util/logger.hpp"
#include "util/time_profiler.hpp"

#include "boost/program_options.hpp"

#include <iostream>
#include <fstream>
#include <streambuf>

namespace SBG {

namespace Eval {

////////////////////////////////////////////////////////////////////////////////
// Auxiliary functions ---------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

void printHeader(Util::prog_opts::variables_map vm)
{
  if (vm.count("debug")) {
    std::cout << "-----------------------------------\n";
    std::cout << "Set implementation: " << LIB::SET_FACT.kind() << "\n";
    std::cout << "PWMap implementation: " << LIB::PWMAP_FACT.kind() << "\n";
    std::cout << "-----------------------------------\n";
    std::cout << "Matching algorithm: " << LIB::MATCH_FACT.kind() << "\n";
    std::cout << "SCC algorithm: " << LIB::SCC_FACT.kind() << "\n";
    std::cout << "MFVS algorithm: " << LIB::MFVS_FACT.kind() << "\n\n";
    //std::cout << "Topological sort algorithm: " << LIB::TS_FACT.kind()
    // << "\n";
  }
  std::cout << "-----------------------------------\n";
  std::cout << ">>>>>>>>>>> Eval result <<<<<<<<<<<\n";
  std::cout << "-----------------------------------\n\n";
}

////////////////////////////////////////////////////////////////////////////////
// Evaluation Executor ---------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

EvalExecutor::EvalExecutor() : scc_impl_(1)
{
  _config.add_options()
    ("set_impl,s", Util::prog_opts::value(&set_impl_),
     " Desired set implementation:"
     "\n  - 0 for unordered sets (default option)"
     "\n  - 1 for ordered sets"
     "\n  - 2 for unidimensional ordered dense sets")
    ("pw_impl,p", Util::prog_opts::value(&pw_impl_),
     " Desired PWMap implementation:"
     "\n  - 0 for unordered PWMaps (default option)"
     "\n  - 1 for ordered PWMaps"
     "\n  - 2 for domain ordered PWMaps")
    ("scc_impl", Util::prog_opts::value(&scc_impl_),
     "Desired SCC algorithm implementation:"
     "\n  - 0 for V1 of minimum reachable SCC"
     "\n  - 1 for V2 of minimum reachable SCC (default option)");

  // First option without name is the input file
  _positional.add("input-file", 1);
  _cmd_line_opts.add(_generic).add(_config).add(_hidden);
  _cfg_file_opts.add(_config).add(_hidden);
  _visible.add(_generic).add(_config);
}

detail::EvalUserInput EvalExecutor::chooseImplementation()
{
  detail::EvalUserInput result;

  detail::AutomImplVisitor autom_impl_visitor;
  detail::EvalUserInput autom_impl = autom_impl_visitor.visit(Parser::parseFile(
    *_input_file, false));

  result.set_set_impl(autom_impl.set_impl());
  result.set_pw_impl(autom_impl.pw_impl());

  if (set_impl_) {
    if (set_impl_ > autom_impl.set_impl()) {
      Util::ERROR("Incompatible set implementation for the SBG input\n");
    }

    result.set_set_impl(set_impl_);
  }
  if (pw_impl_) {
    if (pw_impl_ > autom_impl.pw_impl()) {
      Util::ERROR("Incompatible PWMap implementation for the SBG input\n");
    }

    result.set_pw_impl(pw_impl_);
  }
  result.set_scc_impl(scc_impl_);

  return result;
}

void EvalExecutor::execute(int argc, char* argv[])
{
  // Command line options handling ---------------------------------------------

  Util::prog_opts::variables_map vm;
  store(Util::prog_opts::command_line_parser(argc, argv)
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
    std::ifstream config_fs{(*_config_file).c_str()};
    if (config_fs) {
      store(parse_config_file(config_fs, _cfg_file_opts), vm);
      notify(vm);
    }
  }

  // Input SBG program file handling -------------------------------------------

  if (vm.count("debug")) {
    Util::SBGLogger::instance().setLevel(Util::LogLevel::Debug);
  }

  if (_input_file) {
    detail::EvalUserInput input = chooseImplementation();
    detail::InputTranslator input_translator;
    input_translator.translate(input);
    ProgramIO eval_result = parseEvalFile(*_input_file); 
    printHeader(vm);
    std::cout << eval_result;
    Util::Internal::TimeProfiler::print_execution_time();
  }
  else {
    std::cout << "Usage: filename [options]\n";
    Util::ERROR("EvalExecutor: must provide a SBG program filename\n");
  }
}

} // namespace Eval

} // namespace SBG
