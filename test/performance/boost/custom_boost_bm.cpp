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
    OG::Graph graph = graph_builder.build();
    VertexVector mate(num_vertices(graph));

    for (auto _ : state) {
      edmonds_maximum_cardinality_matching(graph, &mate[0]);
    }
    state.SetComplexityN(N);
  }
}
BENCHMARK(BM_CustomBoostMatchTest)->RangeMultiplier(10)->Range(10, 10)
  ->Complexity()->Unit(benchmark::kMillisecond)->Iterations(1);

} // namespace Internal

} // namespace Test
