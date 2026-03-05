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

#include "sbg/multidim_inter.hpp"
#include "test/performance/boost/ordinary_graph_builder.hpp"
#include "util/time_profiler.hpp"

namespace OG {

////////////////////////////////////////////////////////////////////////////////
// Auxiliary functions ---------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

static SBG::LIB::MD_NAT nextElem(SBG::LIB::MD_NAT curr
  , SBG::LIB::detail::MultiDimInter mdi)
{
  SBG::LIB::MD_NAT min = mdi.minElem();
  SBG::LIB::MD_NAT max = mdi.maxElem();
  SBG::LIB::MD_NAT result;
  for (unsigned int j = 0; j < mdi.arity(); ++j) {
    if (curr[j] == max[j]) {
      result.pushBack(min[j]);
    } else {
      result.pushBack(curr[j] + 1);
      for (unsigned int k = 1; k < mdi.arity() - j; ++k) {
        result.pushBack(curr[j + k]);
      }
      break;
    } 
  }

  return result;
}

////////////////////////////////////////////////////////////////////////////////
// Ordinary undirected graph builder -------------------------------------------
////////////////////////////////////////////////////////////////////////////////

OrdinaryGraphBuilder::OrdinaryGraphBuilder(SBG::LIB::BipartiteSBG bsbg)
  : _bsbg(bsbg), _vertex_map(), _partition() {}

BipartiteGraph OrdinaryGraphBuilder::build()
{
  translateVertices();
  EdgeVector edges = getEdgeList();
  Graph graph(edges.begin(), edges.end(), _bsbg.V().cardinal());
  return BipartiteGraph(std::move(graph), std::move(_partition));
}

BipartiteGraph OrdinaryGraphBuilder::build(SBG::LIB::NAT number_vertices
  , EdgeVector& E, std::vector<int>&& partition)
{
  Graph graph(E.begin(), E.end(), number_vertices);
  return BipartiteGraph(std::move(graph), std::move(_partition));
}

void OrdinaryGraphBuilder::translateVertices()
{
  SBG::LIB::Set V = _bsbg.V();
  _partition.reserve(V.cardinal());
  SBG::LIB::Set X = _bsbg.X();
  SBG::LIB::NAT count = 0;
  for (const SBG::LIB::SetPiece& mdi : V) { 
    SBG::LIB::MD_NAT begin = mdi.minElem();
    SBG::LIB::MD_NAT end = mdi.maxElem();
    for (auto it = begin; it != end; it = nextElem(it, mdi)) {
      _vertex_map[it] = count;
      _partition.emplace_back(
        SBG::LIB::SET_FACT.createSet(it).intersection(X).isEmpty());
      ++count;
    }
    _vertex_map[end] = count;
    _partition.emplace_back(
      SBG::LIB::SET_FACT.createSet(end).intersection(X).isEmpty());
    ++count;
  }
}

EdgeVector OrdinaryGraphBuilder::getEdgeList()
{
  EdgeVector result;
  const SBG::LIB::Set& E = _bsbg.E();
  result.reserve(E.cardinal());

  const SBG::LIB::PWMap& map1 = _bsbg.map1();
  const SBG::LIB::PWMap& map2 = _bsbg.map2();
  for (const SBG::LIB::SetPiece& mdi : E) { 
    SBG::LIB::MD_NAT begin = mdi.minElem();
    SBG::LIB::MD_NAT end = mdi.maxElem();
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
