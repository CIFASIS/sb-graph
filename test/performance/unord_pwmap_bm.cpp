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
#include "sbg/pw_map.hpp"
#include "sbg/pwmap_fact.hpp"

#include <benchmark/benchmark.h>

#include <iostream>
#include <utility>

namespace Test {

namespace Internal {

using SBG::LIB::NAT;
using SBG::LIB::detail::Interval;
using SBG::LIB::detail::MultiDimInter;
using SBG::LIB::SetKind;
using SBG::LIB::Set;
using SBG::LIB::RATIONAL;
using SBG::LIB::detail::LinearExpr;
using SBG::LIB::Expression;
using SBG::LIB::Map;
using SBG::LIB::PWMapKind;
using SBG::LIB::PWMap;

////////////////////////////////////////////////////////////////////////////////
// Auxiliary functions ---------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

PWMap denseDom(SBG::LIB::NAT map_sz)
{
  SBG::LIB::NAT inter_sz = 100;
  SBG::LIB::NAT set_sz = 10;

  PWMap pw = SBG::LIB::PWMAP_FACT.createPWMap();
  for (unsigned int j = 0; j < map_sz; ++j) {
    Set domain = SBG::LIB::SET_FACT.createSet();
    SBG::LIB::NAT off = j*set_sz*inter_sz;
    for (unsigned int h = 0; h < set_sz; ++h) {
      Interval i{off + (h*inter_sz), 1, off + (h + 1)*inter_sz - 1};
      Set jth_domain
        = SBG::LIB::SET_FACT.createSet(i.begin(), i.step(), i.end());
      domain = std::move(domain).disjointCup(std::move(jth_domain));
    }
    Expression multidim_id{1, 0};

    pw.emplaceBack(domain, multidim_id);
  }

  return pw;
}

std::pair<PWMap, PWMap> minAdjMaps(SBG::LIB::NAT map_sz)
{
  SBG::LIB::NAT inter_sz = 100;
  SBG::LIB::NAT set_sz = 10;

  Set second_dim = SBG::LIB::SET_FACT.createSet(0, 1, inter_sz - 1);
  PWMap pw1 = SBG::LIB::PWMAP_FACT.createPWMap();
  PWMap pw2 = SBG::LIB::PWMAP_FACT.createPWMap();
  for (unsigned int j = 0; j < map_sz; ++j) {
    Set domain1 = SBG::LIB::SET_FACT.createSet();
    Set domain2 = SBG::LIB::SET_FACT.createSet();
    SBG::LIB::NAT off = j*set_sz*inter_sz;
    for (unsigned int h = 0; h < set_sz; ++h) {
      Interval i1(off + (h*inter_sz), 1, off + (h + 1)*inter_sz - 1);
      Set jth_domain1 = SBG::LIB::SET_FACT.createSet(i1.begin(), i1.step()
        , i1.end());
      domain1 = std::move(domain1).disjointCup(std::move(jth_domain1));

      SBG::LIB::NAT off2 = off + inter_sz/2;
      Interval i2(off2 + (h*inter_sz), 1, off2 + (h + 1)*inter_sz - 1);
      Set jth_domain2 = SBG::LIB::SET_FACT.createSet(i2.begin(), i2.step()
        , i2.end());
      domain2 = std::move(domain2).disjointCup(std::move(jth_domain2));
    }

    Expression multidim_id{2, 1, 0};
    Expression minus_one{2, 1, RATIONAL{-1, 1}};

    pw1.emplaceBack(domain1, multidim_id);
    pw2.emplaceBack(domain2, minus_one);
  }

  return {pw1, pw2};
}

std::pair<PWMap, PWMap> interlacedMaps(SBG::LIB::NAT map_sz)
{
  SBG::LIB::NAT inter_sz = 100;
  SBG::LIB::NAT set_sz = 10;

  Set second_dim = SBG::LIB::SET_FACT.createSet(0, 1, inter_sz - 1);
  PWMap pw1 = SBG::LIB::PWMAP_FACT.createPWMap();
  PWMap pw2 = SBG::LIB::PWMAP_FACT.createPWMap();
  for (unsigned int j = 0; j < 2*map_sz; j += 2) {
    Set domain1 = SBG::LIB::SET_FACT.createSet();
    Set domain2 = SBG::LIB::SET_FACT.createSet();
    SBG::LIB::NAT off = j*set_sz*inter_sz;
    for (unsigned int h = 0; h < set_sz; h += 2) {
      Interval i1{off + (h*inter_sz), 1, off + (h + 1)*inter_sz - 1};
      Set jth_domain1 = SBG::LIB::SET_FACT.createSet(i1.begin(), i1.step()
        , i1.end());
      domain1 = std::move(domain1).disjointCup(std::move(jth_domain1));

      SBG::LIB::NAT off2 = off + inter_sz;
      Interval i2{off2 + (h*inter_sz), 1, off2 + (h + 1)*inter_sz - 1};
      Set jth_domain2 = SBG::LIB::SET_FACT.createSet(i2.begin(), i2.step()
        , i2.end());
      domain2 = std::move(domain2).disjointCup(std::move(jth_domain2));
    }
    domain1 = domain1.cartesianProduct(second_dim);
    domain2 = domain2.cartesianProduct(second_dim);

    Expression multidim_id{2, 1, 0};

    pw1.emplaceBack(domain1, multidim_id);
    pw2.emplaceBack(domain2, multidim_id);
  }

  return {pw1, pw2};
}

///*
// * @brief Test suite created to analyze the time growth of the different domain
// * ordered PWs operations.
// */
std::pair<PWMap, PWMap> contiguousMaps(SBG::LIB::NAT map_sz)
{
  SBG::LIB::NAT inter_sz = 100;
  SBG::LIB::NAT set_sz = 10;

  Set second_dim = SBG::LIB::SET_FACT.createSet(0, 1, inter_sz - 1);
  PWMap pw1 = SBG::LIB::PWMAP_FACT.createPWMap();
  PWMap pw2 = SBG::LIB::PWMAP_FACT.createPWMap();
  for (unsigned int j = 0; j < map_sz; ++j) {
    Set domain1 = SBG::LIB::SET_FACT.createSet();
    Set domain2 = SBG::LIB::SET_FACT.createSet();
    SBG::LIB::NAT off = j*set_sz*inter_sz;
    for (unsigned int h = 0; h < set_sz; ++h) {
      Interval i1{off + (h*inter_sz), 1, off + (h + 1)*inter_sz - 1};
      Set jth_domain1 = SBG::LIB::SET_FACT.createSet(i1.begin(), i1.step()
        , i1.end());
      domain1 = std::move(domain1).disjointCup(std::move(jth_domain1));

      SBG::LIB::NAT off2 = off + inter_sz/2;
      Interval i2{off2 + (h*inter_sz), 1, off2 + (h + 1)*inter_sz - 1};
      Set jth_domain2 = SBG::LIB::SET_FACT.createSet(i2.begin(), i2.step()
        , i2.end());
      domain2 = std::move(domain2).disjointCup(std::move(jth_domain2));
    }
    domain1 = domain1.cartesianProduct(second_dim);
    domain2 = domain2.cartesianProduct(second_dim);

    Expression multidim_id{2, 1, 0};

    pw1.emplaceBack(domain1, multidim_id);
    pw2.emplaceBack(domain2, multidim_id);
  }

  return {pw1, pw2};
}

////////////////////////////////////////////////////////////////////////////////
// Benchmarks ------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

static void BM_UnordPWMapDenseDom(benchmark::State& state)
{
  int map_sz = state.range(0);
  SBG::LIB::SET_FACT.set_set_fact(SetKind::kUnordered);
  SBG::LIB::PWMAP_FACT.set_pwmap_fact(PWMapKind::kUnordered);
  auto pw = denseDom(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw.domain());
  }
  state.SetComplexityN(map_sz);
}
BENCHMARK(BM_UnordPWMapDenseDom)->RangeMultiplier(10)->Range(10, 1e3)
  ->Complexity();

static void BM_UnordPWMapEq(benchmark::State& state)
{
  int map_sz = state.range(0);
  SBG::LIB::SET_FACT.set_set_fact(SetKind::kUnordered);
  SBG::LIB::PWMAP_FACT.set_pwmap_fact(PWMapKind::kUnordered);
  auto [pw1, pw2] = contiguousMaps(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1 == pw2);
  }
  state.SetComplexityN(map_sz);
}
BENCHMARK(BM_UnordPWMapEq)->RangeMultiplier(10)->Range(10, 1e2)->Complexity();

static void BM_UnordPWMapSum(benchmark::State& state)
{
  int map_sz = state.range(0);
  SBG::LIB::SET_FACT.set_set_fact(SetKind::kUnordered);
  SBG::LIB::PWMAP_FACT.set_pwmap_fact(PWMapKind::kUnordered);
  auto [pw1, pw2] = contiguousMaps(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1 + pw2);
  }
  state.SetComplexityN(map_sz);
}
BENCHMARK(BM_UnordPWMapSum)->RangeMultiplier(10)->Range(10, 1e3)->Complexity();

static void BM_UnordPWMapDom(benchmark::State& state)
{
  int map_sz = state.range(0);
  SBG::LIB::SET_FACT.set_set_fact(SetKind::kUnordered);
  SBG::LIB::PWMAP_FACT.set_pwmap_fact(PWMapKind::kUnordered);
  auto [pw1, pw2] = contiguousMaps(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.domain());
  }
  state.SetComplexityN(map_sz);
}
BENCHMARK(BM_UnordPWMapDom)->RangeMultiplier(10)->Range(10, 1e3)->Complexity();

static void BM_UnordPWMapRestrict(benchmark::State& state)
{
  int map_sz = state.range(0);
  SBG::LIB::SET_FACT.set_set_fact(SetKind::kUnordered);
  SBG::LIB::PWMAP_FACT.set_pwmap_fact(PWMapKind::kUnordered);
  auto [pw1, pw2] = contiguousMaps(map_sz);
  Set domain2 = pw2.domain();

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.restrict(domain2));
  }
  state.SetComplexityN(map_sz);
}
BENCHMARK(BM_UnordPWMapRestrict)->RangeMultiplier(10)->Range(10, 1e3)
  ->Complexity();

static void BM_UnordPWMapImage(benchmark::State& state)
{
  int map_sz = state.range(0);
  SBG::LIB::SET_FACT.set_set_fact(SetKind::kUnordered);
  SBG::LIB::PWMAP_FACT.set_pwmap_fact(PWMapKind::kUnordered);
  auto [pw1, pw2] = contiguousMaps(map_sz);
  Set domain2 = pw2.domain();

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.image(domain2));
  }
  state.SetComplexityN(map_sz);
}
BENCHMARK(BM_UnordPWMapImage)->RangeMultiplier(10)->Range(10, 1e3)->Complexity();

static void BM_UnordPWMapPreImage(benchmark::State& state)
{
  int map_sz = state.range(0);
  SBG::LIB::SET_FACT.set_set_fact(SetKind::kUnordered);
  SBG::LIB::PWMAP_FACT.set_pwmap_fact(PWMapKind::kUnordered);
  auto [pw1, pw2] = contiguousMaps(map_sz);
  Set img2 = pw2.image();

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.preImage(img2));
  }
  state.SetComplexityN(map_sz);
}
BENCHMARK(BM_UnordPWMapPreImage)->RangeMultiplier(10)->Range(10, 1e3)
  ->Complexity();

static void BM_UnordPWMapComposition(benchmark::State& state)
{
  int map_sz = state.range(0);
  SBG::LIB::SET_FACT.set_set_fact(SetKind::kUnordered);
  SBG::LIB::PWMAP_FACT.set_pwmap_fact(PWMapKind::kUnordered);
  auto [pw1, pw2] = contiguousMaps(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.composition(pw2));
  }
  state.SetComplexityN(map_sz);
}
BENCHMARK(BM_UnordPWMapComposition)->RangeMultiplier(10)->Range(10, 1e3)
  ->Complexity();

static void BM_UnordPWMapConcat(benchmark::State& state)
{
  int map_sz = state.range(0);
  SBG::LIB::SET_FACT.set_set_fact(SetKind::kUnordered);
  SBG::LIB::PWMAP_FACT.set_pwmap_fact(PWMapKind::kUnordered);
  auto [pw1, pw2] = interlacedMaps(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.concatenation(pw2));
  }
  state.SetComplexityN(map_sz);
}
BENCHMARK(BM_UnordPWMapConcat)->RangeMultiplier(10)->Range(10, 1e3)
  ->Complexity();

static void BM_UnordPWMapCombine(benchmark::State& state)
{
  int map_sz = state.range(0);
  SBG::LIB::SET_FACT.set_set_fact(SetKind::kUnordered);
  SBG::LIB::PWMAP_FACT.set_pwmap_fact(PWMapKind::kUnordered);
  auto [pw1, pw2] = contiguousMaps(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.combine(pw2));
  }
  state.SetComplexityN(map_sz);
}
BENCHMARK(BM_UnordPWMapCombine)->RangeMultiplier(10)->Range(10, 1e3)->Complexity();

// TODO: reduce

static void BM_UnordPWMapMinAdj(benchmark::State& state)
{
  int map_sz = state.range(0);
  SBG::LIB::SET_FACT.set_set_fact(SetKind::kUnordered);
  SBG::LIB::PWMAP_FACT.set_pwmap_fact(PWMapKind::kUnordered);
  auto [pw1, pw2] = minAdjMaps(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.minAdj(pw2));
  }
  state.SetComplexityN(map_sz);
}
BENCHMARK(BM_UnordPWMapMinAdj)->RangeMultiplier(10)->Range(10, 1e3)->Complexity();

static void BM_UnordPWMapEqImage(benchmark::State& state)
{
  int map_sz = state.range(0);
  SBG::LIB::SET_FACT.set_set_fact(SetKind::kUnordered);
  SBG::LIB::PWMAP_FACT.set_pwmap_fact(PWMapKind::kUnordered);
  auto [pw1, pw2] = contiguousMaps(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.equalImage(pw2));
  }
  state.SetComplexityN(map_sz);
}
BENCHMARK(BM_UnordPWMapEqImage)->RangeMultiplier(10)->Range(10, 1e3)
  ->Complexity();

static void BM_UnordPWMapLessImage(benchmark::State& state)
{
  int map_sz = state.range(0);
  SBG::LIB::SET_FACT.set_set_fact(SetKind::kUnordered);
  SBG::LIB::PWMAP_FACT.set_pwmap_fact(PWMapKind::kUnordered);
  auto [pw1, pw2] = contiguousMaps(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.lessImage(pw2));
  }
  state.SetComplexityN(map_sz);
}
BENCHMARK(BM_UnordPWMapLessImage)->RangeMultiplier(10)->Range(10, 1e3)
  ->Complexity();

static void BM_UnordPWMapCompact(benchmark::State& state)
{
  int map_sz = state.range(0);
  SBG::LIB::SET_FACT.set_set_fact(SetKind::kUnordered);
  SBG::LIB::PWMAP_FACT.set_pwmap_fact(PWMapKind::kUnordered);
  auto [pw1, pw2] = contiguousMaps(map_sz);

  for (auto _ : state) {
    pw1.compact();
  }
  state.SetComplexityN(map_sz);
}
BENCHMARK(BM_UnordPWMapCompact)->RangeMultiplier(10)->Range(10, 1e2)
  ->Complexity();

} // namespace Internal

} // namespace Test
