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
 * @file scc_bm.cpp
 * @brief Executes the SBG version of the SCC algorithm for TestRL1.test
 * , TestRL2.test and TestRL3.test. It is used to showcase the constant
 * execution time when the repetitive patterns increase its size.
 */

#include <benchmark/benchmark.h>

#include "eval/user_impl_map.hpp"
#include "test/performance/utils.hpp"

namespace Test {

namespace Internal {

////////////////////////////////////////////////////////////////////////////////
// Benchmarks ------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

static void BM_TestRL1(benchmark::State& state)
{
  int N = state.range(0);
  SBG::Eval::setSetFactory(2);
  SBG::Eval::setPWFactory(0);

  // Calculate SCC
  SBG::LIB::SCC scc_algorithm = SBG::LIB::SCC_FACT.createSCCAlgorithm();
  SBG::LIB::MatchData match_result
    = calculateMatching("../../TestRL1.test", N, 1);
  SBG::LIB::DSBG scc_dsbg = MISC::buildSCCFromMatching(match_result); 

  for (auto _ : state) {
    scc_algorithm.calculate(scc_dsbg);
  }
  state.SetComplexityN(N);
}
BENCHMARK(BM_TestRL1)->RangeMultiplier(10)->Range(100, 1e6)->Complexity()
  ->Unit(benchmark::kMillisecond);

static void BM_TestRL1WithBuilder(benchmark::State& state)
{
  int N = state.range(0);
  SBG::Eval::setSetFactory(2);
  SBG::Eval::setPWFactory(0);

  // Calculate SCC
  SBG::LIB::SCC scc_algorithm = SBG::LIB::SCC_FACT.createSCCAlgorithm();
  SBG::LIB::MatchData match_result
    = calculateMatching("../../TestRL1.test", N, 1);

  for (auto _ : state) {
    SBG::LIB::DSBG scc_dsbg = MISC::buildSCCFromMatching(match_result); 
    scc_algorithm.calculate(scc_dsbg);
  }
  state.SetComplexityN(N);
}
BENCHMARK(BM_TestRL1WithBuilder)->RangeMultiplier(10)->Range(100, 1e6)
  ->Complexity()->Unit(benchmark::kMillisecond);

static void BM_TestRL2(benchmark::State& state)
{
  int N = state.range(0);
  SBG::Eval::setSetFactory(2);
  SBG::Eval::setPWFactory(0);

  // Calculate SCC
  SBG::LIB::SCC scc_algorithm = SBG::LIB::SCC_FACT.createSCCAlgorithm();
  SBG::LIB::MatchData match_result
    = calculateMatching("../../TestRL2.test", N, 1);
  SBG::LIB::DSBG scc_dsbg = MISC::buildSCCFromMatching(match_result); 

  for (auto _ : state) {
    scc_algorithm.calculate(scc_dsbg);
  }
  state.SetComplexityN(N);
}
BENCHMARK(BM_TestRL2)->RangeMultiplier(10)->Range(100, 1e6)->Complexity()
  ->Unit(benchmark::kMillisecond);

static void BM_TestRL2WithBuilder(benchmark::State& state)
{
  int N = state.range(0);
  SBG::Eval::setSetFactory(2);
  SBG::Eval::setPWFactory(0);

  // Calculate SCC
  SBG::LIB::SCC scc_algorithm = SBG::LIB::SCC_FACT.createSCCAlgorithm();
  SBG::LIB::MatchData match_result
    = calculateMatching("../../TestRL2.test", N, 1);

  for (auto _ : state) {
    SBG::LIB::DSBG scc_dsbg = MISC::buildSCCFromMatching(match_result); 
    scc_algorithm.calculate(scc_dsbg);
  }
  state.SetComplexityN(N);
}
BENCHMARK(BM_TestRL2WithBuilder)->RangeMultiplier(10)->Range(100, 1e6)
  ->Complexity()->Unit(benchmark::kMillisecond);

static void BM_TestRL3(benchmark::State& state)
{
  int N = state.range(0);
  SBG::Eval::setSetFactory(2);
  SBG::Eval::setPWFactory(1);

  // Calculate SCC
  SBG::LIB::SCC scc_algorithm = SBG::LIB::SCC_FACT.createSCCAlgorithm();
  SBG::LIB::MatchData match_result
    = calculateMatching("../../TestRL3.test", N, 1);
  SBG::LIB::DSBG scc_dsbg = MISC::buildSCCFromMatching(match_result); 

  for (auto _ : state) {
    scc_algorithm.calculate(scc_dsbg);
  }
  state.SetComplexityN(N);
}
BENCHMARK(BM_TestRL3)->RangeMultiplier(10)->Range(100, 1e6)->Complexity()
  ->Unit(benchmark::kMillisecond);

static void BM_TestRL3WithBuilder(benchmark::State& state)
{
  int N = state.range(0);
  SBG::Eval::setSetFactory(2);
  SBG::Eval::setPWFactory(1);

  // Calculate SCC
  SBG::LIB::SCC scc_algorithm = SBG::LIB::SCC_FACT.createSCCAlgorithm();
  SBG::LIB::MatchData match_result
    = calculateMatching("../../TestRL3.test", N, 1);

  for (auto _ : state) {
    SBG::LIB::DSBG scc_dsbg = MISC::buildSCCFromMatching(match_result); 
    scc_algorithm.calculate(scc_dsbg);
  }
  state.SetComplexityN(N);
}
BENCHMARK(BM_TestRL3WithBuilder)->RangeMultiplier(10)->Range(100, 1e6)
  ->Complexity()->Unit(benchmark::kMillisecond);

} // namespace Internal

} // namespace Test
