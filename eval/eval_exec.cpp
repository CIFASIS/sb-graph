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

#include "algorithms/matching/matching_impl.hpp"
#include "algorithms/mfvs/mfvs_impl.hpp"
#include "algorithms/scc/scc_impl.hpp"
#include "algorithms/sorting/topological/ts_impl.hpp"
#include "eval/eval_exec.hpp"
#include "eval/file_evaluator.hpp"
#include "eval/input_translator.hpp"
#include "eval/file_evaluator.hpp"
#include "eval/visitors/autom_impl_visitor.hpp"
#include "parser/file_parser.hpp"
#include "sbg/pwmap_impl.hpp"
#include "sbg/set_impl.hpp"
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
    std::cout << "Set implementation: " << LIB::SET_IMPL.kind() << "\n";
    std::cout << "PWMap implementation: " << LIB::PWMAP_IMPL.kind() << "\n";
    std::cout << "-----------------------------------\n";
    std::cout << "Matching algorithm: " << LIB::MATCH_IMPL.kind() << "\n";
    std::cout << "SCC algorithm: " << LIB::SCC_IMPL.kind() << "\n";
    std::cout << "MFVS algorithm: " << LIB::MFVS_IMPL.kind() << "\n";
    std::cout << "Topological sorting algorithm: " << LIB::TS_IMPL.kind()
      << "\n";
  }
  std::cout << "-----------------------------------\n";
  std::cout << ">>>>>>>>>>> Eval result <<<<<<<<<<<\n";
  std::cout << "-----------------------------------\n\n";
}

////////////////////////////////////////////////////////////////////////////////
// Evaluation Executor ---------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

EvalExecutor::EvalExecutor() : _scc_impl(1), _mfvs_impl(1)
{
  _config.add_options()
    ("set_impl,s", Util::prog_opts::value(&_set_impl),
     " Desired set implementation:"
     "\n  - 0 for unordered sets (default option)"
     "\n  - 1 for ordered sets"
     "\n  - 2 for unidimensional ordered dense sets")
    ("pw_impl,p", Util::prog_opts::value(&_pw_impl),
     " Desired PWMap implementation:"
     "\n  - 0 for unordered PWMaps (default option)"
     "\n  - 1 for ordered PWMaps"
     "\n  - 2 for domain ordered PWMaps")
    ("scc_impl", Util::prog_opts::value(&_scc_impl),
     "Desired SCC algorithm implementation:"
     "\n  - 0 for V1 of minimum reachable SCC"
     "\n  - 1 for V2 of minimum reachable SCC (default option)")
    ("mfvs_impl", Util::prog_opts::value(&_mfvs_impl),
     "Desired MFVS algorithm implementation:"
     "\n  - 0 for degree greedy MFVS"
     "\n  - 1 for smallest set-vertex MFVS (default option)");

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

  if (_set_impl) {
    if (_set_impl > autom_impl.set_impl()) {
      Util::ERROR("Incompatible set implementation for the SBG input\n");
    }

    result.set_set_impl(_set_impl);
  }
  if (_pw_impl) {
    if (_pw_impl > autom_impl.pw_impl()) {
      Util::ERROR("Incompatible PWMap implementation for the SBG input\n");
    }

    result.set_pw_impl(_pw_impl);
  }

  result.set_scc_impl(_scc_impl);
  result.set_mfvs_impl(_mfvs_impl);

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
