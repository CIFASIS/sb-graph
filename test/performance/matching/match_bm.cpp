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
 * @file match_bm.cpp
 * @brief Executes the SBG version of the matching algorithm for TestRL1.test
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

  // Calculate Matching
  SBG::LIB::Matching match_algorithm
    = SBG::LIB::MATCH_FACT.createMatchAlgorithm();
  SBG::LIB::BipartiteSBG match_sbg = generateSBG("../../TestRL1.test", N, 1);

  for (auto _ : state) {
    match_algorithm.calculate(match_sbg);
  }
  state.SetComplexityN(N);
}
BENCHMARK(BM_TestRL1)->RangeMultiplier(10)->Range(100, 1e6)->Complexity()
  ->Unit(benchmark::kMillisecond);

static void BM_TestRL2(benchmark::State& state)
{
  int N = state.range(0);
  SBG::Eval::setSetFactory(2);
  SBG::Eval::setPWFactory(0);

  // Calculate Matching
  SBG::LIB::Matching match_algorithm
    = SBG::LIB::MATCH_FACT.createMatchAlgorithm();
  SBG::LIB::BipartiteSBG match_sbg = generateSBG("../../TestRL2.test", N, 1);

  for (auto _ : state) {
    match_algorithm.calculate(match_sbg);
  }

  state.SetComplexityN(N);
}
BENCHMARK(BM_TestRL2)->RangeMultiplier(10)->Range(100, 1e6)->Complexity()
  ->Unit(benchmark::kMillisecond);

static void BM_TestRL3(benchmark::State& state)
{
  int N = state.range(0);
  SBG::Eval::setSetFactory(2);
  SBG::Eval::setPWFactory(1);

  // Calculate Matching
  SBG::LIB::Matching match_algorithm
    = SBG::LIB::MATCH_FACT.createMatchAlgorithm();
  SBG::LIB::BipartiteSBG match_sbg = generateSBG("../../TestRL3.test", N, 1);

  for (auto _ : state) {
    match_algorithm.calculate(match_sbg);
  }
  state.SetComplexityN(N);
}
BENCHMARK(BM_TestRL3)->RangeMultiplier(10)->Range(100, 1e6)->Complexity()
  ->Unit(benchmark::kMillisecond);

static void BM_TestRL1Copies(benchmark::State& state)
{
  int N = state.range(0);
  SBG::Eval::setSetFactory(2);
  SBG::Eval::setPWFactory(2);

  // Calculate Matching
  SBG::LIB::Matching match_algorithm
    = SBG::LIB::MATCH_FACT.createMatchAlgorithm();
  SBG::LIB::BipartiteSBG match_sbg = generateSBG("../../TestRL1.test", 100, N);

  for (auto _ : state) {
    match_algorithm.calculate(match_sbg);
  }
  state.SetComplexityN(N);
}
BENCHMARK(BM_TestRL1Copies)->RangeMultiplier(2)->Range(1, 128)->Complexity()
  ->Unit(benchmark::kMillisecond);

static void BM_TestRL2Copies(benchmark::State& state)
{
  int N = state.range(0);
  SBG::Eval::setSetFactory(2);
  SBG::Eval::setPWFactory(2);

  // Calculate Matching
  SBG::LIB::Matching match_algorithm
    = SBG::LIB::MATCH_FACT.createMatchAlgorithm();
  SBG::LIB::BipartiteSBG match_sbg = generateSBG("../../TestRL2.test", 100, N);

  for (auto _ : state) {
    match_algorithm.calculate(match_sbg);
  }
  state.SetComplexityN(N);
}
BENCHMARK(BM_TestRL2Copies)->RangeMultiplier(2)->Range(1, 128)->Complexity()
  ->Unit(benchmark::kMillisecond);

static void BM_TestRL3Copies(benchmark::State& state)
{
  int N = state.range(0);
  SBG::Eval::setSetFactory(2);
  SBG::Eval::setPWFactory(2);

  // Calculate Matching
  SBG::LIB::Matching match_algorithm
    = SBG::LIB::MATCH_FACT.createMatchAlgorithm();
  SBG::LIB::BipartiteSBG match_sbg = generateSBG("../../TestRL3.test", 100, N);

  for (auto _ : state) {
    match_algorithm.calculate(match_sbg);
  }
  state.SetComplexityN(N);
}
BENCHMARK(BM_TestRL3Copies)->RangeMultiplier(2)->Range(1, 128)->Complexity()
  ->Unit(benchmark::kMillisecond);

} // namespace Internal

} // namespace Test
