/*****************************************************************************

 This file is part of Set--Based Graph Library.

 SBG Library is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, edge_itther version 3 of the License, or
 (at your option) any later version.

 SBG Library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have recedge_itved a copy of the GNU General Public License
 along with SBG Library.  If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#include "test/performance/boost/scc_graph_builder.hpp"

namespace OG {

////////////////////////////////////////////////////////////////////////////////
// Auxiliary structures --------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

std::size_t EdgeHash::operator()(const Edge& e) const
{
  auto h1 = std::hash<unsigned long long>{}(e.first);
  auto h2 = std::hash<unsigned long long>{}(e.second);
  return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
}

////////////////////////////////////////////////////////////////////////////////
// Builder for the algebraic loop detection stage ------------------------------
////////////////////////////////////////////////////////////////////////////////

SCCGraphBuilder::SCCGraphBuilder(BipartiteGraph&& bgraph
  , VertexVector&& matching)
  : _bgraph(bgraph), _matching(matching) {}

DirectedGraph SCCGraphBuilder::build()
{
  SBG::LIB::NAT number_vertices = translateVertices();
  EdgeVector edges = getEdgeList();
  return build(number_vertices, edges);
}

DirectedGraph SCCGraphBuilder::build(SBG::LIB::NAT number_vertices
  , EdgeVector& E)
{
  return DirectedGraph(E.begin(), E.end(), number_vertices);
}

SBG::LIB::NAT SCCGraphBuilder::translateVertices()
{
  _vertex_map.reserve(boost::num_edges(_bgraph.graph()));

  auto null_v = boost::graph_traits<Graph>::null_vertex();
  unsigned int number_matched_edges = 0;
  for (int j = 0; j < _matching.size(); ++j) {
    if (_matching[j] != null_v && j < _matching[j]) {
      _vertex_map[Edge(j, _matching[j])] = number_matched_edges;
      ++number_matched_edges;
    }
  }

  return number_matched_edges;
}

EdgeVector SCCGraphBuilder::getEdgeList()
{
  EdgeVector result;

  const Graph& graph = _bgraph.graph();
  result.reserve(boost::num_edges(graph));
  const std::vector<int>& partition = _bgraph.partition();
  boost::graph_traits<Graph>::edge_iterator edge_it;
  boost::graph_traits<Graph>::edge_iterator edges_end;
  boost::tie(edge_it, edges_end) = boost::edges(graph);
  // Traverse original bipartite graph
  for (; edge_it != edges_end; ++edge_it) {
    Vertex u = source(*edge_it, graph);
    Vertex v = target(*edge_it, graph);
    Vertex match_u = _matching[u];
    // Detect unmatched edge
    if (match_u != v) {
      Vertex match_v = _matching[v];
      Vertex newv = v < match_v ? _vertex_map[Edge(v, match_v)]
        : _vertex_map[Edge(match_v, v)];
      Vertex newu = u < match_u ? _vertex_map[Edge(u, match_u)]
        : _vertex_map[Edge(match_u, u)];
      Edge e_result(newv, newu);
      if (!partition[u] && partition[v]) {
        result.emplace_back(Edge(newu, newv));
      } else {
        result.emplace_back(Edge(newv, newu));
      }
    }
  }

  return result;
}

}  // namespace OG
