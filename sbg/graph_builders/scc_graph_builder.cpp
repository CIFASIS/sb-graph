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

bool SCCGraphBuilder::fullyMatched(Set vertices) { return vertices.subseteq(mtchng_ref().matchedV()); }

DSBGraph SCCGraphBuilder::build() 
{
  DSBGraph result;

  SBGraph grph = mtchng_ref().g();
  Set matched = mtchng_ref().matchedE(), unmatched = mtchng_ref().allEdges_ref().diff(matched);
  Set matchedV = mtchng_ref().matchedV(), unmatchedV = mtchng_ref().unmatchedV();

  BOOST_FOREACH (SetEdgeDesc de, edges(grph)) {
    Set e_dom = grph[de].dom().cap(matched);

    if (!e_dom.empty()) {
      SetVertex source_v = grph[source(de, grph)];
      SetVertex target_v = grph[target(de, grph)];
      SetVertex v(source_v.name() + target_v.name(), grph[de].id(), e_dom, 0);
  
      DSetVertexDesc dv = boost::add_vertex(result);
      result[dv] = v; 
  
      vertex_map_ref()[source(de, grph)] = dv; 
      vertex_map_ref()[target(de, grph)] = dv; 
    }
  }

  // Partition edges
  BOOST_FOREACH (SetEdgeDesc de, edges(grph)) {
    Set e_dom = grph[de].dom().cap(unmatched); // Unmatched edges of a set-edge
    Set vs_range = grph[source(de, grph)].range(), vt_range = grph[target(de, grph)].range();
    
    if (!e_dom.empty()) {
      PWLMap map_f = grph[de].map_f(), map_u = grph[de].map_u();
      PWLMap new_f, new_u;

      if (!map_f.image().cap(vs_range).empty()) {
        Set dom_f1 = map_f.preImage(vs_range.cap(matchedV));
        Set dom_f2 = map_f.preImage(vs_range.cap(unmatchedV));
        Set dom_f = dom_f1.concat(dom_f2);

        Set dom_u1 = map_u.preImage(vt_range.cap(matchedV));
        Set dom_u2 = map_u.preImage(vt_range.cap(unmatchedV));
        Set dom_u = dom_u1.concat(dom_u2);

        Set dom_fu = dom_f.cap(dom_u);

        new_f = map_f.restrictMap(dom_fu);
        new_u = map_u.restrictMap(dom_fu);
      }

      else {
        Set dom_f1 = map_f.preImage(vt_range.cap(matchedV));
        Set dom_f2 = map_f.preImage(vt_range.cap(unmatchedV));
        Set dom_f = dom_f1.concat(dom_f2);

        Set dom_u1 = map_u.preImage(vs_range.cap(matchedV));
        Set dom_u2 = map_u.preImage(vs_range.cap(unmatchedV));
        Set dom_u = dom_u1.concat(dom_u2);

        Set dom_fu = dom_f.cap(dom_u);

        new_f = map_f.restrictMap(dom_fu);
        new_u = map_u.restrictMap(dom_fu);
      }

      SetEdge e = grph[de];
      SetEdge new_e(e.name(), e.id(), new_f, new_u, e.index(), e.desc());
      grph[de] = new_e;
    }
  }

  PWLMap mapF = mtchng().mapF(), mapU = mtchng().mapU();

  BOOST_FOREACH (SetEdgeDesc de, edges(grph)) {
    SetEdge e = grph[de];
    Set e_dom = e.dom().cap(unmatched); // Unmatched edges of a set-edge

    SetVertexDesc vf = vertex(1, grph), vu = vertex(1, grph);
  
    // Find equation
    BOOST_FOREACH(SetVertexDesc dv, vertices(grph)) {
      Set v_range = (grph[dv]).range(), f_image = (grph[de]).map_f_ref().image();
      if (!v_range.cap(f_image).empty())
        vf = dv;
    }

    // Find unknown
    BOOST_FOREACH(SetVertexDesc dv, vertices(grph)) {
      Set v_range = (grph[dv]).range(), f_image = (grph[de]).map_u_ref().image();
      if (!v_range.cap(f_image).empty())
        vu = dv;
    }

    DSetVertexDesc dvf = vertex_map_ref()[vf], dvu = vertex_map_ref()[vu];

    if (!e_dom.empty()) {
      bool first_time = true;
      PWLMap map_b, map_d;
      BOOST_FOREACH (MultiInterval mi_dom, e_dom.asets()) {
        PWLMap map_f = mapF.restrictMap(Set(mi_dom)); 
        PWLMap map_u = mapU.restrictMap(Set(mi_dom)); 

        Set f_vertex = map_f.image(), u_vertex = map_u.image();
        if (fullyMatched(f_vertex) && fullyMatched(u_vertex)) {
          Set adj_f = mapF.preImage(f_vertex).cap(matched); // Should have only 1 aset (because of partitionEdges)
          Set adj_u = mapU.preImage(u_vertex).cap(matched); // Should have only 1 aset (because of partitionEdges)

          AtomPWLMap new_f(mi_dom, *(adj_f.asets_ref().begin()));
          AtomPWLMap new_u(mi_dom, *(adj_u.asets_ref().begin()));

          map_b.addSetLM(Set(new_f.dom()), new_f.lmap());
          map_d.addSetLM(Set(new_u.dom()), new_u.lmap());
          
          first_time = false;
        }
      }

      if (!first_time) {
        DSetEdge directed_e(result[dvf].name() + result[dvu].name(), e.id(), map_b, map_d, 0);
        DSetEdgeDesc dde;
        bool b;
        boost::tie(dde, b) = boost::add_edge(vertex_map()[vf], vertex_map()[vu], result);
        result[dde] = directed_e;
      }
    }
  }

  return result;
}

} // namespace SBG
