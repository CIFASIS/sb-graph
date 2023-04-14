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

SCCGraphBuilder::SCCGraphBuilder(MatchingStruct m) : mtchng_(m), result_(), vertex_map_() {} 

member_imp(SCCGraphBuilder, MatchingStruct, mtchng);
member_imp(SCCGraphBuilder, DSBGraph, result);
member_imp(SCCGraphBuilder, SCCVertexMap, vertex_map);
//member_imp(SCCGraphBuilder, SBG::IO::Annotations, merged_vertices);

bool SCCGraphBuilder::fullyMatched(Set vertices) { return vertices.subseteq(mtchng_ref().matchedV()); }

// Partition edges according to atomic sets
// of matched/not matched edges.
void SCCGraphBuilder::partitionEdges(SBGraph &grph)
{
  Set matched = mtchng_ref().matchedE(), unmatched = mtchng_ref().allEdges_ref().diff(matched);
  Set matchedV = mtchng_ref().matchedV(), unmatchedV = mtchng_ref().unmatchedV();

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
}

// ! grph sould have at least one edge
SetEdgeDesc SCCGraphBuilder::findEdge(SBGraph &grph, Set dom)
{
  EdgeIt e_start, e_end;
  boost::tie(e_start, e_end) = edges(grph);
  SetEdgeDesc e = *e_start;

  BOOST_FOREACH(SetEdgeDesc de, edges(grph)) {
    Set inter = grph[de].dom().cap(dom);
    if (!inter.empty())
      e = de;
  }

  return e;
}

// Traverse all edges to find the matched ones, which
// will result in a vertex the directed graph
void SCCGraphBuilder::createVertices(SBGraph &grph)
{
  Set matched = mtchng_ref().matchedE();

  BOOST_FOREACH (SetEdgeDesc de, edges(grph)) {
    Set e_dom = grph[de].dom().cap(matched);

    if (!e_dom.empty()) {
      SetVertexDesc s = source(de, grph), t = target(de, grph);
      SetVertex source_v = grph[s], target_v = grph[t];
      SetVertex v(source_v.name() + target_v.name(), grph[de].id(), e_dom, 0);
  
      DSetVertexDesc dv = boost::add_vertex(result_ref());
      result_ref()[dv] = v; 
  
      vertex_map_ref()[de] = dv; 

      SetVertex l = source_v.restrictVertex(mtchng_ref().mapF().image(e_dom));
      SetVertex r = target_v.restrictVertex(mtchng_ref().mapU().image(e_dom));
      //merged_vertices_ref().push_back(SBG::IO::Annotation(source_v, target_v, l, r));
    }
  }
}

// Traverse all edges {u, v} to find the unmatched ones.
// If both u and v are matched, there exists matched edges 
// e_u = {u, ???} and e_v = {v, ???}, such that and edge should
// be created in the directed graph. 
void SCCGraphBuilder::createEdges(SBGraph &grph)
{
  Set matched = mtchng_ref().matchedE(), unmatched = mtchng_ref().allEdges_ref().diff(matched);
  PWLMap mapF = mtchng_ref().mapF(), mapU = mtchng_ref().mapU();

  BOOST_FOREACH (SetEdgeDesc de, edges(grph)) {
    SetEdge e = grph[de];
    Set e_dom = e.dom().cap(unmatched); // Unmatched edges of a set-edge

    if (!e_dom.empty()) {
      BOOST_FOREACH (MultiInterval mi_dom, e_dom.asets()) {
        PWLMap map_b, map_d;

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

          if (!map_b.empty() && !map_d.empty()) {
            SetEdgeDesc ef = findEdge(grph, adj_f), eu = findEdge(grph, adj_u);
            DSetVertexDesc dvf = vertex_map_ref()[ef], dvu = vertex_map_ref()[eu];

            DSetEdge directed_e(result_ref()[dvf].name() + result_ref()[dvu].name(), e.id(), map_b, map_d, 0);
            DSetEdgeDesc dde;
            bool b;
            boost::tie(dde, b) = boost::add_edge(dvf, dvu, result_ref());
            result_ref()[dde] = directed_e;
          }
        }
      }
    }
  }
}

// After adding edges, there could be several set-edges
// between the same set-vertices, so we combine them.
DSBGraph SCCGraphBuilder::combineEdges()
{
  DSBGraph aux;
  DSBGraph res = result(), combined_result;

  std::map<DSetVertexDesc, DSetVertexDesc> vmap;
  BOOST_FOREACH (DSetVertexDesc dv, vertices(res)) {
    DSetVertexDesc dv_res = boost::add_vertex(combined_result);
    combined_result[dv_res] = res[dv]; 

    vmap[dv] = dv_res;
  }

  std::set<std::string> visited_nodes;
  BOOST_FOREACH (DSetEdgeDesc dei, edges(res)) {
    DSetEdge ei = res[dei];
    PWLMap mapb = ei.map_b(), mapd = ei.map_d();

    if (visited_nodes.find(ei.name()) == visited_nodes.end()) {
      BOOST_FOREACH (DSetEdgeDesc dej, edges(res)) {
        DSetEdge ej = res[dej];

        if (ei.name() == ej.name() && ei.dom() != ej.dom()) {
          visited_nodes.insert(ei.name());

          mapb = mapb.concat(ej.map_b());
          mapd = mapd.concat(ej.map_d());
        }
      }

      DSetEdge directed_e(ei.name(), ei.id(), mapb, mapd, 0);
      DSetEdgeDesc de_res;
      bool b;
      DSetVertexDesc sdei = source(dei, res), tdei = target(dei, res);
      boost::tie(de_res, b) = boost::add_edge(vmap[sdei], vmap[tdei], combined_result);
      combined_result[de_res] = directed_e;
    }

    visited_nodes.insert(ei.name());
  }

  return combined_result;
}

void SCCGraphBuilder::build() 
{
  SBGraph grph = mtchng_ref().g();

  partitionEdges(grph);
  createVertices(grph);
  createEdges(grph);
  set_result(combineEdges());
}

void SCCGraphBuilder::pretty_print() 
{
  DSBGraph grph = result_ref();
  SBG::IO::DirectedConverter c(grph);
  //SBG::IO::AnnotatedGraphIO grph_io = c.convert_with_annotations(merged_vertices_ref());
  SBG::IO::GraphIO grph_io = c.convert_graph();
  std::cout << grph_io << "\n";

  return;
}

} // namespace SBG
