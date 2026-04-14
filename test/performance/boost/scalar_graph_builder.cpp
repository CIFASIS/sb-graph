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
#include "sbg/set_detail.hpp"
#include "sbg/set_fact.hpp"
#include "test/performance/boost/scalar_graph_builder.hpp"
#include "util/time_profiler.hpp"

namespace SBG {

namespace perf {

namespace detail {

using SBG::LIB::NAT;
using SBG::LIB::MD_NAT;
using SBG::LIB::Set;
using SBG::LIB::detail::SetAccessKey;
using SBG::LIB::detail::SetAccess;
using SBG::LIB::detail::MaybeMD_NAT;
using SBG::LIB::PWMap;
using SBG::LIB::BipartiteSBG;

////////////////////////////////////////////////////////////////////////////////
// Scalar undirected graph builder ---------------------------------------------
////////////////////////////////////////////////////////////////////////////////

ScalarGraphBuilder::ScalarGraphBuilder(BipartiteSBG bsbg)
  : _bsbg(bsbg), _vertex_map(), _partition() {}

BipartiteGraph ScalarGraphBuilder::build()
{
  translateVertices();
  EdgeVector edges = getEdgeList();
  Graph graph(edges.begin(), edges.end(), _bsbg.V().cardinal());
  return BipartiteGraph(std::move(graph), std::move(_partition));
}

BipartiteGraph ScalarGraphBuilder::build(NAT number_vertices
  , EdgeVector& E, std::vector<int>&& partition)
{
  Graph graph(E.begin(), E.end(), number_vertices);
  return BipartiteGraph(std::move(graph), std::move(_partition));
}

void ScalarGraphBuilder::translateVertices()
{

  Set V = _bsbg.V();
  if (V.isEmpty()) {
    return;
  }

  _partition.reserve(V.cardinal());
  Set X = _bsbg.X();

  NAT count = 0;
  SetAccessKey key = SetAccess::key();
  std::vector<MD_NAT> vertices = key.flatten(V);
  for (const MD_NAT& v : vertices) { 
    _vertex_map[v] = count;
    _partition.emplace_back(
      SBG::LIB::SET_FACT.createSet(v).intersection(X).isEmpty());
    ++count;
  }
}

EdgeVector ScalarGraphBuilder::getEdgeList()
{
  EdgeVector result;
  const Set& E = _bsbg.E();
  if (E.isEmpty()) {
    return result;
  }
  result.reserve(E.cardinal());

  const PWMap& map1 = _bsbg.map1();
  const PWMap& map2 = _bsbg.map2();
  SetAccessKey key = SetAccess::key();
  std::vector<MD_NAT> edges = key.flatten(E);
  for (const MD_NAT& e : edges) {
    // Get endings of edge
    Set domain = SBG::LIB::SET_FACT.createSet(e);
    Vertex v1 = _vertex_map[map1.image(domain).minElem()];
    Vertex v2 = _vertex_map[map2.image(domain).minElem()];
    result.emplace_back(v1, v2);
  }

  return result;
}

} // namespace detail

} // namespace perf

} // namespace SBG
