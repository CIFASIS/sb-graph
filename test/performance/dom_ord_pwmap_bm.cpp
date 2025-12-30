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

#include <benchmark/benchmark.h>

#include "eval/user_impl_map.hpp"

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

PWMap denseDom(SBG::LIB::NAT map_sz)
{
  SBG::LIB::NAT inter_sz = 100;
  SBG::LIB::NAT set_sz = 10;

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int j = 0; j < map_sz; ++j) {
    Set dom1 = SBG::LIB::SET_FACT.createSet();
    SBG::LIB::NAT off = j*set_sz*inter_sz;
    for (unsigned int h = 0; h < set_sz; ++h) {
      Interval i1(off + (h*inter_sz), 1, off + (h + 1)*inter_sz - 1);
      SetPiece mdi1;
      mdi1.emplaceBack(i1);
      dom1.emplaceBack(mdi1); 
    }

    LExp id;
    Exp multidim_id(1, id);

    pw.emplaceBack(Map(dom1, multidim_id));
  }

  return pw;
}

PWMap mapInfPW(SBG::LIB::NAT map_sz)
{
  SBG::LIB::NAT inter_sz = 100;
  SBG::LIB::NAT set_sz = 10;

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  unsigned int j = 0;
  for (; j < map_sz; ++j) {
    Set dom1 = SBG::LIB::SET_FACT.createSet();
    SBG::LIB::NAT off = j*set_sz*inter_sz;
    for (unsigned int h = 0; h < set_sz; ++h) {
      Interval i1(off + (h*inter_sz), 1, off + (h + 1)*inter_sz - 1);
      SetPiece mdi1;
      mdi1.emplaceBack(i1);
      dom1.emplaceBack(mdi1); 
    }

    Exp plus_one(1, LExp(1, 1));

    pw.emplaceBack(Map(dom1, plus_one));
  }

  Set dom_id = SBG::LIB::SET_FACT.createSet();
  SBG::LIB::NAT off = j*set_sz*inter_sz;
  for (unsigned int h = 0; h < set_sz; ++h) {
    Interval i1(off + (h*inter_sz), 1, off + (h + 1)*inter_sz - 1);
    SetPiece mdi1;
    mdi1.emplaceBack(i1);
    dom_id.emplaceBack(mdi1); 
  }

  Exp multidim_id(1, LExp());

  pw.emplaceBack(Map(dom_id, multidim_id));

  return pw;
}

std::pair<PWMap, PWMap> minAdjMaps(SBG::LIB::NAT map_sz)
{
  SBG::LIB::NAT inter_sz = 100;
  SBG::LIB::NAT set_sz = 10;

  Interval second_dim(1, 1, inter_sz);
  PWMap pw1 = SBG::LIB::PW_FACT.createPWMap();
  PWMap pw2 = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int j = 0; j < map_sz; ++j) {
    Set dom1 = SBG::LIB::SET_FACT.createSet();
    Set dom2 = SBG::LIB::SET_FACT.createSet();
    SBG::LIB::NAT off = j*set_sz*inter_sz;
    for (unsigned int h = 0; h < set_sz; ++h) {
      Interval i1(off + (h*inter_sz), 1, off + (h + 1)*inter_sz - 1);
      SetPiece mdi1;
      mdi1.emplaceBack(i1);
      mdi1.emplaceBack(second_dim);
      dom1.emplaceBack(mdi1); 

      SBG::LIB::NAT off2 = off + inter_sz/2;
      Interval i2(off2 + (h*inter_sz), 1, off2 + (h + 1)*inter_sz - 1);
      SetPiece mdi2;
      mdi2.emplaceBack(i2);
      mdi2.emplaceBack(second_dim);
      dom2.emplaceBack(mdi2); 
    }

    Exp multidim_id(2, LExp());
    Exp minus_one(2, LExp(1, RATIONAL(-1, 1)));

    pw1.emplaceBack(Map(dom1, multidim_id));
    pw2.emplaceBack(Map(dom2, minus_one));
  }

  return {pw1, pw2};
}

std::pair<PWMap, PWMap> interlacedMaps(SBG::LIB::NAT map_sz)
{
  SBG::LIB::NAT inter_sz = 100;
  SBG::LIB::NAT set_sz = 10;

  Interval second_dim(0, 1, inter_sz - 1);
  PWMap pw1 = SBG::LIB::PW_FACT.createPWMap();
  PWMap pw2 = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int j = 0; j < 2*map_sz; j += 2) {
    Set dom1 = SBG::LIB::SET_FACT.createSet();
    Set dom2 = SBG::LIB::SET_FACT.createSet();
    SBG::LIB::NAT off = j*set_sz*inter_sz;
    for (unsigned int h = 0; h < set_sz; ++h) {
      Interval i1(off + (h*inter_sz), 1, off + (h + 1)*inter_sz - 1);
      SetPiece mdi1;
      mdi1.emplaceBack(i1);
      mdi1.emplaceBack(second_dim);
      dom1.emplaceBack(mdi1); 

      SBG::LIB::NAT off2 = off + inter_sz;
      Interval i2(off2 + (h*inter_sz), 1, off2 + (h + 1)*inter_sz - 1);
      SetPiece mdi2;
      mdi2.emplaceBack(i2);
      mdi2.emplaceBack(second_dim);
      dom2.emplaceBack(mdi2); 
    }

    LExp id;
    Exp multidim_id(2, id);

    pw1.emplaceBack(Map(dom1, multidim_id));
    pw2.emplaceBack(Map(dom2, multidim_id));
  }

  return {pw1, pw2};
}

/*
 * @brief Test suite created to analyze the time growth of the different domain
 * ordered PWs operations.
 */
std::pair<PWMap, PWMap> contiguousMaps(SBG::LIB::NAT map_sz)
{
  SBG::LIB::NAT inter_sz = 100;
  SBG::LIB::NAT set_sz = 10;

  Interval second_dim(0, 1, inter_sz - 1);
  PWMap pw1 = SBG::LIB::PW_FACT.createPWMap();
  PWMap pw2 = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int j = 0; j < map_sz; ++j) {
    Set dom1 = SBG::LIB::SET_FACT.createSet();
    Set dom2 = SBG::LIB::SET_FACT.createSet();
    SBG::LIB::NAT off = j*set_sz*inter_sz;
    for (unsigned int h = 0; h < set_sz; ++h) {
      Interval i1(off + (h*inter_sz), 1, off + (h + 1)*inter_sz - 1);
      SetPiece mdi1;
      mdi1.emplaceBack(i1);
      mdi1.emplaceBack(second_dim);
      dom1.emplaceBack(mdi1); 

      SBG::LIB::NAT off2 = off + inter_sz/2;
      Interval i2(off2 + (h*inter_sz), 1, off2 + (h + 1)*inter_sz - 1);
      SetPiece mdi2;
      mdi2.emplaceBack(i2);
      mdi2.emplaceBack(second_dim);
      dom2.emplaceBack(mdi2); 
    }

    LExp id;
    Exp multidim_id(2, id);

    pw1.emplaceBack(Map(dom1, multidim_id));
    pw2.emplaceBack(Map(dom2, multidim_id));
  }

  return {pw1, pw2};
}

static void BM_DomOrdPWDenseDom(benchmark::State& state)
{
  int map_sz = state.range(0);
  SBG::Eval::setSetFactory(2);
  SBG::Eval::setPWFactory(2);
  auto pw = denseDom(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw.dom());
  }
  state.SetComplexityN(map_sz);
}
BENCHMARK(BM_DomOrdPWDenseDom)->RangeMultiplier(10)->Range(100, 1e5)->Complexity();

static void BM_DomOrdPWEq(benchmark::State& state)
{
  int map_sz = state.range(0);
  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(2);
  auto [pw1, pw2] = contiguousMaps(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1 == pw2);
  }
  state.SetComplexityN(map_sz);
}
BENCHMARK(BM_DomOrdPWEq)->RangeMultiplier(10)->Range(10, 1e4)->Complexity();

static void BM_DomOrdPWSum(benchmark::State& state)
{
  int map_sz = state.range(0);
  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(2);
  auto [pw1, pw2] = contiguousMaps(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1 + pw2);
  }
  state.SetComplexityN(map_sz);
}
BENCHMARK(BM_DomOrdPWSum)->RangeMultiplier(10)->Range(10, 1e5)->Complexity();

static void BM_DomOrdPWDom(benchmark::State& state)
{
  int map_sz = state.range(0);
  SBG::Eval::setSetFactory(2);
  SBG::Eval::setPWFactory(2);
  auto [pw1, pw2] = contiguousMaps(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.dom());
  }
  state.SetComplexityN(map_sz);
}
BENCHMARK(BM_DomOrdPWDom)->RangeMultiplier(10)->Range(100, 1e5)->Complexity();

static void BM_DomOrdPWRestrict(benchmark::State& state)
{
  int map_sz = state.range(0);
  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(2);
  auto [pw1, pw2] = contiguousMaps(map_sz);
  Set dom2 = pw2.dom();

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.restrict(dom2));
  }
  state.SetComplexityN(map_sz);
}
BENCHMARK(BM_DomOrdPWRestrict)->RangeMultiplier(10)->Range(10, 1e4)
  ->Complexity();

static void BM_DomOrdPWImage(benchmark::State& state)
{
  int map_sz = state.range(0);
  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(2);
  auto [pw1, pw2] = contiguousMaps(map_sz);
  Set dom2 = pw2.dom();

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.image(dom2));
  }
  state.SetComplexityN(map_sz);
}
BENCHMARK(BM_DomOrdPWImage)->RangeMultiplier(10)->Range(10, 1e3)->Complexity();

static void BM_DomOrdPWPreImage(benchmark::State& state)
{
  int map_sz = state.range(0);
  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(2);
  auto [pw1, pw2] = contiguousMaps(map_sz);
  Set img2 = pw2.image();

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.preImage(img2));
  }
  state.SetComplexityN(map_sz);
}
BENCHMARK(BM_DomOrdPWPreImage)->RangeMultiplier(10)->Range(10, 1e3)
  ->Complexity();

static void BM_DomOrdPWComposition(benchmark::State& state)
{
  int map_sz = state.range(0);
  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(2);
  auto [pw1, pw2] = contiguousMaps(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.composition(pw2));
  }
  state.SetComplexityN(map_sz);
}
BENCHMARK(BM_DomOrdPWComposition)->RangeMultiplier(10)->Range(10, 1e3)
  ->Complexity();

static void BM_DomOrdPWMapInf(benchmark::State& state)
{
  int map_sz = state.range(0);
  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(2);
  auto pw = mapInfPW(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw.mapInf());
  }
  state.SetComplexityN(map_sz);
}
BENCHMARK(BM_DomOrdPWMapInf)->RangeMultiplier(10)->Range(10, 1e2)
  ->Complexity();

static void BM_DomOrdPWConcat(benchmark::State& state)
{
  int map_sz = state.range(0);
  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(2);
  auto [pw1, pw2] = interlacedMaps(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.concatenation(pw2));
  }
  state.SetComplexityN(map_sz);
}
BENCHMARK(BM_DomOrdPWConcat)->RangeMultiplier(10)->Range(10, 1e5)->Complexity();

static void BM_DomOrdPWCombine(benchmark::State& state)
{
  int map_sz = state.range(0);
  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(2);
  auto [pw1, pw2] = contiguousMaps(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.combine(pw2));
  }
  state.SetComplexityN(map_sz);
}
BENCHMARK(BM_DomOrdPWCombine)->RangeMultiplier(10)->Range(10, 1e4)->Complexity();

// TODO: reduce

static void BM_DomOrdPWMinAdj(benchmark::State& state)
{
  int map_sz = state.range(0);
  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(2);
  auto [pw1, pw2] = minAdjMaps(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.minAdjMap(pw2));
  }
  state.SetComplexityN(map_sz);
}
BENCHMARK(BM_DomOrdPWMinAdj)->RangeMultiplier(10)->Range(10, 1e3)->Complexity();

static void BM_DomOrdPWFirstInv(benchmark::State& state)
{
  int map_sz = state.range(0);
  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(2);
  auto [pw1, pw2] = contiguousMaps(map_sz);
  Set dom2 = pw2.dom();

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.firstInv(dom2));
  }
  state.SetComplexityN(map_sz);
}
BENCHMARK(BM_DomOrdPWFirstInv)->RangeMultiplier(10)->Range(10, 1e3)
  ->Complexity();

static void BM_DomOrdPWEqImage(benchmark::State& state)
{
  int map_sz = state.range(0);
  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(2);
  auto [pw1, pw2] = contiguousMaps(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.equalImage(pw2));
  }
  state.SetComplexityN(map_sz);
}
BENCHMARK(BM_DomOrdPWEqImage)->RangeMultiplier(10)->Range(10, 1e4)
  ->Complexity();

static void BM_DomOrdPWLessImage(benchmark::State& state)
{
  int map_sz = state.range(0);
  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(2);
  auto [pw1, pw2] = contiguousMaps(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.lessImage(pw2));
  }
  state.SetComplexityN(map_sz);
}
BENCHMARK(BM_DomOrdPWLessImage)->RangeMultiplier(10)->Range(10, 1e4)
  ->Complexity();

static void BM_DomOrdPWCompact(benchmark::State& state)
{
  int map_sz = state.range(0);
  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(2);
  auto [pw1, pw2] = contiguousMaps(map_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(pw1.compact());
  }
  state.SetComplexityN(map_sz);
}
BENCHMARK(BM_DomOrdPWCompact)->RangeMultiplier(10)->Range(10, 1e3)
  ->Complexity();

} // namespace Internal

} // namespace Test
