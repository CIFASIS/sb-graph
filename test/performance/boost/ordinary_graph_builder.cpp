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

#include "test/performance/boost/ordinary_graph_builder.hpp"

namespace OG {

////////////////////////////////////////////////////////////////////////////////
// Auxiliary functions ---------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

static SBG::LIB::MD_NAT nextElem(SBG::LIB::MD_NAT curr, SBG::LIB::SetPiece mdi)
{
  assert(!mdi.isEmpty());
  SBG::LIB::MD_NAT min = mdi.minElem(), max = mdi.maxElem(), res;
  for (unsigned int j = 0; j < mdi.arity(); ++j) {
    if (curr[j] == max[j]) 
      res.emplaceBack(min[j]);

    else {
      res.emplaceBack(curr[j] + 1);
      for (unsigned int k = 1; k < mdi.arity() - j; ++k)
        res.emplaceBack(curr[j + k]);
      break;
    } 
  }

  return res;
}

////////////////////////////////////////////////////////////////////////////////
// Ordinary undirected graph builder -------------------------------------------
////////////////////////////////////////////////////////////////////////////////

OrdinaryGraphBuilder::OrdinaryGraphBuilder(SBG::LIB::SBG sbg)
  : _sbg(sbg), _vertex_map() {}

Graph OrdinaryGraphBuilder::build()
{
  translateVertices();
  EdgeVector edges = getEdgeList();
  return build(_sbg.V().cardinal(), edges);
}

Graph OrdinaryGraphBuilder::build(SBG::LIB::NAT number_vertices, EdgeVector& E)
{
  return Graph(E.begin(), E.end(), number_vertices);
}

void OrdinaryGraphBuilder::translateVertices()
{
  SBG::LIB::Set V = _sbg.V();
  SBG::LIB::NAT count = 0;
  for (const SBG::LIB::SetPiece& mdi : V) { 
    SBG::LIB::MD_NAT begin = mdi.minElem(), end = mdi.maxElem();
    for (auto it = begin; it != end; it = nextElem(it, mdi)) {
      _vertex_map[it] = count;
      ++count;
    }
    _vertex_map[end] = count;
    ++count;
  }
}

EdgeVector OrdinaryGraphBuilder::getEdgeList()
{
  EdgeVector result;
  SBG::LIB::Set E = _sbg.E();
  result.reserve(E.cardinal());

  SBG::LIB::PWMap map1 = _sbg.map1();
  SBG::LIB::PWMap map2 = _sbg.map2();
  for (const SBG::LIB::SetPiece& mdi : E) { 
    SBG::LIB::MD_NAT begin = mdi.minElem(), end = mdi.maxElem();
    for (auto it = begin; it != end; it = nextElem(it, mdi)) {
      // Get endings of edge
      SBG::LIB::SetPiece it_mdi(it);
      SBG::LIB::Set dom = SBG::LIB::SET_FACT.createSet(it_mdi);
      Vertex v1 = _vertex_map[map1.image(dom).minElem()];
      Vertex v2 = _vertex_map[map2.image(dom).minElem()];
      result.emplace_back(Edge(v1, v2));
    }
    SBG::LIB::SetPiece end_mdi(end);
    SBG::LIB::Set end_dom = SBG::LIB::SET_FACT.createSet(end_mdi);
    Vertex end_v1 = _vertex_map[map1.image(end_dom).minElem()];
    Vertex end_v2 = _vertex_map[map2.image(end_dom).minElem()];
    result.emplace_back(Edge(end_v1, end_v2));
  }

  return result;
}

}  // namespace OG
