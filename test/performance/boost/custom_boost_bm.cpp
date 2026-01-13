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

/**
 * @file custom_boost_bm.cpp
 * @brief Executes the scalar causalization workflow for the desired file.
 * @note The input .test file should define a variable N that describes the
 * "size" of the repetitive patterns of the graph. 
 */

#include <unordered_map>

#include <benchmark/benchmark.h>
#include <boost/graph/max_cardinality_matching.hpp>
#include <boost/graph/strong_components.hpp>

#include "eval/user_impl_map.hpp"
#include "test/performance/boost/ordinary_graph_builder.hpp"
#include "test/performance/boost/scc_graph_builder.hpp"
#include "test/performance/utils.hpp"
#include "util/defs.hpp"
#include "util/time_profiler.hpp"

namespace Test {

namespace Internal {

int lower = 100;
int mult = 10;
int upper = 1e6;
std::unordered_map<int, OG::BipartiteGraph> input_graphs;

/**
 * @brief Creates single instances of the input bipartite graph of size N,
 * varying N accordingly.
 */
void initialize(const char *filename)
{
  input_graphs.reserve(upper/lower);
  for (int N = lower; N <= upper; N *= mult) {
    SBG::LIB::BipartiteSBG match_sbg = generateSBG(filename, N, 1);
    OG::OrdinaryGraphBuilder graph_builder(match_sbg);
    OG::BipartiteGraph bgraph = graph_builder.build();
    input_graphs[N] = bgraph;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Benchmarks ------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

static void BM_CustomBoostMatch(benchmark::State& state)
{
  int N = state.range(0);

  OG::BipartiteGraph& bgraph = input_graphs[N];
  const OG::Graph& match_graph = bgraph.graph();
  int number_vertices = num_vertices(match_graph);

  for (auto _ : state) {
    OG::VertexVector matching(number_vertices);
    edmonds_maximum_cardinality_matching(match_graph, &matching[0]);
  }
  state.SetComplexityN(N);
}
BENCHMARK(BM_CustomBoostMatch)->RangeMultiplier(mult)
  ->Range(lower, upper)->Complexity()->Unit(benchmark::kMillisecond);

static void BM_CustomBoostSCC(benchmark::State& state)
{
  int N = state.range(0);

  OG::BipartiteGraph& bgraph = input_graphs[N];
  const OG::Graph& match_graph = bgraph.graph();
  OG::VertexVector matching(num_vertices(match_graph));
  edmonds_maximum_cardinality_matching(match_graph, &matching[0]);
  OG::SCCGraphBuilder scc_builder(std::move(bgraph), std::move(matching));
  OG::DirectedGraph scc_graph = scc_builder.build();

  for (auto _ : state) {
    std::vector<int> components(num_vertices(scc_graph));
    strong_components(scc_graph, &components[0]);
  }
  state.SetComplexityN(N);
}
BENCHMARK(BM_CustomBoostSCC)->RangeMultiplier(mult)
  ->Range(lower, upper)->Complexity()->Unit(benchmark::kMillisecond);

static void BM_CustomBoostSCCWithBuilder(benchmark::State& state)
{
  int N = state.range(0);

  OG::BipartiteGraph& bgraph = input_graphs[N];
  const OG::Graph& match_graph = bgraph.graph();
  OG::VertexVector matching(num_vertices(match_graph));
  edmonds_maximum_cardinality_matching(match_graph, &matching[0]);
  OG::SCCGraphBuilder scc_builder(std::move(bgraph), std::move(matching));

  for (auto _ : state) {
    OG::DirectedGraph scc_graph = scc_builder.build();
    std::vector<int> components(num_vertices(scc_graph));
    strong_components(scc_graph, &components[0]);
  }
  state.SetComplexityN(N);
}
BENCHMARK(BM_CustomBoostSCCWithBuilder)->RangeMultiplier(mult)
  ->Range(lower, upper)->Complexity()->Unit(benchmark::kMillisecond);

} // namespace Internal

} // namespace Test

////////////////////////////////////////////////////////////////////////////////
// Main ------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
  const char *filename = std::getenv("TEST_FILE");
  if (filename) {
    Test::Internal::initialize(filename);

    ::benchmark::Initialize(&argc, argv);
    ::benchmark::RunSpecifiedBenchmarks();
  }

  return 0;
}
