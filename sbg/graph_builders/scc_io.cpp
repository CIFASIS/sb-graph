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
#include <sbg/graph_builders/scc_io.hpp>

namespace SBG {

namespace IO {

// Converter --------------------------------------------------------------------------------------

SCCConverter::SCCConverter() : dg_() {}
SCCConverter::SCCConverter(DSBGraph dg) : dg_(atomize(dg)) {}

member_imp(SCCConverter, AtomDSBGraph, dg);

VertexDef SCCConverter::convert_vertex(AtomDSVDesc vd)
{
  AtomSetVertex v = dg_ref()[vd];
  Ints subs_res(v.range_ref().ndim(), 0);

  int j = 0;
  BOOST_FOREACH (Interval i, v.range_ref().inters()) {
    subs_res[j] = subs_res[j] + i.card();

    j++;
  }

  return VertexDef(v.name(), subs_res);
}

std::string iterators[7] = {"i", "j", "k", "l", "m", "n", "o"};

Iterators SCCConverter::create_iterators(MultiInterval mi)
{
  Iterators is_res;

  for (int i = 0; i < mi.ndim(); i++)
    is_res.insert(is_res.end(), iterators[i]);

  return is_res;
}

Range SCCConverter::convert_interval(Interval i)
{
  return Range(i.lo(), i.step(), i.hi());
}

Ranges SCCConverter::convert_multi_interval(MultiInterval mi)
{
  Ranges r;
  BOOST_FOREACH (Interval i, mi.inters_ref())
    r.insert(r.end(), convert_interval(i));

  return r;
}

Ranges SCCConverter::create_ranges(MultiInterval mi)
{
  MultiInterval mi_res;

  BOOST_FOREACH (Interval i, mi.inters()) 
    mi_res.addInter(Interval(1, 1, i.card()));

  return convert_multi_interval(mi_res);
}

// ! The argument mi should be part of a set-vertex
// ! of the graph
LinearExps SCCConverter::create_exp(MultiInterval mi)
{
  LinearExps le_res;

  MultiInterval mi_offset;
  BOOST_FOREACH (DSetVertexDesc vd, vertices(dg_ref())) {
    Set v_dom = dg_ref()[vd].range();

    if (!v_dom.cap(Set(mi)).empty()) 
      mi_offset = *(v_dom.asets_ref().begin());
  }

  int j = 0;
  parallel_foreach2 (mi.inters_ref(), mi_offset.inters_ref()) {
    Interval i = boost::get<0>(items), i_off = boost::get<1>(items);
    REAL m = 1/i.step(), h = i.minElem() - i_off.minElem();
    le_res.insert(le_res.end(), LinearExp(m, h, iterators[j]));
 
    j++;
  }

  return le_res;
}


EdgeDefs SCCConverter::convert_edge(AtomDSEDesc ed)
{
  EdgeDefs eds_res;

  DSetEdge e = dg_ref()[ed];
  AtomDSVDesc es = source(ed, dg_ref()), et = target(ed, dg_ref());

  PWLMap mapb = e.map_b(), mapd = e.map_d();
  BOOST_FOREACH (MultiInterval mi_dom, e.dom().asets()) {
    Iterators is = create_iterators(mi_dom);
    Ranges rs = create_ranges(mi_dom);

    Set image_b = mapb.image(Set(mi_dom)), image_d = mapd.image(Set(mi_dom));
    MultiInterval mi_b = *(image_b.asets_ref().begin()), mi_d = *(image_d.asets_ref().begin());
    LinearExps le_b = create_exp(mi_b), le_d = create_exp(mi_d);
    VertexUsage vb(dg_ref()[es].name(), le_b), vd(dg_ref()[et].name(), le_d);

    eds_res.insert(eds_res.end(), EdgeDef(is, rs, vb, vd));
  }

  return eds_res;
}

GraphIO SCCConverter::convert() 
{
  VertexDefs vds_res;
  BOOST_FOREACH (AtomDSVDesc vd, vertices(dg_ref())) { 
    VertexDef v = convert_vertex(vd);
    vds_res.insert(vds_res.end(), v);
  }

  EdgeDefs eds_res;
  BOOST_FOREACH (AtomDSEDesc ed, edges(dg_ref())) { 
    EdgeDefs e = convert_edge(ed);
    BOOST_FOREACH (EdgeDef e_def, e)
      eds_res.insert(eds_res.end(), e_def);
  }

  return GraphIO(vds_res, eds_res);
}

}  // namespace IO

} // namespace SBG
