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
#include <boost/graph/max_cardinality_matching.hpp>
#include <boost/graph/strong_components.hpp>

#include "eval/user_impl_map.hpp"
#include "test/performance/utils.hpp"
#include "test/performance/boost/ordinary_graph_builder.hpp"
#include "test/performance/utils.hpp"

namespace Test {

namespace Internal {

////////////////////////////////////////////////////////////////////////////////
// Benchmarks ------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

static void BM_CustomBoostMatchTest(benchmark::State& state)
{
  int N = state.range(0);

  const char* filename = std::getenv("TEST_FILE");
  if (filename) {
    SBG::LIB::SBG match_sbg = generateSBG(filename, N, 1);
    OG::OrdinaryGraphBuilder graph_builder(match_sbg);
    graph_builder.build();
    OG::Graph graph = graph_builder.graph();
    std::vector<OG::VertexDesc> mate(num_vertices(graph));

    for (auto _ : state) {
      edmonds_maximum_cardinality_matching(graph, &mate[0]);
    }
    state.SetComplexityN(N);
  }
}
BENCHMARK(BM_CustomBoostMatchTest)->RangeMultiplier(10)->Range(100, 1e5)
  ->Complexity()->Unit(benchmark::kMillisecond);

static void BM_CustomBoostSCCTest(benchmark::State& state)
{
  int N = state.range(0);

  const char* filename = std::getenv("TEST_FILE");
  if (filename) {
    SBG::LIB::SBG match_sbg = generateSBG(filename, N, 1);
    SBG::LIB::Matching match_algorithm
      = SBG::LIB::MATCH_FACT.createMatchAlgorithm();
    SBG::LIB::MatchData match_result = match_algorithm.calculate(match_sbg);
    SBG::LIB::DSBG scc_dsbg = MISC::buildSCCFromMatching(match_result);

    OG::OrdinaryDGraphBuilder graph_builder(scc_dsbg);
    graph_builder.build();
    OG::DGraph dgraph = graph_builder.dgraph();
    std::vector<int> components(num_vertices(dgraph));

    for (auto _ : state) {
      strong_components(dgraph, &components[0]);
    }
    state.SetComplexityN(N);
  }
}
BENCHMARK(BM_CustomBoostSCCTest)->RangeMultiplier(10)->Range(100, 1e6)
  ->Complexity()->Unit(benchmark::kMillisecond);

static void BM_CustomBoostSCCWithBuilderTest(benchmark::State& state)
{
  int N = state.range(0);

  const char* filename = std::getenv("TEST_FILE");
  if (filename) {
    SBG::LIB::SBG match_sbg = generateSBG(filename, N, 1);
    SBG::LIB::Matching match_algorithm
      = SBG::LIB::MATCH_FACT.createMatchAlgorithm();
    SBG::LIB::MatchData match_result = match_algorithm.calculate(match_sbg);
    SBG::LIB::DSBG scc_dsbg = MISC::buildSCCFromMatching(match_result);

    OG::OrdinaryDGraphBuilder graph_builder(scc_dsbg);
    OG::VertexVector vertices = graph_builder.getVertexList();
    OG::EdgeInfoVector edges = graph_builder.getEdgeList();
    for (auto _ : state) {
      graph_builder.build(vertices, edges);
      state.PauseTiming();
      OG::DGraph dgraph = graph_builder.dgraph();
      std::vector<int> components(num_vertices(dgraph));
      state.ResumeTiming();
      strong_components(dgraph, &components[0]);
      state.PauseTiming();
      graph_builder.clear();
      state.ResumeTiming();
    }
    state.SetComplexityN(N);
  }
}
BENCHMARK(BM_CustomBoostSCCWithBuilderTest)->RangeMultiplier(10)
  ->Range(100, 1e6)->Complexity()->Unit(benchmark::kMillisecond);

} // namespace Internal

} // namespace Test
