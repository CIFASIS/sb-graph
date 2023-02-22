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
#include <parser/converter.hpp>

namespace Parser {

// Structures -------------------------------------------------------------------------------------

FieldVisitor::FieldVisitor(ConstantsEnv cenv) : cenv_(cenv) {}

member_imp(FieldVisitor, ConstantsEnv, cenv);

Converter::Converter() : offset_() { set_sg(Parser::SetGraph()); }

Converter::Converter(Parser::SetGraph sg) : offset_() {
  set_sg(sg);
  sg_ref().createConstantsEnv();

  if (!sg.svertices().empty()) {
    Parser::SetVertex v1 = *(sg.svertices_ref().begin());
    Parser::MultiInterval mi1 = *(v1.range_ref().asets_ref().begin());
    
    for (unsigned int i = 0; i < mi1.inters_ref().size(); i++)
      offset_ref().insert(offset_ref().end(), 1);
  }
}

member_imp(Converter, SBG::OrdCT<SBG::INT>, offset);
member_imp(Converter, Parser::SetGraph, sg);

SBG::INT Converter::convertLE(Parser::LinearExp le)
{
  return le.m() * sg().cenv()[le.x()] + le.h();
}

SBG::Interval Converter::convertInterval(Parser::Interval i) 
{ 
  SBG::INT res_lo = convertLE(i.lo()), res_step = convertLE(i.step()), res_hi = convertLE(i.hi());

  return SBG::Interval(res_lo, res_step, res_hi); 
}

SBG::MultiInterval Converter::convertMI(Parser::MultiInterval mi)
{
  SBG::MultiInterval mi_res;

  BOOST_FOREACH (Parser::Interval i, mi.inters())
    mi_res.addInter(convertInterval(i));

  return mi_res;
}

SBG::Set Converter::convertSet(Parser::Set s)
{
  SBG::Set s_res;

  BOOST_FOREACH (Parser::MultiInterval mi, s.asets())
    s_res.addAtomSet(convertMI(mi));

  return s_res;
}

// Vertices ---------------------------------------------------------------------------------------

SBG::SetVertex Converter::convertVertex(Parser::SetVertex sv) { return SBG::SetVertex(sv.name(), convertSet(sv.range())); }

// Edges ------------------------------------------------------------------------------------------

SBG::MultiInterval Converter::makeDom(SBG::MultiInterval mi1, SBG::MultiInterval mi2) {
  SBG::MultiInterval result;

  SBG::OrdCT<SBG::INT> max_card;
  SBG::OrdCT<SBG::INT>::iterator it_max = max_card.begin();

  // Get number of elements in each dimension
  parallel_foreach2 (mi1.inters_ref(), mi2.inters_ref()) {
    SBG::INT n1 = boost::get<0>(items).card(), n2 = boost::get<1>(items).card();

    if ((n1 > n2 && n2 == 1) || n1 == n2) {
      it_max = max_card.insert(it_max, n1);
      ++it_max;
    }

    else if (n2 > n1 && n1 == 1) {
      it_max = max_card.insert(it_max, n2);
      ++it_max;
    }

    else
      return SBG::MultiInterval();
  }

  // Apply offset
  SBG::OrdCT<SBG::INT> new_off;
  parallel_foreach2 (offset_ref(), max_card) {
    SBG::INT off = boost::get<0>(items), elems = boost::get<1>(items);
    result.addInter(SBG::Interval(off, 1, off + elems - 1));

    new_off.insert(new_off.end(), off + elems);
  }
  set_offset(new_off);

  return result;
}

SBG::LMap Converter::makeExp(SBG::MultiInterval dom, SBG::MultiInterval mi) {
  SBG::LMap result;

  parallel_foreach2 (dom.inters_ref(), mi.inters_ref()) {
    SBG::Interval id = boost::get<0>(items), i = boost::get<1>(items);
    SBG::INT m = i.step(), h = i.lo() - m * id.lo();

    result.addGO(m, h - 1);
  } 

  return result;
}

void Converter::addEdge(SBG::SBGraph &g, Parser::SetEdge se) {
  SBG::MultiInterval mi_b = convertMI(se.mb()), mi_f = convertMI(se.mf());

  // Offset edge
  SBG::SetVertexDesc db = boost::vertex(0, g), df = db;
  SBG::OrdCT<SBG::INT> off1, off2;
  BOOST_FOREACH (SBG::SetVertexDesc vd, boost::vertices(g)) {
    if (g[vd].name() == se.vb()) { 
      db = vd;
      off1 = g[db].range().minElem();
    }

    if (g[vd].name() == se.vf()) {
      df = vd;
      off2 = g[df].range().minElem();
    }
  }

  SBG::MultiInterval off_mi_b = mi_b.offset(off1), off_mi_f = mi_f.offset(off2);

  // Create dom
  SBG::MultiInterval dom = makeDom(off_mi_b, off_mi_f); 

  // Create expressions
  SBG::LMap lmb = makeExp(dom, off_mi_b);
  SBG::LMap lmf = makeExp(dom, off_mi_f);

  // Create maps
  SBG::Set sdom;
  sdom.addAtomSet(dom);

  SBG::PWLMap mapb; 
  mapb.addSetLM(dom, lmb);
  SBG::PWLMap mapf; 
  mapf.addSetLM(dom, lmf);

  SBG::SetEdgeDesc ed;
  bool b;
  boost::tie(ed, b) = boost::add_edge(db, df, g);        
  g[ed] = SBG::SetEdge("", mapb, mapf);

  return;
}

void Converter::addDirectedEdge(SBG::DSBGraph &dg, Parser::SetEdge se) {
  SBG::MultiInterval mi_b = convertMI(se.mb()), mi_f = convertMI(se.mf());

  // Offset edge
  SBG::SetVertexDesc db = boost::vertex(0, dg), df = db;
  SBG::OrdCT<SBG::INT> off1, off2;
  BOOST_FOREACH (SBG::SetVertexDesc vd, boost::vertices(dg)) {
    if (dg[vd].name() == se.vb()) { 
      db = vd;
      off1 = dg[db].range().minElem();
    }

    if (dg[vd].name() == se.vf()) {
      df = vd;
      off2 = dg[df].range().minElem();
    }
  }

  SBG::MultiInterval off_mi_b = mi_b.offset(off1), off_mi_f = mi_f.offset(off2);

  // Create dom
  SBG::MultiInterval dom = makeDom(off_mi_b, off_mi_f); 

  // Create expressions
  SBG::LMap lmb = makeExp(dom, off_mi_b);
  SBG::LMap lmf = makeExp(dom, off_mi_f);

  // Create maps
  SBG::Set sdom;
  sdom.addAtomSet(dom);

  SBG::PWLMap mapb; 
  mapb.addSetLM(dom, lmb);
  SBG::PWLMap mapf; 
  mapf.addSetLM(dom, lmf);

  SBG::DSetEdgeDesc ed;
  bool b;
  boost::tie(ed, b) = boost::add_edge(db, df, dg);        
  dg[ed] = SBG::DSetEdge("", mapb, mapf);

  return;
}

// Graphs -----------------------------------------------------------------------------------------

SBG::SBGraph Converter::convertUndirectedGraph() 
{
  SBG::SBGraph result;

  if (!sg_ref().svertices_ref().empty()) {
    // Create vertices
    BOOST_FOREACH (Parser::SetVertex sv, sg().svertices()) {
      SBG::SetVertexDesc vd = boost::add_vertex(result);
      result[vd] = convertVertex(sv);
    }

    // Create edges
    BOOST_FOREACH (Parser::SetEdge se, sg().sedges()) 
      addEdge(result, se);
  }

  return result;
}

SBG::DSBGraph Converter::convertDirectedGraph() 
{
  SBG::DSBGraph result;

  if (!sg_ref().svertices_ref().empty()) {
    // Create vertices
    BOOST_FOREACH (Parser::SetVertex sv, sg().svertices()) {
      SBG::DSetVertexDesc vd = boost::add_vertex(result);
      result[vd] = convertVertex(sv);
    }

    // Create edges
    BOOST_FOREACH (Parser::SetEdge se, sg().sedges()) 
      addDirectedEdge(result, se);
  }

  return result;
}

Grph Converter::convertGraph()
{
  Grph result;

  if (sg().modifier() == "undirected") {
    result = convertUndirectedGraph();
  }

  else if (sg().modifier() == "directed"){
    result = convertDirectedGraph();
  }

  return result;
}

} // namespace Parser
