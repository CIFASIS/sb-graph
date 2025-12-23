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

using SBG::LIB::Interval;
using SBG::LIB::SetPiece;
using SBG::LIB::Set;

std::pair<Set, Set> contiguousPieces(int set_sz) {
  SBG::LIB::NAT inter_sz = 100;

  Interval second_dim(0, 1, inter_sz - 1);
  Set s1 = SBG::LIB::SET_FACT.createSet();
  Set s2 = SBG::LIB::SET_FACT.createSet();
  for (unsigned int h = 0; h < set_sz; ++h) {
    Interval i1(h*inter_sz, 1, (h + 1)*inter_sz - 1);
    SetPiece mdi1;
    mdi1.emplaceBack(i1);
    mdi1.emplaceBack(second_dim);
    s1.emplaceBack(mdi1); 

    SBG::LIB::NAT off = inter_sz/2;
    Interval i2(off + (h*inter_sz), 1, off + (h + 1)*inter_sz - 1);
    SetPiece mdi2;
    mdi2.emplaceBack(i2);
    mdi2.emplaceBack(second_dim);
    s2.emplaceBack(mdi2); 
  }

  return {s1, s2};
}

static void BM_OrdSetDiff(benchmark::State& state) {
  int set_sz = state.range(0);
  SBG::Eval::setSetFactory(1);
  auto [s1, s2] = contiguousPieces(set_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(s1.difference(s2));
  }
  state.SetComplexityN(set_sz);
}
BENCHMARK(BM_OrdSetDiff)->RangeMultiplier(10)->Range(1000, 1e6)->Complexity();

static void BM_OrdSetDisjointUnion(benchmark::State& state) {
  int set_sz = state.range(0);
  SBG::Eval::setSetFactory(1);
  Set s1 = SBG::LIB::SET_FACT.createSet();
  Set s2 = SBG::LIB::SET_FACT.createSet();
  for (SBG::LIB::NAT j = 0; j < set_sz; j += 2) {
    Interval i(j*100 + 1, 1, (j + 1)*100);
    s1.emplaceBack(i);
    Interval i2((j + 1)*100+1, 1, (j + 2)*100);
    s2.emplaceBack(i2);
  }

  for (auto _ : state) {
    benchmark::DoNotOptimize(s1.disjointCup(s2));
  }
  state.SetComplexityN(set_sz);
}
BENCHMARK(BM_OrdSetDisjointUnion)->RangeMultiplier(10)->Range(1000, 1e6)
  ->Complexity();

static void BM_OrdSetEq(benchmark::State& state) {
  int set_sz = state.range(0);
  SBG::Eval::setSetFactory(1);
  auto [s1, s2] = contiguousPieces(set_sz);

  for (auto _ : state) {
    benchmark::DoNotOptimize(s1 == s2);
  }
  state.SetComplexityN(set_sz);
}
BENCHMARK(BM_OrdSetEq)->RangeMultiplier(10)->Range(1000, 1e6)->Complexity();

} // namespace Internal

} // namespace Test
