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
  : _sbg(sbg), _graph(), _vertex_map() {}

const Graph& OrdinaryGraphBuilder::graph() const
{
  return _graph;
}

void OrdinaryGraphBuilder::build()
{
  VertexVector vertices = getVertexList();
  EdgeInfoVector edges = getEdgeList();
  build(vertices, edges);
}

void OrdinaryGraphBuilder::clear()
{
  _graph.clear();
}

void OrdinaryGraphBuilder::build(const VertexVector& V, const EdgeInfoVector& E)
{
  for (const Vertex& v : V) {
    addVertex(v);
  }
  for (const EdgeInfo& info : E) {
    addEdge(info);
  }
}

VertexVector OrdinaryGraphBuilder::getVertexList() const
{
  VertexVector result;
  SBG::LIB::Set V = _sbg.V();
  result.reserve(V.cardinal());

  for (const SBG::LIB::SetPiece& mdi : V) { 
    SBG::LIB::MD_NAT begin = mdi.minElem(), end = mdi.maxElem();
    for (auto it = begin; it != end; it = nextElem(it, mdi)) {
      result.emplace_back(Vertex(it));
    }
    result.emplace_back(Vertex(end));
  }

  return result;
}

EdgeInfoVector OrdinaryGraphBuilder::getEdgeList()
{
  EdgeInfoVector result;
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
      Vertex v1(map1.image(dom).minElem());
      Vertex v2(map2.image(dom).minElem());
      result.emplace_back(EdgeInfo(it, v1, v2));
    }
    SBG::LIB::SetPiece end_mdi(end);
    SBG::LIB::Set end_dom = SBG::LIB::SET_FACT.createSet(end_mdi);
    Vertex end_v1(map1.image(end_dom).minElem());
    Vertex end_v2(map2.image(end_dom).minElem());
    result.emplace_back(EdgeInfo(end, end_v1, end_v2));
  }

  return result;
}

void OrdinaryGraphBuilder::addVertex(const Vertex& v)
{
  VertexDesc v_desc = boost::add_vertex(_graph);
  _vertex_map[v.id()] = v_desc;
  _graph[v_desc] = v;
}

void OrdinaryGraphBuilder::addEdge(const EdgeInfo& info)
{
  EdgeDesc e_desc;
  bool b;
  boost::tie(e_desc, b) = boost::add_edge(_vertex_map[info._v1.id()]
    , _vertex_map[info._v2.id()], _graph);
  _graph[e_desc] = info._e;
}

////////////////////////////////////////////////////////////////////////////////
// Ordinary directed graph builder ---------------------------------------------
////////////////////////////////////////////////////////////////////////////////

EdgeInfo::EdgeInfo(Edge e, Vertex v1, Vertex v2) : _e(e), _v1(v1), _v2(v2) {}

OrdinaryDGraphBuilder::OrdinaryDGraphBuilder(SBG::LIB::DSBG dsbg)
  : _dsbg(dsbg), _dgraph(), _vertex_map() {}

const DGraph& OrdinaryDGraphBuilder::dgraph() const
{
  return _dgraph;
}

void OrdinaryDGraphBuilder::build()
{
  VertexVector vertices = getVertexList();
  EdgeInfoVector edges = getEdgeList();
  build(vertices, edges);
}

void OrdinaryDGraphBuilder::build(const VertexVector& V, const EdgeInfoVector& E)
{
  for (const Vertex& v : V) {
    addVertex(v);
  }
  for (const EdgeInfo& info : E) {
    addEdge(info);
  }
}

void OrdinaryDGraphBuilder::clear()
{
  _dgraph.clear();
}

VertexVector OrdinaryDGraphBuilder::getVertexList() const
{
  VertexVector result;
  SBG::LIB::Set V = _dsbg.V();
  result.reserve(V.cardinal());

  for (const SBG::LIB::SetPiece& mdi : V) { 
    SBG::LIB::MD_NAT begin = mdi.minElem(), end = mdi.maxElem();
    for (auto it = begin; it != end; it = nextElem(it, mdi)) {
      result.emplace_back(Vertex(it));
    }
    result.emplace_back(Vertex(end));
  }

  return result;
}

EdgeInfoVector OrdinaryDGraphBuilder::getEdgeList()
{
  EdgeInfoVector result;
  SBG::LIB::Set E = _dsbg.E();
  result.reserve(E.cardinal());

  SBG::LIB::PWMap mapB = _dsbg.mapB();
  SBG::LIB::PWMap mapD = _dsbg.mapD();

  for (const SBG::LIB::SetPiece& mdi : E) { 
    SBG::LIB::MD_NAT begin = mdi.minElem(), end = mdi.maxElem();
    for (auto it = begin; it != end; it = nextElem(it, mdi)) {
      // Get endings of edge
      SBG::LIB::SetPiece it_mdi(it);
      SBG::LIB::Set dom = SBG::LIB::SET_FACT.createSet(it_mdi);
      Vertex v1(mapB.image(dom).minElem());
      Vertex v2(mapD.image(dom).minElem());
      result.emplace_back(EdgeInfo(it, v1, v2));
    }
    SBG::LIB::SetPiece end_mdi(end);
    SBG::LIB::Set end_dom = SBG::LIB::SET_FACT.createSet(end_mdi);
    Vertex end_v1(mapB.image(end_dom).minElem());
    Vertex end_v2(mapD.image(end_dom).minElem());
    result.emplace_back(EdgeInfo(end, end_v1, end_v2));
  }

  return result;
}

void OrdinaryDGraphBuilder::addVertex(const Vertex& v)
{
  DVertexDesc v_desc = boost::add_vertex(_dgraph);
  _vertex_map[v.id()] = v_desc;
  _dgraph[v_desc] = v;
}

void OrdinaryDGraphBuilder::addEdge(const EdgeInfo& info)
{
  DEdgeDesc e_desc;
  bool b;
  boost::tie(e_desc, b) = boost::add_edge(_vertex_map[info._v2.id()]
    , _vertex_map[info._v1.id()], _dgraph);
  _dgraph[e_desc] = Edge(info._e);
}

}  // namespace OG
