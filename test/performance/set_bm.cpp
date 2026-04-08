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

#include "sbg/interval.hpp"
#include "sbg/multidim_inter.hpp"
#include "sbg/natural.hpp"
#include "sbg/set.hpp"
#include "sbg/set_fact.hpp"
#include "test/performance/utils.hpp"

#include <benchmark/benchmark.h>

#include <iostream>
#include <utility>

namespace SBG {

namespace perf {

namespace detail {

using SBG::LIB::NAT;
using SBG::LIB::detail::Interval;
using SBG::LIB::SetKind;
using SBG::LIB::Set;

////////////////////////////////////////////////////////////////////////////////
// Benchmarks ------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

static void BM_SetEq(benchmark::State& state)
{
  int set_sz = state.range(0);
  auto [s1, s2] = nonDisjointPieces(set_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(s1 == s2);
  }
  state.SetComplexityN(set_sz);
}

static void BM_SetCap(benchmark::State& state)
{
  int set_sz = state.range(0);
  auto [s1, s2] = nonDisjointPieces(set_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(s1.intersection(s2));
  }
  state.SetComplexityN(set_sz);
}

static void BM_SetCup(benchmark::State& state)
{
  int set_sz = state.range(0);
  auto [s1, s2] = nonDisjointPieces(set_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(s1.cup(s2));
  }
  state.SetComplexityN(set_sz);
}

static void BM_SetComplement(benchmark::State& state)
{
  int set_sz = state.range(0);
  auto [s1, s2] = nonDisjointPieces(set_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(s1.complement());
  }
  state.SetComplexityN(set_sz);
}

static void BM_SetDiff(benchmark::State& state)
{
  int set_sz = state.range(0);
  auto [s1, s2] = nonDisjointPieces(set_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(s1.difference(s2));
  }
  state.SetComplexityN(set_sz);
}

static void BM_SetDisjointUnion(benchmark::State& state)
{
  int set_sz = state.range(0);
  auto [s1, s2] = interlacedPieces(set_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(std::move(s1).disjointCup(std::move(s2)));
  }
  state.SetComplexityN(set_sz);
}

static void BM_SetCompact(benchmark::State& state)
{
  int set_sz = state.range(0);
  auto [s1, s2] = nonDisjointPieces(set_sz);

  for (auto _ : state) {
    s1.compact();
    benchmark::DoNotOptimize(s1);
  }
  state.SetComplexityN(set_sz);
}

////////////////////////////////////////////////////////////////////////////////
// Register Benchmarks ---------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

void registerSetBenchmarks()
{
  BENCHMARK(BM_SetEq)->RangeMultiplier(10)->Range(10, 1e3)->Complexity();
  
  BENCHMARK(BM_SetCap)->RangeMultiplier(10)->Range(10, 1e3)->Complexity();
  
  BENCHMARK(BM_SetCup)->RangeMultiplier(10)->Range(10, 1e3)->Complexity();
  
  BENCHMARK(BM_SetComplement)->RangeMultiplier(10)->Range(10, 1e3)
    ->Complexity();
  
  BENCHMARK(BM_SetDiff)->RangeMultiplier(10)->Range(10, 1e3)->Complexity();
  
  BENCHMARK(BM_SetDisjointUnion)->RangeMultiplier(10)->Range(10, 1e3)
    ->Complexity();
  
  BENCHMARK(BM_SetCompact)->RangeMultiplier(10)->Range(10, 1e2)
    ->Complexity();
}

} // namespace detail

} // namespace perf

} // namespace SBG
