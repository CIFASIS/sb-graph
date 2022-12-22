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

#include <sbg/graph_builders/scc_graph_builder.hpp>

namespace SBG {

SCCGraphBuilder::SCCGraphBuilder(MatchingStruct m) : mtchng_(m)
{
  set_vertex_map(VertexMap());
  set_edge_map(EdgeMap());
}; 

member_imp(SCCGraphBuilder, MatchingStruct, mtchng);
member_imp(SCCGraphBuilder, VertexMap, vertex_map);
member_imp(SCCGraphBuilder, EdgeMap, edge_map);

DSBGraph SCCGraphBuilder::build() 
{
  DSBGraph result;

  SBGraph grph = mtchng().g();
  Set matched = mtchng().matchedE();

  BOOST_FOREACH (SetEdgeDesc e, edges(grph)) {
    Set e_dom = grph[e].map_f().wholeDom().cap(matchedE);

    if (!e_dom.empty()) {
      SetVertex source_v = grph[source(e, grph)];
      SetVertex target_v = grph[target(e, grph)];
      SetVertex v(source_v.name() + target_v.name(), grph[e].id(), e_dom, 0);
  
      DSetVertexDesc dv = boost::add_vertex(result);
      result[dv] = v; 
  
      vertex_map()[source(e, grph)] = dv; 
      vertex_map()[target(e, grph)] = dv; 
    }
  }

  BOOST_FOREACH (SetEdgeDesc e, edges(grph)) {
    SetVertexDesc vf = findEquation(e);

    BOOST_FOREACH (SetEdgeDesc adj_e, out_edges(vf, grph)) {
      if (e != adj_e) {
        SetVertexDesc vu = findUnknown(adj_e);

        PWLMap e_f = grph[e].map_f(); 
        Set matched_f = e_f.image(e_f.wholeDom().cap(matched)); // Matched vertices for edge e

        DSetEdge directed_e("");

        SetEdgeDesc de;
        bool b;
        boost::tie(directed_e, b) = boost::add_edge(vertex_map[vf], vertex_map[vu], result);
        result[de] = directed_e;
      }
    }
  }

  return result;
}

} // namespace SBG
