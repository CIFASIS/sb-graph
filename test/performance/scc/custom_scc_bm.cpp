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

/**
 * @file custom_scc_bm.cpp
 * @brief Executes the SBG version of the SCC algorithm for the desired file. It
 * is used to showcase the constant execution time when the repetitive patterns
 * increase its size. The final benchmark increases the number of repetitve
 * patterns, copying the original graph several times.
 * @note The input .test file should define a variable N that describes the
 * "size" of the repetitive patterns of the graph. 
 */

#include "algorithms/scc/scc.hpp"
#include "algorithms/scc/scc_fact.hpp"
#include "eval/user_impl_map.hpp"
#include "sbg/directed_sbg.hpp"
#include "test/performance/utils.hpp"

#include <benchmark/benchmark.h>

namespace Test {

namespace Internal {

////////////////////////////////////////////////////////////////////////////////
// Auxiliary data --------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

int lower = 100;
int mult = 10;
int upper = 1e6;

const char *filename = std::getenv("TEST_FILE");

////////////////////////////////////////////////////////////////////////////////
// Benchmarks ------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

static void BM_CustomSCCTest(benchmark::State& state)
{
  int N = state.range(0);
  SBG::LIB::MatchData match_result = calculateMatching(filename, N, 1);
  SBG::LIB::DirectedSBG scc_dsbg = misc::buildSCCFromMatching(match_result); 
  SBG::LIB::SCC scc_algorithm = SBG::LIB::SCC_FACT.createSCCAlgorithm();

  for (auto _ : state) {
    scc_algorithm.calculate(scc_dsbg);
  }
  state.SetComplexityN(N);
}
BENCHMARK(BM_CustomSCCTest)->RangeMultiplier(mult)->Range(lower, upper)
  ->Complexity()->Unit(benchmark::kMillisecond);

static void BM_CustomSCCTestWithBuilder(benchmark::State& state)
{
  int N = state.range(0);
  SBG::LIB::MatchData match_result = calculateMatching(filename, N, 1);
  SBG::LIB::SCC scc_algorithm = SBG::LIB::SCC_FACT.createSCCAlgorithm();

  for (auto _ : state) {
    SBG::LIB::DirectedSBG scc_dsbg = misc::buildSCCFromMatching(match_result); 
    scc_algorithm.calculate(scc_dsbg);
  }
  state.SetComplexityN(N);
}
BENCHMARK(BM_CustomSCCTestWithBuilder)->RangeMultiplier(mult)
  ->Range(lower, upper)->Complexity()->Unit(benchmark::kMillisecond);

static void BM_CustomSCCTestCopies(benchmark::State& state)
{
  int N = state.range(0);
  SBG::LIB::MatchData match_result = calculateMatching(filename, 100, N);
  SBG::LIB::SCC scc_algorithm = SBG::LIB::SCC_FACT.createSCCAlgorithm();

  for (auto _ : state) {
    SBG::LIB::DirectedSBG scc_dsbg = misc::buildSCCFromMatching(match_result); 
    scc_algorithm.calculate(scc_dsbg);
  }
  state.SetComplexityN(N);
}
BENCHMARK(BM_CustomSCCTestCopies)->RangeMultiplier(2)->Range(1, 128)
  ->Complexity()->Unit(benchmark::kMillisecond);

} // namespace Internal

} // namespace Test

////////////////////////////////////////////////////////////////////////////////
// Main ------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
  if (Test::Internal::filename) {
    const char *set_impl = std::getenv("SET_IMPL");
    const char *pw_impl = std::getenv("PW_IMPL");
    SBG::Eval::setSetFactory(set_impl ? std::stoi(set_impl) : 1);
    SBG::Eval::setPWFactory(pw_impl ? std::stoi(pw_impl) : 1);

    ::benchmark::Initialize(&argc, argv);
    ::benchmark::RunSpecifiedBenchmarks();
  }

  return 0;
}
