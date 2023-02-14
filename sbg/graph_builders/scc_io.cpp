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

// Pretty-printer ---------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream &out, Ints &is)
{
  out << "[";

  if (is.size() > 0) {
    auto it = is.begin();
    for (; std::next(it) != is.end(); it++){
      int i = *it;
      out << i << ", ";
    }
 
    int i = *it;
    out << i;
  }

  out << "]";

  return out;
}

VertexDef::VertexDef() : name_(), subs_dims_() {}
VertexDef::VertexDef(std::string name, Ints subs_dims) : name_(name), subs_dims_(subs_dims) {};

member_imp(VertexDef, std::string, name);
member_imp(VertexDef, Ints, subs_dims);

std::ostream &operator<<(std::ostream &out, VertexDef &vd)
{
  out << vd.name() << " " << vd.subs_dims_ref();

  return out;
}

std::ostream &operator<<(std::ostream &out, VertexDefs &vds)
{
  BOOST_FOREACH (VertexDef vd, vds)
    out << vd << ";\n"; 

  return out;
}

LinearExp::LinearExp() : slope_(1), constant_(0), var_() {}
LinearExp::LinearExp(REAL slope, REAL constant, std::string var) : slope_(slope), constant_(constant), var_(var) {}

member_imp(LinearExp, REAL, slope);
member_imp(LinearExp, REAL, constant);
member_imp(LinearExp, std::string, var);

std::ostream &operator<<(std::ostream &out, LinearExp &le)
{
  if (le.slope() == 1) {
    out << le.var();
  } else if (le.slope() != 0) {
    out << le.slope() << " * " << le.var();
  }

  if (le.constant() > 0) {
    out << " + " << le.constant();
  } else if (le.constant() < 0) {
    out << le.constant();
  }

  return out;
}

std::ostream &operator<<(std::ostream &out, LinearExps &les)
{
  if (les.empty()) {
    return out;
  }

  unsigned int size = les.size() - 1;
  out << "[";
  for (unsigned int i = 0; i < size; i++) 
    out << les[i] << ",";
  out << les[size] << "]";

  return out;
}

VertexUsage::VertexUsage() : name_(), subs_() {}
VertexUsage::VertexUsage(std::string name, LinearExps subs) : name_(name), subs_(subs) {}

member_imp(VertexUsage, std::string, name);
member_imp(VertexUsage, LinearExps, subs);

std::ostream &operator<<(std::ostream &out, VertexUsage &vu)
{
  out << vu.name() << vu.subs_ref();

  return out;
}

Range::Range() : begin_(1), step_(1), end_(1) {}
Range::Range(INT begin, INT step, INT end) : begin_(begin), step_(step), end_(end) {}

member_imp(Range, INT, begin);
member_imp(Range, INT, step);
member_imp(Range, INT, end);

bool Range::operator<(const Range &other) const
{
  return begin() < other.begin();
}

std::ostream &operator<<(std::ostream &out, Range &r) 
{
  out << "[" << r.begin() << ":";
  if (r.step() != 1) {
    out << r.step() << ":";
  }
  out << r.end() << "]";

  return out;
}

std::ostream &operator<<(std::ostream &out, Ranges &ranges)
{
  int sz = ranges.size();
  if (sz == 1) {
    out << *(ranges.begin());
  }

  else if (sz > 1) {
    out << "[";
    for (int i = 0; i < sz - 1; i++) {
      Range r = ranges[i];

      out << r.begin() << ":";
      if (r.step() != 1)
        out << r.step() << ":";
      out << r.end() << ", ";
    }
    out << "]";
  }

  return out;
}

EdgeDef::EdgeDef() : vb_(), vf_() {}
EdgeDef::EdgeDef(Iterators is, Ranges rs, VertexUsage vb, VertexUsage vf) : is_(is), rs_(rs), vb_(vb), vf_(vf) {}

member_imp(EdgeDef, Iterators, is);
member_imp(EdgeDef, Ranges, rs);
member_imp(EdgeDef, VertexUsage, vb);
member_imp(EdgeDef, VertexUsage, vf);

std::ostream &operator<<(std::ostream &out, EdgeDef &ed)
{
  VertexUsage vub = ed.vb_ref(), vuf = ed.vf_ref();
  int sz = ed.is_ref().size(), i = 0;
  out << "for ";
  parallel_foreach2 (ed.is_ref(), ed.rs_ref()) {
    if (i < sz - 1)
      out << boost::get<0>(items) << " in " << boost::get<1>(items) << ", ";

    else
      out << boost::get<0>(items) << " in " << boost::get<1>(items);

    i++;
  }
  out << "\n  " << vub << " -> " << vuf << "\nendfor";

  return out;
}

std::ostream &operator<<(std::ostream &out, EdgeDefs &eds)
{
  BOOST_FOREACH (EdgeDef ed, eds)
    out << ed << ";\n";

  return out;
}

SCCGraphIO::SCCGraphIO() : vds_(), eds_() {}
SCCGraphIO::SCCGraphIO(VertexDefs vds, EdgeDefs eds) : vds_(vds), eds_(eds) {}

member_imp(SCCGraphIO, VertexDefs, vds);
member_imp(SCCGraphIO, EdgeDefs, eds);

std::ostream &operator<<(std::ostream &out, SCCGraphIO &s)
{
  out << s.vds_ref() << "\n" << s.eds_ref();

  return out;
}

// Converter --------------------------------------------------------------------------------------

SCCConverter::SCCConverter() : dg_() {}
SCCConverter::SCCConverter(DSBGraph dg) : dg_(atomize(dg)) {}

member_imp(SCCConverter, DSBGraph, dg);

VertexDef SCCConverter::convert_vertex(DSetVertexDesc vd)
{
  SetVertex v = dg_ref()[vd];
  Ints subs_res(v.range_ref().ndim(), 0);

  BOOST_FOREACH (MultiInterval mi, v.range_ref().asets()) {
    int j = 0;
    BOOST_FOREACH (Interval i, mi.inters()) {
      subs_res[j] = subs_res[j] + i.card();

      j++;
    }
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


EdgeDefs SCCConverter::convert_edge(DSetEdgeDesc ed)
{
  EdgeDefs eds_res;

  DSetEdge e = dg_ref()[ed];
  DSetVertexDesc es = source(ed, dg_ref()), et = target(ed, dg_ref());

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

SCCGraphIO SCCConverter::convert() 
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

  return SCCGraphIO(vds_res, eds_res);
}

}  // namespace IO

} // namespace SBG
