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

// Directed graph converter -----------------------------------------------------------------------

DirectedConverter::DirectedConverter() : dg_(), compacted_vertices_(), converted_vertices_(), converted_edges_(), converted_dg_() {}
DirectedConverter::DirectedConverter(DSBGraph dg) : dg_(dg), compacted_vertices_(), converted_vertices_(), converted_edges_(), converted_dg_() {}

member_imp(DirectedConverter, DSBGraph, dg);
member_imp(DirectedConverter, CompactVertexMap, compacted_vertices);
member_imp(DirectedConverter, ConverterVertexMap, converted_vertices);
member_imp(DirectedConverter, ConverterEdgeMap, converted_edges);
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

std::string iterators[7] = {"i", "j", "k", "l", "m", "n", "o"};

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

  CompPair mi_offset = get_vertex(mi);

  int j = 0;
  parallel_foreach3 (mi.inters_ref(), mi_offset.first.inters_ref(), mi_offset.second.inters_ref()) {
    Interval i = boost::get<0>(items), i_off = boost::get<1>(items), i_converted = boost::get<2>(items);
    REAL m = i.step()/i_off.step(), h = i_converted.lo() + (i.lo() - i_off.lo()) / i_off.step() - m;
    le_res.insert(le_res.end(), LinearExp(m, h, iterators[j]));
 
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
      mi_lm.addGO();

      j++;
    }

    compacted_vertices_ref()[mi] = mi_lm;
  }

  VertexDef res(v.name(), subs_res);
  converted_vertices_ref().insert(VConv(vd, res));
  return res;
}

EdgeDefs DirectedConverter::convert_edge(DSetEdgeDesc ed)
{
  EdgeDefs eds_res;

  DSetEdge e = dg_ref()[ed];
  DSetVertexDesc es = source(ed, dg_ref()), et = target(ed, dg_ref());

  BOOST_FOREACH (MultiInterval mi_dom, e.dom().asets()) {
    AtomPWLMap mapb(mi_dom, e.map_b().getLM(mi_dom)), mapd(mi_dom, e.map_d().getLM(mi_dom));

    Iterators is = create_iterators(mi_dom);
    Ranges rs = create_ranges(mi_dom);

    MultiInterval mi_b = mapb.image(mi_dom), mi_d = mapd.image(mi_dom);
    LinearExps le_b = create_exp(mi_b), le_d = create_exp(mi_d);
    VertexUsage vb(dg_ref()[es].name(), le_b), vd(dg_ref()[et].name(), le_d);

    eds_res.insert(eds_res.end(), EdgeDef(is, rs, EdgeUsages(1, EdgeUsage(vb, vd))));
  }

  converted_edges_ref().insert(EConv(ed, eds_res));
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
}


/*
AnnotatedGraphIO DirectedConverter::convert_with_annotations(Annotations ans)
{
  GraphIO converted = convert_graph();

  return AnnotatedGraphIO(ans, converted);
}
*/

// Graph getters ----------------------------------------------------------------------------------

CompPair DirectedConverter::get_vertex(MultiInterval mi)
{
  CompPair res;

  BOOST_FOREACH (CompactVertexMap::value_type &cv, compacted_vertices_ref()) { 
    if (!mi.cap(cv.first).empty()) 
      res = cv;
  }

  return res;
}

VertexDefs DirectedConverter::get_vertex(SetVertex v)
{
  VertexDef res;

  VertexDef v_io;
  BOOST_FOREACH (VertexDef vd, converted_dg_ref().vds())
    if (vd.name() == v.name()) 
      v_io = vd;

  Set v_range = v.range(); 
  BOOST_FOREACH (MultiInterval mi, v_range.asets()) {
    CompPair compacted = get_vertex(mi); 
  }

  return res;
}

// Structure converters ---------------------------------------------------------------------------

RMapIO DirectedConverter::convert_map(PWLMap rmap)
{
  RMapIO result;

  Set whole_dom;
  BOOST_FOREACH (DSetVertexDesc vd, vertices(dg_ref())) {
    whole_dom.concat(dg_ref()[vd].range());
  }
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
          result.push_back(ComponentIO(is, rs, vrtcs));
      }
    }
  }

  return result;
}

}  // namespace IO

} // namespace SBG
