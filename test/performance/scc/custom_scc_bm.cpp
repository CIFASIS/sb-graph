/*****************************************************************************

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

#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <regex>

#include <benchmark/benchmark.h>

#include "algorithms/misc/causalization_builders.hpp"
#include "eval/file_evaluator.cpp"
#include "eval/user_impl_map.hpp"
#include "eval/visitors/autom_impl_visitor.hpp"
#include "parser/file_parser.hpp"

namespace Test {

namespace Internal {

using SBG::LIB::RATIONAL;
using SBG::LIB::Interval;
using SBG::LIB::SetPiece;
using SBG::LIB::Set;
using SBG::LIB::LExp;
using SBG::LIB::Exp;
using SBG::LIB::Map;
using SBG::LIB::PWMap;

////////////////////////////////////////////////////////////////////////////////
// Auxiliary functions ---------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Replaces the value of N with the desired value in the designated
 * .test file.
 */
bool updateN(const std::string& filename, int N)
{
    std::ifstream input_file(filename);
    if (!input_file.is_open()) {
      std::cerr << "Error>> Unable to open file " << filename << "\n";
      return false;
    }

    std::string new_content = "";
    std::string line;
    std::regex pattern("^(\\s*N\\s*=\\s*).*");
    bool found = false;
    while (std::getline(input_file, line)) {
      std::smatch common;
      if (std::regex_search(line, common, pattern)) {
        line = common[1].str() + std::to_string(N) + ";";
        found = true;
      }
      new_content += line + "\n";
    }
    input_file.close();

    if (!found) {
      std::cout << "\n";
      return false;
    }

    std::ofstream output_file(filename);
    output_file << new_content;
    output_file.close();

    return true;
}

/**
 * @brief Reads a .test file to search for an SBG that will be the input for
 * the causalization workflow.
 */
SBG::LIB::MatchData calculateMatching(std::string filename, int N)
{
  std::streambuf* original_buf = std::cout.rdbuf();
  std::ofstream nullStream("/dev/null");
  std::cout.rdbuf(nullStream.rdbuf());

  // Set value of N in file
  updateN(filename, N);

  // Get graph from file
  SBG::LIB::SBG g;
  SBG::Eval::ProgramIO eval_result = SBG::Eval::parseEvalFile(filename); 
  for (const SBG::Eval::ExprResult &ev : eval_result.exprs()) {
    SBG::Eval::ExprBaseType e = std::get<1>(ev);
    if (std::holds_alternative<SBG::LIB::SBG>(e)) {
      g = std::get<SBG::LIB::SBG>(e);
    }
  }

  // Calculate matching
  SBG::LIB::Matching match_algorithm
    = SBG::LIB::MATCH_FACT.createMatchAlgorithm();
  SBG::LIB::MatchData match_result = match_algorithm.calculate(g);

  std::cout.rdbuf(original_buf);

  return match_result;  
}

////////////////////////////////////////////////////////////////////////////////
// Benchmarks ------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

static void BM_CustomSCCTest(benchmark::State& state)
{
  int N = state.range(0);

  // Calculate SCC
  const char* filename = std::getenv("TEST_FILE");
  if (filename) {
    const char* set_impl = std::getenv("SET_IMPL");
    const char* pw_impl = std::getenv("PW_IMPL");
    SBG::Eval::setSetFactory(set_impl ? std::stoi(set_impl) : 1);
    SBG::Eval::setPWFactory(pw_impl ? std::stoi(pw_impl) : 1);

    SBG::LIB::SCC scc_algorithm = SBG::LIB::SCC_FACT.createSCCAlgorithm();
    SBG::LIB::MatchData match_result = calculateMatching(filename, N);
    SBG::LIB::DSBG scc_dsbg = MISC::buildSCCFromMatching(match_result); 

    for (auto _ : state) {
      scc_algorithm.calculate(scc_dsbg);
    }
    state.SetComplexityN(N);
  }
}
BENCHMARK(BM_CustomSCCTest)->RangeMultiplier(10)->Range(100, 1e6)->Complexity()
  ->Unit(benchmark::kMillisecond);

static void BM_CustomSCCTestWithBuilder(benchmark::State& state)
{
  int N = state.range(0);

  // Calculate SCC
  const char* filename = std::getenv("TEST_FILE");
  if (filename) {
    const char* set_impl = std::getenv("SET_IMPL");
    const char* pw_impl = std::getenv("PW_IMPL");
    SBG::Eval::setSetFactory(set_impl ? std::stoi(set_impl) : 1);
    SBG::Eval::setPWFactory(pw_impl ? std::stoi(pw_impl) : 1);

    SBG::LIB::SCC scc_algorithm = SBG::LIB::SCC_FACT.createSCCAlgorithm();
    SBG::LIB::MatchData match_result = calculateMatching(filename, N);

    for (auto _ : state) {
      SBG::LIB::DSBG scc_dsbg = MISC::buildSCCFromMatching(match_result); 
      scc_algorithm.calculate(scc_dsbg);
    }
    state.SetComplexityN(N);
  }
}
BENCHMARK(BM_CustomSCCTestWithBuilder)->RangeMultiplier(10)->Range(100, 1e6)
  ->Complexity()->Unit(benchmark::kMillisecond);

} // namespace Internal

} // namespace Test
