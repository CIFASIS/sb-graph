/** @file scc_graph_builder.hpp

 @brief <b>SCC Graph Builder</b>

 Module in charge of constructing the scalar graph used as input of the
 algebraic loops detection process. The generated result will be used as
 input of the Tarjan SCC algorithm.

 <hr>

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

#ifndef SBGRAPH_TEST_PERFORMANCE_BOOST_SCC_GRAPH_BUILDER_HPP_
#define SBGRAPH_TEST_PERFORMANCE_BOOST_SCC_GRAPH_BUILDER_HPP_

#include "sbg/natural.hpp"
#include "test/performance/boost/scalar_graph.hpp"

#include <unordered_map>

namespace SBG {

namespace perf {

namespace detail { 

////////////////////////////////////////////////////////////////////////////////
// Auxiliary structures --------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

class EdgeHash {
public:
  std::size_t operator()(const Edge& e) const;
};

////////////////////////////////////////////////////////////////////////////////
// Builder for the algebraic loop detection stage ------------------------------
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Builds the graph taken as input by the SCC algorithm. Given an input
 * bipartite graph G = (V, E) with partition X-Y and a maximum matching M of G
 * , the resulting graph G' = (V', E') satisfies:
 *   1- V' = M.
 *   2- E' = {({u1, v1}, {u2, v2}) : {u1, v1}, {u2, v2} ∈ M
 *   and {X ∩ {u1, v1}, Y ∩ {u2, v2}} ∈ (E\M)}.
 */
class SCCGraphBuilder {
public:
  SCCGraphBuilder(BipartiteGraph&& g, VertexVector&& matching);

  DirectedGraph build();
  DirectedGraph build(SBG::LIB::NAT number_vertices, EdgeVector& edges);
  SBG::LIB::NAT translateVertices();
  EdgeVector getEdgeList();

private:
  const BipartiteGraph _bgraph; ///< Input bipartite graph
  const VertexVector _matching;
  std::unordered_map<Edge, Vertex, EdgeHash> _vertex_map;
};

} // namespace detail

} // namespace perf

} // namespace SBG

#endif // SBGRAPH_TEST_PERFORMANCE_BOOST_SCC_GRAPH_BUILDER_HPP_
