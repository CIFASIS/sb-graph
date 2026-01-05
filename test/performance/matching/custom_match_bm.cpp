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
#include "test/performance/utils.hpp"

namespace Test {

namespace Internal {

////////////////////////////////////////////////////////////////////////////////
// Benchmarks ------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

static void BM_CustomMatchTest(benchmark::State& state)
{
  int N = state.range(0);

  const char* filename = std::getenv("TEST_FILE");
  if (filename) {
    const char* set_impl = std::getenv("SET_IMPL");
    const char* pw_impl = std::getenv("PW_IMPL");
    SBG::Eval::setSetFactory(set_impl ? std::stoi(set_impl) : 1);
    SBG::Eval::setPWFactory(pw_impl ? std::stoi(pw_impl) : 1);

    SBG::LIB::Matching match_algorithm
      = SBG::LIB::MATCH_FACT.createMatchAlgorithm();
    SBG::LIB::SBG match_sbg = generateSBG(filename, N, 1); 

    for (auto _ : state) {
      match_algorithm.calculate(match_sbg);
    }
    state.SetComplexityN(N);
  }
}
BENCHMARK(BM_CustomMatchTest)->RangeMultiplier(10)->Range(100, 1e6)->Complexity()
  ->Unit(benchmark::kMillisecond);

static void BM_CustomMatchCopies(benchmark::State& state)
{
  int N = state.range(0);

  const char* filename = std::getenv("TEST_FILE");
  if (filename) {
    const char* set_impl = std::getenv("SET_IMPL");
    const char* pw_impl = std::getenv("PW_IMPL");
    SBG::Eval::setSetFactory(set_impl ? std::stoi(set_impl) : 1);
    SBG::Eval::setPWFactory(pw_impl ? std::stoi(pw_impl) : 1);

    SBG::LIB::Matching match_algorithm
      = SBG::LIB::MATCH_FACT.createMatchAlgorithm();
    SBG::LIB::SBG match_sbg = generateSBG(filename, 100, N);

    for (auto _ : state) {
      match_algorithm.calculate(match_sbg);
    }
    state.SetComplexityN(N);
  }
}
BENCHMARK(BM_CustomMatchCopies)->RangeMultiplier(2)->Range(1, 128)
  ->Complexity()->Unit(benchmark::kMillisecond);

} // namespace Internal

} // namespace Test
