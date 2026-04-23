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

#include "algorithms/matching/match_data.hpp"
#include "algorithms/scc/scc.hpp"
#include "algorithms/scc/scc_fact.hpp"
#include "algorithms/misc/causalization_builders.hpp"
#include "sbg/directed_sbg.hpp"
#include "test/performance/scc_bm.hpp"
#include "test/performance/utils.hpp"

#include <benchmark/benchmark.h>

namespace SBG {

namespace perf {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Benchmarks ------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

static void BM_SCC(benchmark::State& state, std::string filename)
{
  int N = state.range(0);
  SBG::LIB::MatchData match_result = calculateMatching(filename, N, 1);
  SBG::LIB::DirectedSBG scc_dsbg = misc::buildLoopDetectionSBG(match_result); 
  SBG::LIB::SCC scc_algorithm = SBG::LIB::SCC_FACT.createSCCAlgorithm();

  for (auto _ : state) {
    benchmark::DoNotOptimize(scc_algorithm.calculate(scc_dsbg));
  }
  state.SetComplexityN(N);
}

static void BM_SCCWithBuilder(benchmark::State& state, std::string filename)
{
  int N = state.range(0);
  SBG::LIB::MatchData match_result = calculateMatching(filename, 100, N);
  SBG::LIB::SCC scc_algorithm = SBG::LIB::SCC_FACT.createSCCAlgorithm();

  for (auto _ : state) {
    SBG::LIB::DirectedSBG scc_dsbg = misc::buildLoopDetectionSBG(match_result); 
    benchmark::DoNotOptimize(scc_algorithm.calculate(scc_dsbg));
  }
  state.SetComplexityN(N);
}

static void BM_SCCCopies(benchmark::State& state, std::string filename)
{
  int N = state.range(0);
  SBG::LIB::MatchData match_result = calculateMatching(filename, 100, N);
  SBG::LIB::SCC scc_algorithm = SBG::LIB::SCC_FACT.createSCCAlgorithm();

  for (auto _ : state) {
    SBG::LIB::DirectedSBG scc_dsbg = misc::buildLoopDetectionSBG(match_result); 
    benchmark::DoNotOptimize(scc_algorithm.calculate(scc_dsbg));
  }
  state.SetComplexityN(N);
}

////////////////////////////////////////////////////////////////////////////////
// Register Benchmarks ---------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

void registerSCCBenchmarks(std::string filename)
{
  benchmark::RegisterBenchmark(
    ("BM_SCC/" + filename).c_str(),
    [filename](benchmark::State& state) {
      BM_SCC(state, filename);
    }
  )->RangeMultiplier(10)->Range(100, 1e6)->Complexity()
    ->Unit(benchmark::kMillisecond);

  benchmark::RegisterBenchmark(
    ("BM_SCCWithBuilder/" + filename).c_str(),
    [filename](benchmark::State& state) {
      BM_SCCWithBuilder(state, filename);
    }
  )->RangeMultiplier(10)->Range(100, 1e6)->Complexity()
    ->Unit(benchmark::kMillisecond);

  benchmark::RegisterBenchmark(
    ("BM_SCCCopies/" + filename).c_str(),
    [filename](benchmark::State& state) {
      BM_SCCCopies(state, filename);
    }
  )->RangeMultiplier(2)->Range(1, 128)->Complexity()
    ->Unit(benchmark::kMillisecond);
}

void registerSCCBenchmarks()
{
  registerSCCBenchmarks("../../TestRL1.test");
  registerSCCBenchmarks("../../TestRL2.test");
  registerSCCBenchmarks("../../TestRL3.test");
}

} // namespace detail

} // namespace perf

} // namespace SBG
