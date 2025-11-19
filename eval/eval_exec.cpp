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
#include <streambuf>

#include "algorithms/cutvertex/cv_fact.hpp"
#include "algorithms/matching/matching_fact.hpp"
#include "algorithms/scc/scc_fact.hpp"
#include "algorithms/toposort/ts_fact.hpp"
#include "eval/eval_exec.hpp"
#include "eval/file_evaluator.hpp"
#include "eval/input_translator.hpp"
#include "eval/file_evaluator.hpp"
#include "eval/visitors/autom_impl_visitor.hpp"
#include "parser/file_parser.hpp"
#include "util/debug.hpp"

namespace SBG {

namespace Eval {

////////////////////////////////////////////////////////////////////////////////
// Auxiliary functions ---------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

void printHeader(Util::prog_opts::variables_map vm)
{
  if (vm.count("debug")) {
    std::cout << "-----------------------------------\n";
    std::cout << "Set implementation: " << LIB::SET_FACT.prettyPrint() << "\n";
    std::cout << "PWMap implementation: " << LIB::PW_FACT.prettyPrint() << "\n";
    std::cout << "-----------------------------------\n";
    std::cout << "Matching algorithm: " << LIB::MATCH_FACT.prettyPrint()
      << "\n";
    std::cout << "SCC algorithm: " << LIB::SCC_FACT.prettyPrint() << "\n";
    std::cout << "Cut vertex algorithm: " << LIB::CV_FACT.prettyPrint() << "\n";
    std::cout << "Topological sort algorithm: " << LIB::TS_FACT.prettyPrint()
      << "\n";
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
  config_.add_options()
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

  cmd_line_opts_.add(generic_).add(config_).add(hidden_);
  cfg_file_opts_.add(config_).add(hidden_);
  visible_.add(generic_).add(config_);
}

EvalUserInput EvalExecutor::chooseImplementation()
{
  EvalUserInput result;

  AutomImplVisitor autom_impl_visitor;
  EvalUserInput autom_impl = autom_impl_visitor.visit(Parser::parseFile(
    *input_file_, false));

  result.set_set_impl(autom_impl.set_impl());
  result.set_pw_impl(autom_impl.pw_impl());

  if (set_impl_) {
    if (set_impl_ > autom_impl.set_impl())
      Util::ERROR("Incompatible set implementation for the SBG input\n");

    result.set_set_impl(set_impl_);
  }
  if (pw_impl_) {
    if (pw_impl_ > autom_impl.pw_impl())
      Util::ERROR("Incompatible PW implementation for the SBG input\n");

    result.set_pw_impl(pw_impl_);
  }
  result.set_scc_impl(scc_impl_);

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
    EvalUserInput input = chooseImplementation();
    InputTranslator input_translator;
    input_translator.translate(input);
    ProgramIO eval_result = parseEvalFile(*input_file_); 
    printHeader(vm);
    std::cout << eval_result;
  }
  else {
    std::cout << "Usage: filename [options]\n";
    Util::ERROR("EvalExecutor: must provide a SBG program filename\n");
  }
}

} // namespace Eval

} // namespace SBG
