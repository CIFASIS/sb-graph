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
#include <sbg/converter_io.hpp>

namespace SBG {

namespace IO {

std::string iterators[7] = {"i", "j", "k", "l", "m", "n", "o"};

// Undirected graph converter -----------------------------------------------------------------------

UndirectedConverter::UndirectedConverter() : g_(), compacted_vertices_(), converted_vertices_(), converted_g_() {}
UndirectedConverter::UndirectedConverter(SBGraph g) : g_(g), compacted_vertices_(), converted_vertices_(), converted_g_() {}

member_imp(UndirectedConverter, SBGraph, g);
member_imp(UndirectedConverter, CompactVertexMap, compacted_vertices);
member_imp(UndirectedConverter, UConverterVertexMap, converted_vertices);
member_imp(UndirectedConverter, GraphIO, converted_g);

Range UndirectedConverter::convert_interval(Interval i)
{
  return Range(i.lo(), i.step(), i.hi());
}

Ranges UndirectedConverter::convert_multi_interval(MultiInterval mi)
{
  Ranges r;
  BOOST_FOREACH (Interval i, mi.inters_ref())
    r.insert(r.end(), convert_interval(i));

  return r;
}

Iterators UndirectedConverter::create_iterators(MultiInterval mi)
{
  Iterators is_res;

  for (int i = 0; i < mi.ndim(); i++)
    is_res.insert(is_res.end(), iterators[i]);

  return is_res;
}

Ranges UndirectedConverter::create_ranges(MultiInterval mi)
{
  MultiInterval mi_res;

  BOOST_FOREACH (Interval i, mi.inters()) 
    mi_res.addInter(Interval(1, 1, i.card()));

  return convert_multi_interval(mi_res);
}

// ! The argument mi should be part of a set-vertex
// ! of the graph
LinearExps UndirectedConverter::create_exp(MultiInterval mi)
{
  LinearExps le_res;

  CompPair off = get_vertex(mi);
  AtomPWLMap map_offset(mi, off.second);
  MultiInterval mi_converted = map_offset.image();

  MultiInterval ranges_mi;
  BOOST_FOREACH (Interval i, mi.inters()) 
    ranges_mi.addInter(Interval(1, 1, i.card()));

  LMap lexpr = mapFromMI(ranges_mi, mi_converted); 
  int j = 0;
  parallel_foreach2 (lexpr.gain_ref(), lexpr.offset_ref()) {
    SBG::REAL g = boost::get<0>(items), o = boost::get<1>(items);
    le_res.insert(le_res.end(), LinearExp(g, o, iterators[j]));
 
    j++;
  }

  return le_res;
}

// ------------------------------------------------------------------------------------------------

VertexDef UndirectedConverter::convert_vertex(SetVertexDesc vd)
{
  SetVertex v = g_ref()[vd];
  CompactIntervals subs_res(v.range_ref().ndim(), CompactInterval(0, 0));

  BOOST_FOREACH (MultiInterval mi, v.range_ref().sort()) {
    MultiInterval converted_mi;

    int j = 0;
    LMap mi_lm;
    BOOST_FOREACH (Interval i, mi.inters()) {
      int card_lo = subs_res[j].lo(), card_hi = subs_res[j].hi();
      subs_res[j] = CompactInterval(i.card() + card_lo, i.card() + card_hi);

      converted_mi.addInter(Interval(card_lo + 1, 1, card_lo + i.card()));

      j++;
    }

    compacted_vertices_ref()[mi] = mapFromMI(mi, converted_mi);
  }

  VertexDef res(v.id(), subs_res);
  converted_vertices_ref().insert(VConv(vd, res));
  return res;
}

EdgeDef UndirectedConverter::convert_subsetedge(SetEdge e, MultiInterval mi_dom, std::string s_name, std::string t_name) {
  AtomPWLMap mapf(mi_dom, e.map_f().getLM(mi_dom)), mapu(mi_dom, e.map_u().getLM(mi_dom));

  Iterators is = create_iterators(mi_dom);
  Ranges rs = create_ranges(mi_dom);

  MultiInterval mi_f = mapf.image(mi_dom), mi_u = mapu.image(mi_dom);
  LinearExps le_f = create_exp(mi_f), le_u = create_exp(mi_u);
  VertexUsage vf(s_name, le_f), vu(t_name, le_u);

  return EdgeDef(is, rs, EdgeUsages(1, EdgeUsage(vf, vu)));
}

EdgeDefs UndirectedConverter::convert_edge(SetEdgeDesc ed)
{
  EdgeDefs eds_res;

  SetEdge e = g_ref()[ed];
  SetVertexDesc es = source(ed, g_ref()), et = target(ed, g_ref());

  BOOST_FOREACH (MultiInterval mi_dom, e.dom().asets())
    eds_res.insert(eds_res.end(), convert_subsetedge(e, mi_dom, g_ref()[es].id(), g_ref()[et].id()));

  return eds_res;
}

GraphIO UndirectedConverter::convert_graph() 
{
  VertexDefs vds_res;
  BOOST_FOREACH (SetVertexDesc vd, vertices(g_ref())) { 
    VertexDef v = convert_vertex(vd);
    vds_res.insert(vds_res.end(), v);
  }

  EdgeDefs eds_res;
  BOOST_FOREACH (SetEdgeDesc ed, edges(g_ref())) { 
    EdgeDefs e = convert_edge(ed);
    BOOST_FOREACH (EdgeDef e_def, e)
      eds_res.insert(eds_res.end(), e_def);
  }

  GraphIO g(vds_res, eds_res);
  g.merge_edges();
  set_converted_g(g);

  return g;
}


// Graph getters ----------------------------------------------------------------------------------

CompPair UndirectedConverter::get_vertex(MultiInterval mi)
{
  CompPair res;

  BOOST_FOREACH (CompactVertexMap::value_type &cv, compacted_vertices_ref()) { 
    MultiInterval aux = cv.first;
    if (mi.subseteq(aux)) 
      res = cv;
  }

  return res;
}

VertexDef UndirectedConverter::get_vertex_def(std::string vertex_name)
{
  VertexDef res;

  BOOST_FOREACH (VertexDef vd, converted_g().vds())
    if (vd.name() == vertex_name)
      res = vd;

  return res;
}

EdgeDefs UndirectedConverter::get_edge(SetEdge e, std::string s_name, std::string t_name)
{
  EdgeDefs res;

  BOOST_FOREACH (MultiInterval mi_dom, e.dom().asets()) {
    EdgeDef ed = convert_subsetedge(e, mi_dom, s_name, t_name);
    res.insert(res.end(), ed);
  }

  return res;
}

// Structure converters ---------------------------------------------------------------------------

MatchingIO UndirectedConverter::convert_matching(Set edges)
{
  EdgeDefs eds;

  BOOST_FOREACH (MultiInterval mi, edges.asets()) {
    auto ed = SBG::get_edge(mi, g_ref());
    SetVertexDesc vs = source(*ed, g_ref()), vt = target(*ed, g_ref());
    EdgeDef e = convert_subsetedge(g_ref()[*ed], mi, g_ref()[vs].id(), g_ref()[vt].id());

    eds.push_back(e);
  }

  return MatchingIO(eds);
}

SCCIO UndirectedConverter::convert_map(PWLMap rmap)
{
  ComponentsIO cc_io;

  Set whole_dom;
  BOOST_FOREACH (SetVertexDesc vd, vertices(g_ref())) 
    whole_dom = whole_dom.concat(g_ref()[vd].range());
  rmap = rmap.restrictMap(whole_dom);

  parallel_foreach2 (rmap.dom_ref(), rmap.lmap_ref()) {
    Set d = boost::get<0>(items);
    LMap l = boost::get<1>(items);

    BOOST_FOREACH (MultiInterval mi, d.asets()) {
      AtomPWLMap apw(mi, l);

      if (apw.isId()) {
        Set same_component = rmap.preImage(Set(mi));

        Iterators is = create_iterators(mi);
        Ranges rs = create_ranges(mi);

        VertexUsages vrtcs;
        BOOST_FOREACH (MultiInterval represented, same_component.asets()) {
          LinearExps lexps = create_exp(represented);
          std::string nm = get_name(Set(represented), g_ref());
          vrtcs.push_back(VertexUsage(nm, lexps));
        }

        if (!vrtcs.empty())
          cc_io.push_back(ComponentIO(is, rs, vrtcs));
      }
    }
  }

  return SCCIO(cc_io);
}

// Directed graph converter -----------------------------------------------------------------------

DirectedConverter::DirectedConverter() : dg_(), compacted_vertices_(), converted_vertices_(), converted_dg_() {}
DirectedConverter::DirectedConverter(DSBGraph dg) : dg_(dg), compacted_vertices_(), converted_vertices_(), converted_dg_() {}

member_imp(DirectedConverter, DSBGraph, dg);
member_imp(DirectedConverter, CompactVertexMap, compacted_vertices);
member_imp(DirectedConverter, DConverterVertexMap, converted_vertices);
member_imp(DirectedConverter, GraphIO, converted_dg);

Range DirectedConverter::convert_interval(Interval i)
{
  return Range(i.lo(), i.step(), i.hi());
}

Ranges DirectedConverter::convert_multi_interval(MultiInterval mi)
{
  Ranges r;
  BOOST_FOREACH (Interval i, mi.inters_ref())
    r.insert(r.end(), convert_interval(i));

  return r;
}

Iterators DirectedConverter::create_iterators(MultiInterval mi)
{
  Iterators is_res;

  for (int i = 0; i < mi.ndim(); i++)
    is_res.insert(is_res.end(), iterators[i]);

  return is_res;
}

Ranges DirectedConverter::create_ranges(MultiInterval mi)
{
  MultiInterval mi_res;

  BOOST_FOREACH (Interval i, mi.inters()) 
    mi_res.addInter(Interval(1, 1, i.card()));

  return convert_multi_interval(mi_res);
}

// ! The argument mi should be part of a set-vertex
// ! of the graph
LinearExps DirectedConverter::create_exp(MultiInterval mi)
{
  LinearExps le_res;

  CompPair off = get_vertex(mi);
  AtomPWLMap map_offset(mi, off.second);
  MultiInterval mi_converted = map_offset.image();

  MultiInterval ranges_mi;
  BOOST_FOREACH (Interval i, mi.inters()) 
    ranges_mi.addInter(Interval(1, 1, i.card()));

  LMap lexpr = mapFromMI(ranges_mi, mi_converted); 
  int j = 0;
  parallel_foreach2 (lexpr.gain_ref(), lexpr.offset_ref()) {
    SBG::REAL g = boost::get<0>(items), o = boost::get<1>(items);
    le_res.insert(le_res.end(), LinearExp(g, o, iterators[j]));
 
    j++;
  }

  return le_res;
}

// ------------------------------------------------------------------------------------------------

VertexDef DirectedConverter::convert_vertex(DSetVertexDesc vd)
{
  SetVertex v = dg_ref()[vd];
  CompactIntervals subs_res(v.range_ref().ndim(), CompactInterval(0, 0));

  BOOST_FOREACH (MultiInterval mi, v.range_ref().sort()) {
    MultiInterval converted_mi;

    int j = 0;
    LMap mi_lm;
    BOOST_FOREACH (Interval i, mi.inters()) {
      int card_lo = subs_res[j].lo(), card_hi = subs_res[j].hi();
      subs_res[j] = CompactInterval(i.card() + card_lo, i.card() + card_hi);

      converted_mi.addInter(Interval(card_lo + 1, 1, card_lo + i.card()));

      j++;
    }

    compacted_vertices_ref()[mi] = mapFromMI(mi, converted_mi);
  }

  VertexDef res(v.id(), subs_res);
  converted_vertices_ref().insert(DVConv(vd, res));
  return res;
}

EdgeDef DirectedConverter::convert_subsetedge(DSetEdge e, MultiInterval mi_dom, std::string s_name, std::string t_name) {
  AtomPWLMap mapb(mi_dom, e.map_b().getLM(mi_dom)), mapd(mi_dom, e.map_d().getLM(mi_dom));

  Iterators is = create_iterators(mi_dom);
  Ranges rs = create_ranges(mi_dom);

  MultiInterval mi_b = mapb.image(mi_dom), mi_d = mapd.image(mi_dom);
  LinearExps le_b = create_exp(mi_b), le_d = create_exp(mi_d);
  VertexUsage vb(s_name, le_b), vd(t_name, le_d);

  return EdgeDef(is, rs, EdgeUsages(1, EdgeUsage(vb, vd)));
}

EdgeDefs DirectedConverter::convert_edge(DSetEdgeDesc ed)
{
  EdgeDefs eds_res;

  DSetEdge e = dg_ref()[ed];
  DSetVertexDesc es = source(ed, dg_ref()), et = target(ed, dg_ref());

  BOOST_FOREACH (MultiInterval mi_dom, e.dom().asets())
    eds_res.insert(eds_res.end(), convert_subsetedge(e, mi_dom, dg_ref()[es].id(), dg_ref()[et].id()));

  return eds_res;
}

GraphIO DirectedConverter::convert_graph() 
{
  VertexDefs vds_res;
  BOOST_FOREACH (DSetVertexDesc vd, vertices(dg_ref())) { 
    VertexDef v = convert_vertex(vd);
    vds_res.insert(vds_res.end(), v);
  }

  EdgeDefs eds_res;
  BOOST_FOREACH (DSetEdgeDesc ed, edges(dg_ref())) { 
    EdgeDefs e = convert_edge(ed);
    BOOST_FOREACH (EdgeDef e_def, e)
      eds_res.insert(eds_res.end(), e_def);
  }

  GraphIO g(vds_res, eds_res);
  g.merge_edges();
  set_converted_dg(g);

  return g;
}

// Graph getters ----------------------------------------------------------------------------------

CompPair DirectedConverter::get_vertex(MultiInterval mi)
{
  CompPair res;

  BOOST_FOREACH (CompactVertexMap::value_type &cv, compacted_vertices_ref()) { 
    MultiInterval aux = cv.first;
    if (mi.subseteq(aux)) 
      res = cv;
  }

  return res;
}

VertexDef DirectedConverter::get_vertex_def(std::string vertex_name)
{
  VertexDef res;

  BOOST_FOREACH (VertexDef vd, converted_dg().vds())
    if (vd.name() == vertex_name)
      res = vd;

  return res;
}

EdgeDefs DirectedConverter::get_edge(DSetEdge e, std::string s_name, std::string t_name)
{
  EdgeDefs res;

  BOOST_FOREACH (MultiInterval mi_dom, e.dom().asets()) {
    EdgeDef ed = convert_subsetedge(e, mi_dom, s_name, t_name);
    res.insert(res.end(), ed);
  }

  return res;
}

// Structure converters ---------------------------------------------------------------------------

SCCIO DirectedConverter::convert_map(PWLMap rmap)
{
  ComponentsIO cc_io;

  Set whole_dom;
  BOOST_FOREACH (DSetVertexDesc vd, vertices(dg_ref())) 
    whole_dom = whole_dom.concat(dg_ref()[vd].range_ref());
  rmap = rmap.restrictMap(whole_dom);

  parallel_foreach2 (rmap.dom_ref(), rmap.lmap_ref()) {
    Set d = boost::get<0>(items);
    LMap l = boost::get<1>(items);

    BOOST_FOREACH (MultiInterval mi, d.asets()) {
      AtomPWLMap apw(mi, l);

      if (apw.isId()) {
        Set same_component = rmap.preImage(Set(mi));

        Iterators is = create_iterators(mi);
        Ranges rs = create_ranges(mi);

        VertexUsages vrtcs;
        BOOST_FOREACH (MultiInterval represented, same_component.asets()) {
          LinearExps lexps = create_exp(represented);
          std::string nm = get_name(Set(represented), dg_ref());
          vrtcs.push_back(VertexUsage(nm, lexps));
        }

        if (!vrtcs.empty())
          cc_io.push_back(ComponentIO(is, rs, vrtcs));
      }
    }
  }

  return SCCIO(cc_io);
}

VertexOrderIO DirectedConverter::convert_vertex_order(VertexOrder vo)
{
  ComponentsIO cc_io;

  BOOST_FOREACH (Set v, vo) {
    if (!v.empty()) {
      BOOST_FOREACH (MultiInterval mi, v.asets()) {
        Iterators is = create_iterators(mi);
        Ranges rs = create_ranges(mi);

        VertexUsages vrtcs;

        LinearExps lexps = create_exp(mi);
        std::string nm = get_name(Set(mi), dg_ref());
        vrtcs.push_back(VertexUsage(nm, lexps));

        if (!vrtcs.empty())
          cc_io.push_back(ComponentIO(is, rs, vrtcs));
      }
    }
  }

  return VertexOrderIO(cc_io);
}

}  // namespace IO

} // namespace SBG
