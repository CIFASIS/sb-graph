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

SBG::Util::MD_NAT nextElem(SBG::Util::MD_NAT curr, SBG::LIB::SetPiece mdi)
{
  assert(!mdi.isEmpty());
  SBG::Util::MD_NAT min = mdi.minElem(), max = mdi.maxElem(), res;
  for (unsigned int j = 0; j < mdi.size(); ++j) {
    if (curr[j] == max[j]) 
      res.emplaceBack(min[j]);

    else {
      res.emplaceBack(curr[j] + 1);
      for (unsigned int k = 1; k < mdi.size() - j; ++k)
        res.emplaceBack(curr[j]);
      break;
    } 
  }

  return res;
}

template<typename Set>
OrdinaryGraphBuilder<Set>::OrdinaryGraphBuilder(
  OrdinaryGraphBuilder::SBGraph sb_graph
)
  : _sb_graph(sb_graph), _vertex_map() {}

template<typename Set>
OG::Graph OrdinaryGraphBuilder<Set>::build()
{
  OG::Graph graph;

  for (const SBG::LIB::SetPiece &v : _sb_graph.V()) { 
    assert(!v.isEmpty());
    SBG::Util::MD_NAT beg = v.minElem(), end = v.maxElem();
    for (auto it = beg; it != end; it = nextElem(it, v)) 
      addVertex(it, graph);
    addVertex(end, graph);
  }

  for (const SBG::LIB::SetPiece &e : _sb_graph.E()) {
    assert(!e.isEmpty());
    SBG::Util::MD_NAT beg = e.minElem(), end = e.maxElem();
    for (auto it = beg; it != end; it = nextElem(it, e)) 
      addEdge(it, graph);
    addEdge(end, graph);
  }

  return graph;
}

template<typename Set>
OG::VertexDesc OrdinaryGraphBuilder<Set>::addVertex(
  SBG::Util::MD_NAT id, OG::Graph &g
)
{
  Vertex V(id);
  OG::VertexDesc v = boost::add_vertex(g);
  _vertex_map[id] = v;
  g[v] = V;
  return v;
}

template<typename Set>
OG::EdgeDesc OrdinaryGraphBuilder<Set>::addEdge(
  SBG::Util::MD_NAT id, OG::Graph &g
)
{
  SBG::LIB::SetPiece mdi(id);
  PW map1 = _sb_graph.map1(), map2 = _sb_graph.map2();
  SBG::Util::MD_NAT v1 = map1.image(Set(mdi)).minElem();
  SBG::Util::MD_NAT v2 = map2.image(Set(mdi)).minElem();

  Edge E(id);
  OG::EdgeDesc e;
  bool b;
  boost::tie(e, b) = boost::add_edge(_vertex_map[v1], _vertex_map[v2], g);
  g[e] = E;
  return e;
}

template class OrdinaryGraphBuilder<SBG::LIB::UnordSet>;

template class OrdinaryGraphBuilder<SBG::LIB::OrdSet>;

}  // namespace OG
