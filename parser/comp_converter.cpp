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
#include <parser/comp_converter.hpp>

namespace Parser {

// Converter --------------------------------------------------------------------------------------

CompConverter::CompConverter() : vertex_offset_(), edge_offset_(), vmap_(), sg_() {}

CompConverter::CompConverter(Parser::CompSetGraph sg) : vertex_offset_(), edge_offset_(), vmap_(), sg_(sg) {
  sg_ref().createConstantsEnv();

  if (!sg.svertices().empty()) {
    Parser::CompSetVertex v1 = *(sg.svertices_ref().begin());
   
    BOOST_FOREACH (Parser::Expr mi, v1.size()) {
      vertex_offset_ref().insert(vertex_offset_ref().end(), 1);
      edge_offset_ref().insert(edge_offset_ref().end(), 1);
    }
  }
} 

member_imp(CompConverter, SBG::OrdCT<SBG::INT>, vertex_offset);
member_imp(CompConverter, SBG::OrdCT<SBG::INT>, edge_offset);
member_imp(CompConverter, CompVertexMap, vmap);
member_imp(CompConverter, Parser::CompSetGraph, sg);

// Set-vertex -------------------------------------------------------------------------------------

SBG::SetVertex CompConverter::convertVertex(Parser::CompSetVertex sv)
{
  SBG::MultiInterval mi_res;

  ExprVisitor ev(sg_ref().cenv());
  SBG::OrdCT<SBG::INT> new_off;

  parallel_foreach2 (sv.size_ref(), vertex_offset_ref()) {
    SBG::INT szi = boost::apply_visitor(ev, boost::get<0>(items));
    SBG::INT off = boost::get<1>(items);

    mi_res.addInter(SBG::Interval(off, 1, szi + off - 1));
    new_off.insert(new_off.end(), szi + off);
  }
 
  vmap_ref()[sv.name()] = mi_res; 
  set_vertex_offset(new_off);
  return SBG::SetVertex(sv.name(), SBG::Set(mi_res));
}


// Set-edge ---------------------------------------------------------------------------------------

SBG::MultiInterval CompConverter::createDom(SBG::MultiInterval mi)
{
  SBG::MultiInterval mi_res;
  
  SBG::OrdCT<SBG::INT> new_off;

  parallel_foreach2 (mi.inters_ref(), edge_offset_ref()) {
    SBG::INT szi = boost::get<0>(items).card();
    SBG::INT off = boost::get<1>(items);

    mi_res.addInter(SBG::Interval(off, 1, szi + off - 1));
    new_off.insert(new_off.end(), szi + off);
  }

  set_edge_offset(new_off);
  return mi_res;
}

SBG::LMap CompConverter::createMap(SBG::MultiInterval dom, SBG::MultiInterval sbs, std::string vertex)
{
  SBG::LMap result;
  SBG::MultiInterval vertex_dom = vmap_ref()[vertex];

  parallel_foreach3(dom.inters_ref(), sbs.inters_ref(), vertex_dom.inters_ref()) {
    SBG::Interval idom = boost::get<0>(items), isbs = boost::get<1>(items), ivdom = boost::get<2>(items);
 
    SBG::INT m = isbs.step();
    SBG::INT h = ivdom.lo() + isbs.lo() - m * idom.lo() - 1;

    result.addGO(m, h);
  }

  return result;
}

VariantEdge CompConverter::convertEdge(Parser::CompSetEdge se)
{
  VariantEdge result;
  ConstantsEnv c = sg_ref().cenv();

  VarUsage l = se.left(), r = se.right();
  SBG::MultiInterval lmi = convertMI(l.subs(), c), rmi = convertMI(r.subs(), c);
 
  SBG::MultiInterval mi_dom = createDom(lmi);
  SBG::LMap llm = createMap(mi_dom, lmi, l.name()), rlm = createMap(mi_dom, rmi, r.name());

  SBG::PWLMap lmap;
  lmap.addSetLM(SBG::Set(mi_dom), llm);
  SBG::PWLMap rmap;
  rmap.addSetLM(SBG::Set(mi_dom), rlm);

  std::string nm = l.name() + r.name();
  if (sg_ref().modifier() == "undirected") {
    result = SBG::SetEdge(nm, lmap, rmap);
  }

  else if (sg_ref().modifier() == "directed") {
    result = SBG::DSetEdge(nm, lmap, rmap);
  }

  return result;
}

// Graphs -----------------------------------------------------------------------------------------

SBG::SBGraph CompConverter::convertUndirectedGraph() 
{
  SBG::SBGraph result;
  VertexMap vm;

  if (!sg_ref().svertices_ref().empty()) {
    // Create vertices
    BOOST_FOREACH (Parser::CompSetVertex sv, sg().svertices()) {
      SBG::SetVertexDesc vd = boost::add_vertex(result);
      result[vd] = convertVertex(sv);
      vm[sv.name()] = vd;
    }

    // Create all edges
    CompEdgeMap emap;
    int i = 0; 
    BOOST_FOREACH (Parser::CompSetEdge se, sg_ref().sedges()) {
      VariantEdge res_se = convertEdge(se);
      emap[i] = res_se;
   
      i++;
    }

    // Lookup edges from the same set-edge
    std::set<VertexPair> visited;
    i = 0;
    BOOST_FOREACH (Parser::CompSetEdge se, sg_ref().sedges()) { 
      std::string lvertex = se.left().name(), rvertex = se.right().name();
      VertexPair combined_name(lvertex, rvertex);

      SBG::SetEdge res_se = boost::get<SBG::SetEdge>(emap[i]);
      SBG::PWLMap lmap = res_se.map_f(), rmap = res_se.map_u();

      if (visited.find(combined_name) == visited.end()) {
        int j = 0;
        BOOST_FOREACH (Parser::CompSetEdge se2, sg_ref().sedges_ref()) {
          std::string lvertex2 = se2.left().name(), rvertex2 = se2.right().name();

          SBG::SetEdge res_se2 = boost::get<SBG::SetEdge>(emap[j]);
          SBG::PWLMap lmap2 = res_se2.map_f(), rmap2 = res_se2.map_u();

          bool same_vertices1 = lvertex == lvertex2 && rvertex == rvertex2;
          bool same_vertices2 = lvertex == rvertex2 && rvertex == lvertex2;
          bool same_maps1 = se.left() == se2.left() && se.right() == se2.right(); 
          bool same_maps2 = se.left() == se2.right() && se.right() == se2.left(); 

          if ((same_vertices1 || same_vertices2) && !(same_maps1 || same_maps2)) {
            lmap = lmap.concat(lmap2);
            rmap = rmap.concat(rmap2);
          }

          j++;
        }

        visited.insert(VertexPair(lvertex, rvertex));
        visited.insert(VertexPair(rvertex, lvertex));

        SBG::SetEdgeDesc ed;
        bool b;
        boost::tie(ed, b) = boost::add_edge(vm[lvertex], vm[rvertex], result); 
        result[ed] = SBG::SetEdge(lvertex + rvertex, lmap, rmap);
      } 

      i++;
    }
  }

  return result;
}

SBG::DSBGraph CompConverter::convertDirectedGraph() 
{
  SBG::DSBGraph result;
  DVertexMap dvm;

  if (!sg_ref().svertices_ref().empty()) {
    // Create vertices
    BOOST_FOREACH (Parser::CompSetVertex sv, sg().svertices()) {
      SBG::DSetVertexDesc vd = boost::add_vertex(result);
      result[vd] = convertVertex(sv);
      dvm[sv.name()] = vd;
    }

    // Create all edges
    CompEdgeMap emap;
    int i = 0; 
    BOOST_FOREACH (Parser::CompSetEdge se, sg_ref().sedges()) {
      VariantEdge res_se = convertEdge(se);
      emap[i] = res_se;
    
      i++;
    }

    // Lookup edges from the same set-edge
    std::set<VertexPair> visited;
    i = 0;
    BOOST_FOREACH (Parser::CompSetEdge se, sg().sedges()) { 
      std::string lvertex = se.left().name(), rvertex = se.right().name();
      VertexPair combined_name(lvertex, rvertex);

      SBG::DSetEdge res_se = boost::get<SBG::DSetEdge>(emap[i]);
      SBG::PWLMap lmap = res_se.map_b(), rmap = res_se.map_d();

      if (visited.find(combined_name) == visited.end()) {
        int j = 0;
        BOOST_FOREACH (Parser::CompSetEdge se2, sg().sedges()) {
          std::string lvertex2 = se2.left().name(), rvertex2 = se2.right().name();

          SBG::DSetEdge res_se2 = boost::get<SBG::DSetEdge>(emap[j]);
          SBG::PWLMap lmap2 = res_se2.map_b(), rmap2 = res_se2.map_d();

          bool same_vertices = lvertex == lvertex2 && rvertex == rvertex2;
          bool same_maps = se.left() == se2.left() && se.right() == se2.right(); 

          if (same_vertices && !same_maps) {
            lmap = lmap.concat(lmap2);
            rmap = rmap.concat(rmap2);
          }

          j++;
        }

        visited.insert(VertexPair(lvertex, rvertex));

        SBG::DSetEdgeDesc ed;
        bool b;
        boost::tie(ed, b) = boost::add_edge(dvm[lvertex], dvm[rvertex], result); 
        result[ed] = SBG::DSetEdge(lvertex + rvertex, lmap, rmap);
      }

      i++;
    }
  }

  return result;
}

Grph CompConverter::convertGraph()
{
  Grph result;

  if (sg_ref().modifier() == "undirected") {
    result = convertUndirectedGraph();
  }

  else if (sg_ref().modifier() == "directed") {
    result = convertDirectedGraph();
  }
 
  return result;
}

} // namespace Parser
