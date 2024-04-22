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

#ifndef PERF_ORD_HPP
#define PERF_ORD_HPP

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/subgraph.hpp>

#include <util/defs.hpp>

namespace OG {

// Set-vertex ------------------------------------------------------------------

struct Vertex {
  member_class(SBG::Util::MD_NAT, id);            // Unique identifier

  Vertex();
  Vertex(SBG::Util::MD_NAT id);

  bool operator==(const Vertex &v) const;
};

std::ostream &operator<<(std::ostream &out, const Vertex &v);

// Ordinary Graph Edge ---------------------------------------------------------

struct Edge {
  member_class(SBG::Util::MD_NAT, id);            // Unique identifier

  Edge();
  Edge(SBG::Util::MD_NAT id);

  bool operator==(const Edge &e) const;
};

std::ostream &operator<<(std::ostream &out, const Edge &e);

// Ordinary Graph definition ---------------------------------------------------

typedef boost::adjacency_list<
  boost::vecS, boost::vecS, boost::undirectedS, Vertex, Edge
> Graph;
typedef Graph::vertex_descriptor VertexDesc;
typedef boost::graph_traits<Graph>::vertex_iterator VertexIt;
typedef Graph::edge_descriptor EdgeDesc;
typedef boost::graph_traits<Graph>::edge_iterator EdgeIt;
typedef boost::graph_traits<Graph>::out_edge_iterator OutEdgeIt;

// Ordinary directed Graph definition ------------------------------------------

typedef boost::adjacency_list<
  boost::vecS, boost::vecS, boost::bidirectionalS, Vertex, Edge
> DGraph;
typedef DGraph::vertex_descriptor DVertexDesc;
typedef boost::graph_traits<DGraph>::vertex_iterator DVertexIt;
typedef DGraph::edge_descriptor DEdgeDesc;
typedef boost::graph_traits<DGraph>::edge_iterator DEdgeIt;
typedef boost::graph_traits<DGraph>::out_edge_iterator OutDEdgeIt;

}  // namespace OG

#endif
