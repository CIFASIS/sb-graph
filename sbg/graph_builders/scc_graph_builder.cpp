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

/*
void SCCGraphBuilder::partitionVertices() 
{
  Set matched = mtchng().matchedE(), unmatched = mtchng().matchedE();

  BOOST_FOREACH (SetVertexDesc dv, vertices(grph)) {
    Set new_v = grph[dv];

    BOOST_FOREACH (SetEdgeDesc de, out_edges(grph)) {
      Set v_range = grph[dv].range();
      

      if (!v.cap(mtchng.F()).empty()) {
        PWLMap map = grph[de].map_f();
        new_v = v_range.cup(v_range.cap(map.image(matched)));
        new_v = v_range.cup(v_range.cap(map.image(unmatched)));
      }
 
      else {
        PWLMap map = grph[de].map_u();
        new_v = v_range.cup(v_range.cap(map.image(matched)));
        new_v = v_range.cup(v_range.cap(map.image(unmatched)));
      }
    }

    grph[dv] = new_v;
  }
}

void SCCGraphBuilder::partitionVertices() 
{
  SBGraph grph = mtchng().g();
  Set matched = mtchng_ref().matchedV(), unmatched = mtchng_ref().unmatchedV();

  BOOST_FOREACH (SetVertexDesc dv, vertices(grph)) {
    SetVertex v = grph[dv];
    Set new_v, v_range = v.range();
 
    new_v = v_range.cap(matched);
    new_v = v_range.cup(v_range.cap(unmatched));

    grph[dv] = SetVertex(v.name(), v.id(), new_v, v.index(), v.desc());
  }
}

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
        
        if (!matched_f.empty()) {
          Set adj_matched = grph[adj_e].map_f().preImage(matched_f);

          DSetEdge directed_e("");

          SetEdgeDesc de;
          bool b;
          boost::tie(directed_e, b) = boost::add_edge(vertex_map[vf], vertex_map[vu], result);
          result[de] = directed_e;
        }
      }
    }
  }

  return result;
}


DSBGraph SCCGraphBuilder::build() 
{
  DSBGraph result;

  SBGraph grph = mtchng().g();
  Set matched = mtchng().matchedE();

  partitionVertices();

  BOOST_FOREACH (SetEdgeDesc e, edges(grph)) {
    Set e_dom = grph[e].map_f().wholeDom().cap(matched);

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

  BOOST_FOREACH (SetEdgeDesc de, edges(grph)) {
   SetEdge e = grph[de];

   if (!e.dom().cap(matched).empty()) { // Set-edge has matched edges
      PWLMap e_f = e.map_f(); 
      Set matched_f = e_f.image(e.dom().cap(matched)); // Matched vertices for edge e

      SetVertexDesc vf = findEquation(de);
      BOOST_FOREACH (SetEdgeDesc adj_de, out_edges(vf, grph)) {
        if (de != adj_de) {
          SetVertexDesc vu = findUnknown(adj_de);

          
          if (!matched_f.empty()) {
            Set adj_matched = grph[adj_de].map_f().preImage(matched_f);

            DSetEdge directed_e;

            DSetEdgeDesc dde;
            bool b;
            boost::tie(dde, b) = boost::add_edge(vertex_map()[vf], vertex_map()[vu], result);
            result[dde] = directed_e;
          }
        }
      }
    }
  }

  return result;
}

DSBGraph SCCGraphBuilder::build() 
{
  DSBGraph result;

  SBGraph grph = mtchng().g();
  Set matched = mtchng().matchedE();

  BOOST_FOREACH (SetEdgeDesc de, edges(grph)) {
    Set e_dom = grph[de].dom().cap(matched);

    if (!e_dom.empty()) {
      SetVertex source_v = grph[source(de, grph)];
      SetVertex target_v = grph[target(de, grph)];
      SetVertex v(source_v.name() + target_v.name(), grph[de].id(), e_dom, 0);
  
      DSetVertexDesc dv = boost::add_vertex(result);
      result[dv] = v; 
  
      vertex_map()[source(de, grph)] = dv; 
      vertex_map()[target(de, grph)] = dv; 
    }
  }

  BOOST_FOREACH (SetVertexDesc df, vertices(grph)) {
    Set v_range = grph[df].range();

    if (!v_range.cap(mtchng().F()).empty()) { // Set-vertex is an equation
      BOOST_FOREACH (MultiInterval v_mi, v_range.cap(matched).asets()) {
        BOOST_FOREACH (SetEdgeDesc de, out_edges(df, grph)) {
          Set e_dom = grph[de].map_f_ref().preImage(Set(v_mi));

          if (e_dom.cap(matched).empty()) { // Look for unmatched edges
            SetVertexDesc du = vertex(0, grph);
            if (df == source(de, grph)) {
              du = target(de, grph);
            }
         
            else {
              du = source(de, grph);
            }
          }
        }
      }
    }
  } 

  return result;
}
*/

void SCCGraphBuilder::partitionEdges()
{
  SBGraph grph = mtchng_ref().g();
  Set matchedV = mtchng_ref().matchedV(), unmatchedV = mtchng_ref().unmatchedV();
  Set unmatchedE = mtchng_ref().allEdges_ref().diff(mtchng_ref().matchedE());

  BOOST_FOREACH (SetEdgeDesc de, edges(grph)) {
    Set e_dom = grph[de].dom().cap(unmatchedE); // Unmatched edges of a set-edge
    Set vs_range = grph[source(de, grph)].range(), vt_range = grph[target(de, grph)].range();
    
    if (!e_dom.empty()) {
      PWLMap map_f = grph[de].map_f(), map_u = grph[de].map_u();
      PWLMap new_f, new_u;

      if (!map_f.image().cap(vs_range).empty()) {
        PWLMap auxf1 = map_f.restrictMap(map_f.preImage(vs_range.cap(matchedV)));
        PWLMap auxf2 = map_f.restrictMap(map_f.preImage(vs_range.cap(unmatchedV)));
        new_f = auxf1.concat(auxf2);

        PWLMap auxu1 = map_u.preImage(vt_range.cap(matchedV));
        PWLMap auxu2 = map_u.preImage(vt_range.cap(unmatchedV));
        new_u = auxu1.concat(auxu2);
      }

      else {
        PWLMap auxf1 = map_f.preImage(vt_range.cap(matchedV));
        PWLMap auxf2 = map_f.preImage(vt_range.cap(unmatchedV));
        new_f = auxf1.concat(auxf2);

        PWLMap auxu1 = map_u.preImage(vs_range.cap(matchedV));
        PWLMap auxu2 = map_u.preImage(vs_range.cap(unmatchedV));
        new_u = auxu1.concat(auxu2);
      }

      SetEdge e = grph[de];
      grph[de] = SetEdge(e.name(), e.id(), new_f, new_u, e.index(), e.desc());
    }
  }
}

SetVertexDesc SCCGraphBuilder::findEquation(SetEdgeDesc de) 
{
  SBGraph grph = mtchng().g();

  SetVertexDesc result = vertex(1, grph);

  BOOST_FOREACH (SetVertexDesc dv, vertices(grph)) {
    Set v_range = grph[dv].range(), f_image = grph[de].map_f_ref().image();
    if (!v_range.cap(f_image).empty())
      result = dv;
  }

  return result;
}

SetVertexDesc SCCGraphBuilder::findUnknown(SetEdgeDesc de) 
{
  SBGraph grph = mtchng().g();

  SetVertexDesc result = vertex(1, grph);

  BOOST_FOREACH (SetVertexDesc dv, vertices(grph)) {
    Set v_range = grph[dv].range(), u_image = grph[de].map_u_ref().image();
    if (!v_range.cap(u_image).empty())
      result = dv;
  }

  return result;
}

bool SCCGraphBuilder::fullyMatched(Set vertices) { return vertices.subseteq(mtchng_ref().matchedV()); }

DSBGraph SCCGraphBuilder::build() 
{
  DSBGraph result;

  SBGraph grph = mtchng_ref().g();
  Set matched = mtchng_ref().matchedE(), unmatched = mtchng_ref().allEdges_ref().diff(matched);

  BOOST_FOREACH (SetEdgeDesc de, edges(grph)) {
    Set e_dom = grph[de].dom().cap(matched);

    if (!e_dom.empty()) {
      SetVertex source_v = grph[source(de, grph)];
      SetVertex target_v = grph[target(de, grph)];
      SetVertex v(source_v.name() + target_v.name(), grph[de].id(), e_dom, 0);
  
      SetVertexDesc dv = boost::add_vertex(result);
      result[dv] = v; 
  
      vertex_map()[source(de, grph)] = dv; 
      vertex_map()[target(de, grph)] = dv; 
    }
  }

  PWLMap mapF = mtchng().mapF(), mapU = mtchng().mapU();

  BOOST_FOREACH (SetEdgeDesc de, edges(grph)) {
    SetEdge e = grph[de];
    Set e_dom = e.dom().cap(unmatched); // Unmatched edges of a set-edge

    SetVertexDesc vf = findEquation(de), vu = findUnknown(de);
    SetVertexDesc dvf = vertex_map()[vf], dvu = vertex_map()[vu];

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
