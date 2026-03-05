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

#include <benchmark/benchmark.h>

#include <iostream>
#include <utility>

namespace Test {

namespace Internal {

using SBG::LIB::NAT;
using SBG::LIB::detail::Interval;
using SBG::LIB::detail::SetPiece;
using SBG::LIB::SetKind;
using SBG::LIB::Set;

////////////////////////////////////////////////////////////////////////////////
// Auxiliary functions ---------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Test suite created to analyze the time growth of the different ordered
 * set operations.
 */
std::pair<Set, Set> contiguousPieces(int set_sz)
{
  SBG::LIB::NAT inter_sz = 100;

  Set s1 = SBG::LIB::SET_FACT.createSet();
  Set s2 = SBG::LIB::SET_FACT.createSet();
  for (unsigned int h = 0; h < set_sz; ++h) {
    Interval i1(h*inter_sz, 1, (h + 1)*inter_sz - 1);
    Set jth_s1 = SBG::LIB::SET_FACT.createSet(i1.begin(), i1.step()
      , i1.end());
    s1 = s1.disjointCup(jth_s1);

    SBG::LIB::NAT off = inter_sz/2;
    Interval i2(off + (h*inter_sz), 1, off + (h + 1)*inter_sz - 1);
    Set jth_s2 = SBG::LIB::SET_FACT.createSet(i2.begin(), i2.step(), i2.end());
    s2 = s2.disjointCup(jth_s2);
  }

  Set second_dim = SBG::LIB::SET_FACT.createSet(0, 1, inter_sz - 1);
  s1 = s1.cartesianProduct(second_dim);
  s2 = s2.cartesianProduct(second_dim);

  return std::make_pair(std::move(s1), std::move(s2));
}

////////////////////////////////////////////////////////////////////////////////
// Benchmarks ------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

static void BM_UnordSetEq(benchmark::State& state)
{
  int set_sz = state.range(0);
  SBG::LIB::SET_FACT.set_set_fact(SetKind::kUnordered);
  auto [s1, s2] = contiguousPieces(set_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(s1 == s2);
  }
  state.SetComplexityN(set_sz);
}
BENCHMARK(BM_UnordSetEq)->RangeMultiplier(10)->Range(10, 1e3)->Complexity();

static void BM_UnordSetCap(benchmark::State& state)
{
  int set_sz = state.range(0);
  SBG::LIB::SET_FACT.set_set_fact(SetKind::kUnordered);
  auto [s1, s2] = contiguousPieces(set_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(s1.intersection(s2));
  }
  state.SetComplexityN(set_sz);
}
BENCHMARK(BM_UnordSetCap)->RangeMultiplier(10)->Range(10, 1e3)->Complexity();

static void BM_UnordSetCup(benchmark::State& state)
{
  int set_sz = state.range(0);
  SBG::LIB::SET_FACT.set_set_fact(SetKind::kUnordered);
  auto [s1, s2] = contiguousPieces(set_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(s1.cup(s2));
  }
  state.SetComplexityN(set_sz);
}
BENCHMARK(BM_UnordSetCup)->RangeMultiplier(10)->Range(10, 1e3)->Complexity();

static void BM_UnordSetComplement(benchmark::State& state)
{
  int set_sz = state.range(0);
  SBG::LIB::SET_FACT.set_set_fact(SetKind::kUnordered);
  auto [s1, s2] = contiguousPieces(set_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(s1.complement());
  }
  state.SetComplexityN(set_sz);
}
BENCHMARK(BM_UnordSetComplement)->RangeMultiplier(10)->Range(10, 1e3)
  ->Complexity();

static void BM_UnordSetDiff(benchmark::State& state)
{
  int set_sz = state.range(0);
  SBG::LIB::SET_FACT.set_set_fact(SetKind::kUnordered);
  auto [s1, s2] = contiguousPieces(set_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(s1.difference(s2));
  }
  state.SetComplexityN(set_sz);
}
BENCHMARK(BM_UnordSetDiff)->RangeMultiplier(10)->Range(10, 1e3)->Complexity();

static void BM_UnordSetDisjointUnion(benchmark::State& state)
{
  int set_sz = state.range(0);
  SBG::LIB::SET_FACT.set_set_fact(SetKind::kUnordered);
  Set s1 = SBG::LIB::SET_FACT.createSet();
  Set s2 = SBG::LIB::SET_FACT.createSet();
  for (SBG::LIB::NAT j = 0; j < set_sz; j += 2) {
    Set jth_s1 = SBG::LIB::SET_FACT.createSet(j*100 + 1, 1, (j + 1)*100);
    s1 = std::move(s1.disjointCup(jth_s1));
    Set jth_s2 = SBG::LIB::SET_FACT.createSet((j + 1)*100+1, 1, (j + 2)*100);
    s2 = std::move(s2.disjointCup(jth_s2));
  }

  for (auto _ : state) {
    benchmark::DoNotOptimize(std::move(s1).disjointCup(std::move(s2)));
  }
  state.SetComplexityN(set_sz);
}
BENCHMARK(BM_UnordSetDisjointUnion)->RangeMultiplier(10)->Range(10, 1e3)
  ->Complexity();

static void BM_UnordSetCompact(benchmark::State& state)
{
  int set_sz = state.range(0);
  SBG::LIB::SET_FACT.set_set_fact(SetKind::kUnordered);
  auto [s1, s2] = contiguousPieces(set_sz);

  for (auto _ : state) {
    s1.compact();
  }
  state.SetComplexityN(set_sz);
}
BENCHMARK(BM_UnordSetCompact)->RangeMultiplier(10)->Range(10, 1e2)
  ->Complexity();

} // namespace Internal

} // namespace Test
