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

#include "sbg/set.hpp"
#include "sbg/pw_map.hpp"
#include "sbg/pwmap_detail.hpp"
#include "test/performance/utils.hpp"

#include <benchmark/benchmark.h>

#include <utility>

namespace SBG {

namespace perf {

namespace detail {

using SBG::LIB::Set;

////////////////////////////////////////////////////////////////////////////////
// Benchmarks ------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

static void BM_PWMapDenseDom(benchmark::State& state)
{
  int map_sz = state.range(0);
  auto pw = denseDom(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw.domain());
  }
  state.SetComplexityN(map_sz);
}

static void BM_PWMapEq(benchmark::State& state)
{
  int map_sz = state.range(0);
  auto [pw1, pw2] = nonDisjointMaps(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1 == pw2);
  }
  state.SetComplexityN(map_sz);
}

static void BM_PWMapSum(benchmark::State& state)
{
  int map_sz = state.range(0);
  auto [pw1, pw2] = nonDisjointMaps(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1 + pw2);
  }
  state.SetComplexityN(map_sz);
}

static void BM_PWMapDom(benchmark::State& state)
{
  int map_sz = state.range(0);
  auto [pw1, pw2] = nonDisjointMaps(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.domain());
  }
  state.SetComplexityN(map_sz);
}

static void BM_PWMapRestrict(benchmark::State& state)
{
  int map_sz = state.range(0);
  auto [pw1, pw2] = nonDisjointMaps(map_sz);
  Set domain2 = pw2.domain();

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.restrict(domain2));
  }
  state.SetComplexityN(map_sz);
}

static void BM_PWMapImage(benchmark::State& state)
{
  int map_sz = state.range(0);
  auto [pw1, pw2] = nonDisjointMaps(map_sz);
  Set domain2 = pw2.domain();

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.image(domain2));
  }
  state.SetComplexityN(map_sz);
}

static void BM_PWMapPreImage(benchmark::State& state)
{
  int map_sz = state.range(0);
  auto [pw1, pw2] = nonDisjointMaps(map_sz);
  Set img2 = pw2.image();

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.preImage(img2));
  }
  state.SetComplexityN(map_sz);
}

static void BM_PWMapComposition(benchmark::State& state)
{
  int map_sz = state.range(0);
  auto [pw1, pw2] = nonDisjointMaps(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.composition(pw2));
  }
  state.SetComplexityN(map_sz);
}

static void BM_PWMapConcat(benchmark::State& state)
{
  int map_sz = state.range(0);
  auto [pw1, pw2] = interlacedMaps(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.concatenation(pw2));
  }
  state.SetComplexityN(map_sz);
}

static void BM_PWMapCombine(benchmark::State& state)
{
  int map_sz = state.range(0);
  auto [pw1, pw2] = nonDisjointMaps(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.combine(pw2));
  }
  state.SetComplexityN(map_sz);
}

static void BM_PWMapReduce(benchmark::State& state)
{
  int map_sz = state.range(0);
  PWMap pw = reducibleMaps(map_sz);

  LIB::detail::PWMapAccessKey key = LIB::detail::PWMapAccess::key();
  for (auto _ : state) {
    benchmark::DoNotOptimize(key.reduce(pw));
  }
  state.SetComplexityN(map_sz);
}

static void BM_PWMapMinAdj(benchmark::State& state)
{
  int map_sz = state.range(0);
  auto [pw1, pw2] = minAdjMaps(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.minAdj(pw2));
  }
  state.SetComplexityN(map_sz);
}

static void BM_PWMapEqImage(benchmark::State& state)
{
  int map_sz = state.range(0);
  auto [pw1, pw2] = nonDisjointMaps(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.equalImage(pw2));
  }
  state.SetComplexityN(map_sz);
}

static void BM_PWMapLessImage(benchmark::State& state)
{
  int map_sz = state.range(0);
  auto [pw1, pw2] = nonDisjointMaps(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.lessImage(pw2));
  }
  state.SetComplexityN(map_sz);
}

static void BM_PWMapCompact(benchmark::State& state)
{
  int map_sz = state.range(0);
  auto [pw1, pw2] = nonDisjointMaps(map_sz);

  for (auto _ : state) {
    pw1.compact();
    benchmark::DoNotOptimize(pw1);
  }
  state.SetComplexityN(map_sz);
}

////////////////////////////////////////////////////////////////////////////////
// Register Benchmarks ---------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

void registerPWMapBenchmarks()
{
  BENCHMARK(BM_PWMapDenseDom)->RangeMultiplier(10)->Range(10, 1e3)
    ->Complexity();
  
  BENCHMARK(BM_PWMapEq)->RangeMultiplier(10)->Range(10, 1e2)->Complexity();
  
  BENCHMARK(BM_PWMapSum)->RangeMultiplier(10)->Range(10, 1e3)->Complexity();
  
  BENCHMARK(BM_PWMapDom)->RangeMultiplier(10)->Range(10, 1e3)->Complexity();
  
  BENCHMARK(BM_PWMapRestrict)->RangeMultiplier(10)->Range(10, 1e3)
    ->Complexity();
  
  BENCHMARK(BM_PWMapImage)->RangeMultiplier(10)->Range(10, 1e3)->Complexity();
  
  BENCHMARK(BM_PWMapPreImage)->RangeMultiplier(10)->Range(10, 1e3)
    ->Complexity();
  
  BENCHMARK(BM_PWMapComposition)->RangeMultiplier(10)->Range(10, 1e3)
    ->Complexity();
  
  BENCHMARK(BM_PWMapConcat)->RangeMultiplier(10)->Range(10, 1e3)
    ->Complexity();
  
  BENCHMARK(BM_PWMapCombine)->RangeMultiplier(10)->Range(10, 1e3)->Complexity();

  BENCHMARK(BM_PWMapReduce)->RangeMultiplier(10)->Range(10, 1e3)->Complexity();
  
  BENCHMARK(BM_PWMapMinAdj)->RangeMultiplier(10)->Range(10, 1e3)->Complexity();
  
  BENCHMARK(BM_PWMapEqImage)->RangeMultiplier(10)->Range(10, 1e3)
    ->Complexity();
  
  BENCHMARK(BM_PWMapLessImage)->RangeMultiplier(10)->Range(10, 1e3)
    ->Complexity();
  
  BENCHMARK(BM_PWMapCompact)->RangeMultiplier(10)->Range(10, 1e2)
    ->Complexity();
}

} // namespace detail

} // namespace perf

} // namespace SBG
