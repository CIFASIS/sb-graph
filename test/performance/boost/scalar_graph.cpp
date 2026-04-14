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

#include <test/performance/boost/scalar_graph.hpp>

namespace SBG {

namespace perf {

namespace detail {

// Graph -----------------------------------------------------------------------

std::ostream& operator<<(std::ostream& out, const Graph& g)
{
  out << "Vertices: {0, ..., " << boost::num_vertices(g) - 1 << "}\n";

  out << "Edges:\n";
  boost::graph_traits<Graph>::vertex_iterator vi;
  boost::graph_traits<Graph>::vertex_iterator vi_end;
  for (boost::tie(vi, vi_end) = boost::vertices(g); vi != vi_end; ++vi) {
    out << *vi << ": { ";
    boost::graph_traits<Graph>::adjacency_iterator ai;
    boost::graph_traits<Graph>::adjacency_iterator ai_end;
    boost::tie(ai, ai_end) = boost::adjacent_vertices(*vi, g);
    for (; ai != ai_end; ++ai) {
      out << *ai << " ";
    }
    out << "}" << std::endl;
  }

  return out;
}

// Bipartite Graph -------------------------------------------------------------

BipartiteGraph::BipartiteGraph() : _graph(), _partition() {}

BipartiteGraph::BipartiteGraph(Graph&& graph, std::vector<int>&& partition)
  : _graph(graph), _partition(partition) {}

const Graph& BipartiteGraph::graph() const
{
  return _graph;
}

const std::vector<int>& BipartiteGraph::partition() const
{
  return _partition;
}

std::ostream& operator<<(std::ostream& out, const BipartiteGraph& bgraph)
{
  out << bgraph.graph();
  out << "Partition:\n";
  std::vector<int> partition = bgraph.partition();
  for (int j = 0; j < partition.size(); ++j) {
    out << j << ": " << partition[j] << "\n";
  }

  return out;
}

// Directed Graph --------------------------------------------------------------

std::ostream& operator<<(std::ostream& out, const DirectedGraph& dgraph)
{
  out << "Vertices: {0, ..., " << boost::num_vertices(dgraph) - 1 << "}\n";

  out << "Edges:\n";
  boost::graph_traits<DirectedGraph>::edge_iterator edge_it;
  boost::graph_traits<DirectedGraph>::edge_iterator edges_end;
  boost::tie(edge_it, edges_end) = boost::edges(dgraph);
  for (; edge_it != edges_end; ++edge_it) {
    auto u = source(*edge_it, dgraph);
    auto v = target(*edge_it, dgraph);
    out << u << " -> " << v << "\n";
  }

  return out;
}

} // namespace detail

} // namespace perf

} // namespace SBG
