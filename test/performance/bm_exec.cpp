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

#include "test/performance/bm_exec.hpp"
#include "algorithms/matching/matching_impl.hpp"
#include "algorithms/scc/scc_impl.hpp"
#include "eval/user_impl_map.hpp"
#include "sbg/pwmap_impl.hpp"
#include "sbg/set_impl.hpp"
#include "test/performance/boost/boost_bm.hpp"
#include "test/performance/matching_bm.hpp"
#include "test/performance/pwmap_bm.hpp"
#include "test/performance/scc_bm.hpp"
#include "test/performance/set_bm.hpp"
#include "util/debug.hpp"
#include "util/user_input_handler.hpp"

#include <benchmark/benchmark.h>

#include <iostream>
#include <fstream>
#include <streambuf>
#include <vector>

namespace SBG {

namespace perf {

namespace detail {

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
    std::cout << "SCC algorithm: " << LIB::SCC_IMPL.kind() << "\n\n";
  }
}

////////////////////////////////////////////////////////////////////////////////
// Benchmark Executor ----------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

BMExecutor::BMExecutor()
  : _benchmark(0), _set_impl(0), _pw_impl(0), _scc_impl(1)
{
  _config.add_options()
    ("benchmark,b", Util::prog_opts::value(&_benchmark), 
     "Desired benchmark:"
     "\n  - 0 for set benchmark"
     "\n  - 1 for PWMap benchmark"
     "\n  - 2 for matching benchmark"
     "\n  - 3 for SCC benchmark")
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
    ("boost", "Executes Boost Graph Library algorithms for scalar graphs");

  _positional.add("input-file", 1);
  _cmd_line_opts.add(_generic).add(_config).add(_hidden);
  _cfg_file_opts.add(_config).add(_hidden);
  _visible.add(_generic).add(_config);
}

void BMExecutor::execute(int argc, char* argv[])
{
  // Command line options handling ---------------------------------------------

  Util::prog_opts::variables_map vm;
  store(Util::prog_opts::command_line_parser(argc, argv)
    .options(_cmd_line_opts).positional(_positional).allow_unregistered().run()
    , vm);
  notify(vm);

  // Help handling -------------------------------------------------------------
 
  if (vm.count("help")) {
    std::cout << "Usage: [input-file] [options]\n";
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

  // Benchmark execution -------------------------------------------------------

  SBG::Eval::setSetFactory(*_set_impl);
  SBG::Eval::setPWFactory(*_pw_impl);
  SBG::Eval::setSCCFactory(*_scc_impl);
  printHeader(vm);

  if (vm.count("boost")) {
    if (_input_file) {
      registerBoostBenchmarks(*_input_file);
    } else {
      Util::ERROR("BoostExecutor: must provide a SBG program filename to run "
        , "Boost benchmark\n");
    }
  } else {
    switch (*_benchmark) {
      case 0: {
        registerSetBenchmarks();
        break;
      }

      case 1: {
        registerPWMapBenchmarks();
        break;
      }

      case 2: {
        if (_input_file) {
          registerMatchingBenchmarks(*_input_file);
        } else {
          registerMatchingBenchmarks();
        }
        break;
      }

      case 3: {
        if (_input_file) {
          registerSCCBenchmarks(*_input_file);
        } else {
          registerSCCBenchmarks();
        }
        break;
      }

      default: {
        break;
      }
    }
  }

  Util::prog_opts::parsed_options parsed
    = Util::prog_opts::command_line_parser(argc, argv).options(_cmd_line_opts)
     .allow_unregistered().run();
  std::vector<std::string> to_pass_further
    = Util::prog_opts::collect_unrecognized(parsed.options
      , Util::prog_opts::exclude_positional);
  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();
}

} // namespace detail

} // namespace perf

} // namespace SBG
