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
#include <sbg/sbg_io.hpp>

namespace SBG {

namespace IO {

// Pretty-printer ---------------------------------------------------------------------------------

CompactInterval::CompactInterval() : lo_(), hi_() {}
CompactInterval::CompactInterval(int lo, int hi) : lo_(lo), hi_(hi) {}

member_imp(CompactInterval, int, lo);
member_imp(CompactInterval, int, hi);

std::ostream &operator<<(std::ostream &out, CompactInterval &ci)
{
   out << "[" << ci.lo();

  if (ci.lo() != ci.hi())
    out << ":" << ci.hi();

  out << "]";

  return out;
}

std::ostream &operator<<(std::ostream &out, CompactIntervals &cis)
{
  out << "[";
  int sz = cis.size(), i = 0;
  for (; i < sz - 1; i++) {
    out << cis[i].lo();

    if (cis[i].lo() != cis[i].hi())
      out << ":" << cis[i].hi(); 

    out << ", ";
  }
  out << cis[i].lo();

  if (cis[i].lo() != cis[i].hi())
    out << ":" << cis[i].hi(); 

  out << "]";

  return out;
}

VertexDef::VertexDef() : name_(), subs_dims_() {}
VertexDef::VertexDef(std::string name, CompactIntervals subs_dims) : name_(name), subs_dims_(subs_dims) {};

member_imp(VertexDef, std::string, name);
member_imp(VertexDef, CompactIntervals, subs_dims);

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

GraphIO::GraphIO() : vds_(), eds_() {}
GraphIO::GraphIO(VertexDefs vds, EdgeDefs eds) : vds_(vds), eds_(eds) {}

member_imp(GraphIO, VertexDefs, vds);
member_imp(GraphIO, EdgeDefs, eds);

std::ostream &operator<<(std::ostream &out, GraphIO &s)
{
  out << s.vds_ref() << "\n" << s.eds_ref();

  return out;
}

std::ostream &operator<<(std::ostream &out, VertexUsages &vrtcs)
{
  out << "{";
  int sz = vrtcs.size(), i = 0;
  for (; i < sz - 1; i++) 
    out << vrtcs[i] << ", ";
  out << vrtcs[i];
  out << "}";

  return out;
}

ComponentIO::ComponentIO() : is_(), rs_(), vrtcs_() {}
ComponentIO::ComponentIO(Iterators is, Ranges rs, VertexUsages vrtcs) : is_(is), rs_(rs), vrtcs_(vrtcs) {}

member_imp(ComponentIO, Iterators, is);
member_imp(ComponentIO, Ranges, rs); 
member_imp(ComponentIO, VertexUsages, vrtcs);

std::ostream &operator<<(std::ostream &out, ComponentIO &c)
{
  VertexUsages aux = c.vrtcs();

  int sz = c.is_ref().size(), i = 0;
  out << "for ";
  parallel_foreach2 (c.is_ref(), c.rs_ref()) {
    if (i < sz - 1)
      out << boost::get<0>(items) << " in " << boost::get<1>(items) << ", ";

    else
      out << boost::get<0>(items) << " in " << boost::get<1>(items);

    i++;
  }
  out << "\n  " << aux << "\nendfor";

  return out;
}

std::ostream &operator<<(std::ostream &out, RMapIO &rmap)
{
  BOOST_FOREACH (ComponentIO c, rmap)
    out << c << "\n";

  return out;
}

} // namespace IO

} // namespace SBG
