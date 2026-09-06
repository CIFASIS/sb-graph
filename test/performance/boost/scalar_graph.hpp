/** @file scalar_graph.hpp

 @brief <b>Scalar Graph</b>

 Module that defines the structure for scalar graphs.

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

#ifndef SBGRAPH_TEST_PERFORMANCE_BOOST_SCALAR_GRAPH_HPP_
#define SBGRAPH_TEST_PERFORMANCE_BOOST_SCALAR_GRAPH_HPP_

#include "sbg/integer.hpp"

#include <boost/graph/adjacency_list.hpp>

namespace SBG {

namespace perf {

namespace detail {

using Vertex = SBG::LIB::Int;
using VertexVector = std::vector<Vertex>;
using Edge = std::pair<SBG::LIB::Int, SBG::LIB::Int>;
using EdgeVector = std::vector<Edge>;

using Graph = boost::adjacency_list<boost::vecS, boost::vecS
  , boost::undirectedS>;
std::ostream& operator<<(std::ostream& out, const Graph& bgraph);

class BipartiteGraph {
  public:
  BipartiteGraph();
  BipartiteGraph(Graph&& graph, std::vector<int>&& partition);

  const Graph& graph() const;
  const std::vector<int>& partition() const;

  private:
  Graph _graph;

  /**
   * @brief Vector of the size of the set of vertices of _graph.
   * _partition[i] = 0 if vertex i belongs to X, and _partition[i] = 1 if
   * it belongs to Y. 
   */
  std::vector<int> _partition;
};
std::ostream& operator<<(std::ostream& out, const BipartiteGraph& bgraph);

using DirectedGraph = boost::adjacency_list<boost::vecS, boost::vecS
  , boost::bidirectionalS>;
std::ostream& operator<<(std::ostream& out, const DirectedGraph& dgraph);

} // namespace detail

} // namespace perf

} // namespace SBG

#endif // SBGRAPH_TEST_PERFORMANCE_BOOST_SCALAR_GRAPH_HPP_
