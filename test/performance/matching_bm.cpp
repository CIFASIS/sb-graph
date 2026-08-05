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

#include "test/performance/matching_bm.hpp"
#include "algorithms/matching/matching.hpp"
#include "sbg/bipartite_sbg.hpp"
#include "test/performance/utils.hpp"

#include <benchmark/benchmark.h>

namespace SBG {

namespace perf {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Benchmarks ------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

static void BM_Matching(benchmark::State& state, std::string filename)
{
  int N = state.range(0);

  SBG::LIB::Matching match_algorithm;
  SBG::LIB::BipartiteSBG match_sbg = generateSBG(filename, N, 1); 
  for (auto _ : state) {
    benchmark::DoNotOptimize(match_algorithm.calculate(match_sbg));
  }

  state.SetComplexityN(N);
}

static void BM_MatchingCopies(benchmark::State& state, std::string filename)
{
  int N = state.range(0);

  // Calculate Matching
  SBG::LIB::Matching match_algorithm;
  SBG::LIB::BipartiteSBG match_sbg = generateSBG(filename, 100, N);

  for (auto _ : state) {
    benchmark::DoNotOptimize(match_algorithm.calculate(match_sbg));
  }
  state.SetComplexityN(N);
}

////////////////////////////////////////////////////////////////////////////////
// Register Benchmarks ---------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

void registerMatchingBenchmarks(std::string filename)
{
  benchmark::RegisterBenchmark(
    ("BM_Matching/" + filename).c_str(),
    [filename](benchmark::State& state) {
      BM_Matching(state, filename);
    }
  )->RangeMultiplier(10)->Range(100, 1e6)->Complexity()
    ->Unit(benchmark::kMillisecond);

  benchmark::RegisterBenchmark(
    ("BM_MatchingCopies/" + filename).c_str(),
    [filename](benchmark::State& state) {
      BM_MatchingCopies(state, filename);
    }
  )->RangeMultiplier(2)->Range(1, 128)->Complexity()
    ->Unit(benchmark::kMillisecond);
}

void registerMatchingBenchmarks()
{
  registerMatchingBenchmarks("../../TestRL1.test");
  registerMatchingBenchmarks("../../TestRL2.test");
  registerMatchingBenchmarks("../../TestRL3.test");
}

} // namespace detail

} // namespace perf

} // namespace SBG
