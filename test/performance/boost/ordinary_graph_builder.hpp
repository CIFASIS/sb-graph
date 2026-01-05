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

#ifndef PERF_ORD_BUILDER_HPP
#define PERF_ORD_BUILDER_HPP

#include <cassert>
#include <chrono>
#include <map>

#include "test/performance/boost/ordinary_graph.hpp"
#include "sbg/sbg.hpp"
#include "util/logger.hpp"

namespace OG {

using VertexVector = std::vector<Vertex>;

struct EdgeInfo {
  EdgeInfo(Edge e, Vertex v1, Vertex v2);

  Edge _e;
  Vertex _v1;
  Vertex _v2;
};
using EdgeInfoVector = std::vector<EdgeInfo>;

class OrdinaryGraphBuilder {
  public:
  OrdinaryGraphBuilder(SBG::LIB::SBG sbg);

  const Graph& graph() const;

  void build();
  void build(const VertexVector& V, const EdgeInfoVector& E);
  void clear();
  VertexVector getVertexList() const;
  EdgeInfoVector getEdgeList();

  private:
  void addVertex(const Vertex& v);
  void addEdge(const EdgeInfo& info);

  const SBG::LIB::SBG _sbg; ///< Input SBG to convert
  Graph _graph; ///< Resulting ordinary graph
  std::map<SBG::LIB::MD_NAT, VertexDesc> _vertex_map;
    ///< Auxiliar map for addEdge 
};

class OrdinaryDGraphBuilder {
  public:
  OrdinaryDGraphBuilder(SBG::LIB::DSBG dsbg);

  const DGraph& dgraph() const;

  void build();
  void build(const VertexVector& V, const EdgeInfoVector& E);
  void clear();
  VertexVector getVertexList() const;
  EdgeInfoVector getEdgeList();

  private:
  void addVertex(const Vertex& v);
  void addEdge(const EdgeInfo& info);

  const SBG::LIB::DSBG _dsbg; ///< Input SBG to convert
  DGraph _dgraph; ///< Resulting ordinary graph
  std::map<SBG::LIB::MD_NAT, VertexDesc> _vertex_map;
    ///< Auxiliar map for addEdge 
};

}  // namespace OG

#endif
