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

#include <sbg/atom_sbg.hpp>

namespace SBG {

// Set-vertex --------------------------------------------------------------------------------------

ATOM_SET_VERTEX_TEMPLATE
ATOM_SETV_TEMP_TYPE::AtomSetVertexImp() : name_(""), id_(-1), range_(), desc_() {}

ATOM_SET_VERTEX_TEMPLATE
ATOM_SETV_TEMP_TYPE::AtomSetVertexImp(std::string name, MultiInterval range) : name_(name), id_(-1), range_(range), desc_() {}

ATOM_SET_VERTEX_TEMPLATE
ATOM_SETV_TEMP_TYPE::AtomSetVertexImp(std::string name, int id, MultiInterval range) : name_(name), id_(id), range_(range), desc_() {}

ATOM_SET_VERTEX_TEMPLATE
ATOM_SETV_TEMP_TYPE::AtomSetVertexImp(std::string name, int id, MultiInterval range, DESC desc)
    : name_(name), id_(id), range_(range), desc_(desc) {}

member_imp_temp(ATOM_SET_VERTEX_TEMPLATE, ATOM_SETV_TEMP_TYPE, std::string, name);
member_imp_temp(ATOM_SET_VERTEX_TEMPLATE, ATOM_SETV_TEMP_TYPE, int, id);
member_imp_temp(ATOM_SET_VERTEX_TEMPLATE, ATOM_SETV_TEMP_TYPE, MultiInterval, range);
member_imp_temp(ATOM_SET_VERTEX_TEMPLATE, ATOM_SETV_TEMP_TYPE, DESC, desc);

ATOM_SET_VERTEX_TEMPLATE
bool ATOM_SETV_TEMP_TYPE::operator==(const ATOM_SETV_TEMP_TYPE &other) const { return id() == other.id(); }

template struct AtomSetVertexImp<SVDesc>;

ATOM_SET_VERTEX_TEMPLATE
std::ostream &operator<<(std::ostream &out, const ATOM_SETV_TEMP_TYPE &v)
{
  out << v.name() << ": " << v.range();

  return out;
}

template std::ostream &operator<<(std::ostream &out, const AtomSetVertex &v);

// Graphs ------------------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream &out, AtomSBGraph &g)
{
  BOOST_FOREACH (AtomSVDesc vd, vertices(g))
    out << g[vd] << "\n";

  out << "\n";
  BOOST_FOREACH (AtomSEDesc ed, edges(g))
    out << g[ed] << "\n";

  return out;
}

std::ostream &operator<<(std::ostream &out, AtomDSBGraph &g)
{
  BOOST_FOREACH (AtomDSVDesc vd, vertices(g))
    out << g[vd] << "\n";

  out << "\n";
  BOOST_FOREACH (AtomDSEDesc ed, edges(g))
    out << g[ed] << "\n";

  return out;
}

// -------------------------------------------------------------------------------------------------

AtomDSBGraph atomize(DSBGraph dg)
{
  AtomDSBGraph dg_res;

  // Create vertices
  BOOST_FOREACH (DSetVertexDesc vd, vertices(dg)) {
    SetVertex v = dg[vd];

    int j = 1;
    BOOST_FOREACH (MultiInterval mi, v.range_ref().asets()) {
      std::string nm = v.name();
      int sz = v.range_ref().asets_ref().size();
      if (sz > 1)
        nm = nm + "_" + std::to_string(j); 

      AtomSetVertex v_res(nm, mi); 

      AtomDSVDesc vd_res = boost::add_vertex(dg_res);
      dg_res[vd_res] = v_res;

      j++;
    } 
  }

  PWLMap mapB, mapD;
  BOOST_FOREACH (DSetEdgeDesc ed, edges(dg)) {
    PWLMap bmap = dg[ed].map_b(), dmap = dg[ed].map_d();

    mapB = mapB.concat(bmap);
    mapD = mapD.concat(dmap);
  }
  Set all_edges = mapB.wholeDom();

  // Create edges
  BOOST_FOREACH (DSetVertexDesc vd, vertices(dg_res)) {
    AtomSetVertex v = dg_res[vd];
    Set v_range = Set(v.range());

    Set v_b = mapB.preImage(v_range), v_d = mapD.preImage(v_range);
    BOOST_FOREACH (DSetEdgeDesc ed, edges(dg)) {
      Set e_dom = dg[ed].dom();

      Set b_dom = v_b.cap(e_dom), d_dom = v_d.cap(e_dom);
      if (!b_dom.empty()) {
        Set adj_b = mapD.image(b_dom);
        
        BOOST_FOREACH (DSetVertexDesc vd_adj, vertices(dg_res)) {
          AtomSetVertex adj_v = dg_res[vd_adj];
          Set adj_v_dom = Set(adj_v.range());

          if (!adj_v_dom.cap(adj_b).empty()) {
            // Add edge
            AtomDSEDesc ed_adj;
            bool b;
            boost::tie(ed_adj, b) = boost::add_edge(vd, vd_adj, dg_res);
            dg_res[ed_adj] = DSetEdge(v.name() + adj_v.name(), mapB.restrictMap(b_dom), mapD.restrictMap(b_dom));

            // Take out traversed edges
            Set remaining = all_edges.diff(b_dom);
            mapB = mapB.restrictMap(remaining);
            mapD = mapD.restrictMap(remaining);
          }
        }
      }

      if (!d_dom.empty()) {
        Set adj_d = mapB.image(d_dom);
        
        BOOST_FOREACH (DSetVertexDesc vd_adj, vertices(dg_res)) {
          AtomSetVertex adj_v = dg_res[vd_adj];
          Set adj_v_dom = Set(adj_v.range());

          if (!adj_v_dom.cap(adj_d).empty()) {
            // Add edge
            AtomDSEDesc ed_adj;
            bool b;
            boost::tie(ed_adj, b) = boost::add_edge(vd_adj, vd, dg_res);
            dg_res[ed_adj] = DSetEdge(adj_v.name() + v.name(), mapB.restrictMap(d_dom), mapD.restrictMap(d_dom));

            // Take out traversed edges
            Set remaining = all_edges.diff(d_dom);
            mapB = mapB.restrictMap(remaining);
            mapD = mapD.restrictMap(remaining);
          }
        }
      }
    }
  }

  return dg_res;
}

} // namespace SBG
