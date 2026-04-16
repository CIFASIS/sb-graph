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

#include "test/performance/boost/boost_bm.hpp"
#include "sbg/bipartite_sbg.hpp"
#include "test/performance/boost/scalar_graph.hpp"
#include "test/performance/boost/scalar_graph_builder.hpp"
#include "test/performance/boost/scc_graph_builder.hpp"
#include "test/performance/utils.hpp"

#include <benchmark/benchmark.h>
#include <boost/graph/max_cardinality_matching.hpp>
#include <boost/graph/strong_components.hpp>

namespace SBG {

namespace perf {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Benchmarks ------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

static void BM_BoostMatching(benchmark::State& state, std::string filename)
{
  int N = state.range(0);

  LIB::BipartiteSBG match_sbg = generateSBG(filename, N, 1);
  ScalarGraphBuilder graph_builder(match_sbg);
  BipartiteGraph bgraph = graph_builder.build();
  const Graph& match_graph = bgraph.graph();
  int number_vertices = num_vertices(match_graph);
  for (auto _ : state) {
    VertexVector matching(number_vertices);
    edmonds_maximum_cardinality_matching(match_graph, &matching[0]);
    benchmark::DoNotOptimize(matching);
  }

  state.SetComplexityN(N);
}

static void BM_BoostSCC(benchmark::State& state
  , std::string filename)
{
  int N = state.range(0);

  LIB::BipartiteSBG match_sbg = generateSBG(filename, N, 1);
  ScalarGraphBuilder graph_builder(match_sbg);
  BipartiteGraph bgraph = graph_builder.build();
  const Graph& match_graph = bgraph.graph();
  VertexVector matching(num_vertices(match_graph));
  edmonds_maximum_cardinality_matching(match_graph, &matching[0]);

  SCCGraphBuilder scc_builder(std::move(bgraph), std::move(matching));
  DirectedGraph scc_graph = scc_builder.build();
  for (auto _ : state) {
    std::vector<int> components(num_vertices(scc_graph));
    benchmark::DoNotOptimize(strong_components(scc_graph, &components[0]));
  }
  state.SetComplexityN(N);
}

static void BM_BoostSCCWithBuilder(benchmark::State& state
  , std::string filename)
{
  int N = state.range(0);

  LIB::BipartiteSBG match_sbg = generateSBG(filename, N, 1);
  ScalarGraphBuilder graph_builder(match_sbg);
  BipartiteGraph bgraph = graph_builder.build();
  const Graph& match_graph = bgraph.graph();
  VertexVector matching(num_vertices(match_graph));
  edmonds_maximum_cardinality_matching(match_graph, &matching[0]);

  SCCGraphBuilder scc_builder(std::move(bgraph), std::move(matching));
  for (auto _ : state) {
    DirectedGraph scc_graph = scc_builder.build();
    std::vector<int> components(num_vertices(scc_graph));
    benchmark::DoNotOptimize(strong_components(scc_graph, &components[0]));
  }
  state.SetComplexityN(N);
}

////////////////////////////////////////////////////////////////////////////////
// Register Benchmarks ---------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

void registerBoostBenchmarks(std::string filename)
{
  benchmark::RegisterBenchmark(
    ("BM_BoostMatching/" + filename).c_str(),
    [filename](benchmark::State& state) {
      BM_BoostMatching(state, filename);
    }
  )->RangeMultiplier(10)->Range(100, 1e6)->Complexity()
    ->Unit(benchmark::kMillisecond);

  benchmark::RegisterBenchmark(
    ("BM_BoostSCC/" + filename).c_str(),
    [filename](benchmark::State& state) {
      BM_BoostSCC(state, filename);
    }
  )->RangeMultiplier(10)->Range(100, 1e6)->Complexity()
    ->Unit(benchmark::kMillisecond);

  benchmark::RegisterBenchmark(
    ("BM_BoostSCCWithBuilder/" + filename).c_str(),
    [filename](benchmark::State& state) {
      BM_BoostSCCWithBuilder(state, filename);
    }
  )->RangeMultiplier(10)->Range(100, 1e6)->Complexity()
    ->Unit(benchmark::kMillisecond);
}

} // namespace detail

} // namespace perf

} // namespace SBG
