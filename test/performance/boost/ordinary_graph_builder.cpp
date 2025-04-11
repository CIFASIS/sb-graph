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

#include <test/performance/boost/ordinary_graph_builder.hpp>

namespace OG {

SBG::LIB::MD_NAT nextElem(SBG::LIB::MD_NAT curr, SBG::LIB::SetPiece mdi)
{
  assert(!mdi.isEmpty());
  SBG::LIB::MD_NAT min = mdi.minElem(), max = mdi.maxElem(), res;
  for (unsigned int j = 0; j < mdi.arity(); ++j) {
    if (curr[j] == max[j]) 
      res.emplaceBack(min[j]);

    else {
      res.emplaceBack(curr[j] + 1);
      for (unsigned int k = 1; k < mdi.arity() - j; ++k)
        res.emplaceBack(curr[j]);
      break;
    } 
  }

  return res;
}

OrdinaryGraphBuilder::OrdinaryGraphBuilder(SBG::LIB::SBG sb_graph)
  : _sb_graph(sb_graph), _vertex_map() {}

OG::Graph OrdinaryGraphBuilder::build()
{
  OG::Graph graph;

  for (const SBG::LIB::SetPiece &v : _sb_graph.V()) { 
    assert(!v.isEmpty());
    SBG::LIB::MD_NAT beg = v.minElem(), end = v.maxElem();
    for (auto it = beg; it != end; it = nextElem(it, v))
      addVertex(it, graph);
    addVertex(end, graph);
  }

  for (const SBG::LIB::SetPiece &e : _sb_graph.E()) {
    assert(!e.isEmpty());
    SBG::LIB::MD_NAT beg = e.minElem(), end = e.maxElem();
    for (auto it = beg; it != end; it = nextElem(it, e)) 
      addEdge(it, graph);
    addEdge(end, graph);
  }

  return graph;
}

OG::VertexDesc OrdinaryGraphBuilder::addVertex(
  SBG::LIB::MD_NAT id, OG::Graph &g
)
{
  Vertex V(id);
  OG::VertexDesc v = boost::add_vertex(g);
  _vertex_map[id] = v;
  g[v] = V;
  return v;
}

OG::EdgeDesc OrdinaryGraphBuilder::addEdge(
  SBG::LIB::MD_NAT id, OG::Graph &g
)
{
  SBG::LIB::SetPiece mdi(id);
  SBG::LIB::PWMap map1 = _sb_graph.map1(), map2 = _sb_graph.map2();
  SBG::LIB::UnordAF fact;
  SBG::LIB::MD_NAT v1 = map1.image(fact.createSet(mdi)).minElem();
  SBG::LIB::MD_NAT v2 = map2.image(fact.createSet(mdi)).minElem();

  Edge E(id);
  OG::EdgeDesc e;
  bool b;
  boost::tie(e, b) = boost::add_edge(_vertex_map[v1], _vertex_map[v2], g);
  g[e] = E;
  return e;
}

// Directed graph builder ------------------------------------------------------

OrdinaryDGraphBuilder::OrdinaryDGraphBuilder(SBG::LIB::DSBG sb_graph)
  : _sb_graph(sb_graph), _vertex_map() {}

OG::DGraph OrdinaryDGraphBuilder::build()
{
  OG::DGraph graph;

  std::chrono::microseconds total(0);
  for (const SBG::LIB::SetPiece &v : _sb_graph.V()) { 
    assert(!v.isEmpty());
    SBG::LIB::MD_NAT beg = v.minElem(), end = v.maxElem();
    for (auto it = beg; it != end; it = nextElem(it, v)) { 
      auto tbegin = std::chrono::high_resolution_clock::now();
      addVertex(it, graph);
      auto tend = std::chrono::high_resolution_clock::now();
      total += std::chrono::duration_cast<std::chrono::microseconds>(
        tend - tbegin
      );
    }
    addVertex(end, graph);
  }

  SBG::LIB::UnordAF fact;
  SBG::LIB::PWMap mapb = _sb_graph.mapB(), mapd = _sb_graph.mapD();
  for (const SBG::LIB::SetPiece &e : _sb_graph.E()) {
    assert(!e.isEmpty());
    SBG::LIB::MD_NAT beg = e.minElem(), end = e.maxElem();
    for (auto it = beg; it != end; it = nextElem(it, e)) {
      SBG::LIB::SetPiece mdi(it);
      SBG::LIB::MD_NAT v1 = mapb.image(fact.createSet(mdi)).minElem();
      SBG::LIB::MD_NAT v2 = mapd.image(fact.createSet(mdi)).minElem();
      auto tbegin = std::chrono::high_resolution_clock::now();
      addEdge(it, v1, v2, graph);
      auto tend = std::chrono::high_resolution_clock::now();
      total += std::chrono::duration_cast<std::chrono::microseconds>(
        tend - tbegin
      );
    }
    SBG::LIB::SetPiece mdi(end);
    SBG::LIB::MD_NAT v1 = mapb.image(fact.createSet(mdi)).minElem();
    SBG::LIB::MD_NAT v2 = mapd.image(fact.createSet(mdi)).minElem();
    addEdge(end, v1, v2, graph);
  }
  SBG::Util::SBG_LOG << "Scalar directed graph builder time: " 
    << total.count() << " [μs]\n";

  return graph;
}

OG::DVertexDesc OrdinaryDGraphBuilder::addVertex(
  SBG::LIB::MD_NAT id, OG::DGraph &g
)
{
  Vertex V(id);
  OG::DVertexDesc v = boost::add_vertex(g);
  _vertex_map[id] = v;
  g[v] = V;
  return v;
}

OG::DEdgeDesc OrdinaryDGraphBuilder::addEdge(
  SBG::LIB::MD_NAT id, SBG::LIB::MD_NAT v1, SBG::LIB::MD_NAT v2, OG::DGraph &g
)
{
  Edge E(id);
  OG::DEdgeDesc e;
  bool b;
  boost::tie(e, b) = boost::add_edge(_vertex_map[v1], _vertex_map[v2], g);
  g[e] = E;
  return e;
}

}  // namespace OG
