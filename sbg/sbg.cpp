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
#include <sbg/sbg.hpp>

namespace SBG {

// Set-vertex --------------------------------------------------------------------------------------

SET_VERTEX_TEMPLATE
SETV_TEMP_TYPE::SetVertexImp() : id_(), range_(), desc_() {}

SET_VERTEX_TEMPLATE
SETV_TEMP_TYPE::SetVertexImp(std::string id, Set range) : id_(id), range_(range), desc_() {}

SET_VERTEX_TEMPLATE
SETV_TEMP_TYPE::SetVertexImp(std::string id, Set range, DESC desc) : id_(id), range_(range), desc_(desc) {}

member_imp_temp(SET_VERTEX_TEMPLATE, SETV_TEMP_TYPE, std::string, id);
member_imp_temp(SET_VERTEX_TEMPLATE, SETV_TEMP_TYPE, Set, range);
member_imp_temp(SET_VERTEX_TEMPLATE, SETV_TEMP_TYPE, DESC, desc);

SET_VERTEX_TEMPLATE
SETV_TEMP_TYPE SETV_TEMP_TYPE::restrictVertex(Set dom)
{
  return SetVertexImp(id(), range().cap(dom), desc());
}

SET_VERTEX_TEMPLATE
bool SETV_TEMP_TYPE::operator==(const SETV_TEMP_TYPE &other) const { return id() == other.id(); }

template struct SetVertexImp<SVDesc>;

SET_VERTEX_TEMPLATE
std::ostream &operator<<(std::ostream &out, const SETV_TEMP_TYPE &v)
{
  out << v.id() << ": " << v.range();

  return out;
}

template std::ostream &operator<<(std::ostream &out, const SetVertex &v);

// Set-edge ----------------------------------------------------------------------------------------

SET_EDGE_TEMPLATE
SETE_TEMP_TYPE::SetEdgeImp() : id_(), map_f_(), map_u_(), desc_() {}

SET_EDGE_TEMPLATE
SETE_TEMP_TYPE::SetEdgeImp(std::string id, PWLMap map_f, PWLMap map_u)
    : id_(id), map_f_(map_f), map_u_(map_u), desc_() {}

SET_EDGE_TEMPLATE
SETE_TEMP_TYPE::SetEdgeImp(std::string id, PWLMap map_f, PWLMap map_u, DESC desc)
    : id_(id), map_f_(map_f), map_u_(map_u), desc_(desc) {}

member_imp_temp(SET_EDGE_TEMPLATE, SETE_TEMP_TYPE, std::string, id);
member_imp_temp(SET_EDGE_TEMPLATE, SETE_TEMP_TYPE, PWLMap, map_f);
member_imp_temp(SET_EDGE_TEMPLATE, SETE_TEMP_TYPE, PWLMap, map_u);
member_imp_temp(SET_EDGE_TEMPLATE, SETE_TEMP_TYPE, DESC, desc);

SET_EDGE_TEMPLATE
SETE_TEMP_TYPE SETE_TEMP_TYPE::restrictEdge(Set dom)
{
  PWLMap resf = map_f_ref().restrictMap(dom);
  PWLMap resu = map_u_ref().restrictMap(dom);

  return SetEdgeImp(id(), resf, resu, desc());
}

SET_EDGE_TEMPLATE
Set SETE_TEMP_TYPE::dom() { return map_f_ref().wholeDom().cap(map_u_ref().wholeDom()); }

SET_EDGE_TEMPLATE
bool SETE_TEMP_TYPE::operator==(const SETE_TEMP_TYPE &other) const { return id() == other.id(); }

template struct SetEdgeImp<SEDesc>;

SET_EDGE_TEMPLATE
std::ostream &operator<<(std::ostream &out, const SETE_TEMP_TYPE &E)
{
  SetEdge auxE = E;

  std::string Enm = E.id();
  OrdCT<Set> dom = auxE.map_f_ref().dom();
  OrdCT<Set>::iterator itdom = dom.begin();

  if(dom.size() > 0) {
    out << Enm << " dom: ";
    out << "[";
    for (; next(itdom, 1) != dom.end(); ++itdom) out << *itdom << ", ";
    out << *itdom << "]\n";

    OrdCT<LMap> lmleft = auxE.map_f_ref().lmap();
    OrdCT<LMap>::iterator itleft = lmleft.begin();
    OrdCT<LMap> lmright = auxE.map_u_ref().lmap();
    OrdCT<LMap>::iterator itright = lmright.begin();

    out << Enm << " left | right: ";
    out << "[";
    for (; next(itleft, 1) != lmleft.end(); ++itleft) out << *itleft << ", ";
    out << *itleft << "] | ";

    out << "[";
    for (; next(itright, 1) != lmright.end(); ++itright) out << *itright << ", ";
    out << *itright << "]";
  }

  return out;
}

template std::ostream &operator<<(std::ostream &out, const SetEdge &e);

// Directed Set-edge -------------------------------------------------------------------------------

DSET_EDGE_TEMPLATE
DSETE_TEMP_TYPE::DSetEdgeImp() : id_(), map_b_(), map_d_(), desc_() {}

DSET_EDGE_TEMPLATE
DSETE_TEMP_TYPE::DSetEdgeImp(std::string id, PWLMap map_b, PWLMap map_d)
    : id_(id), map_d_(map_d), map_b_(map_b), desc_() {}

DSET_EDGE_TEMPLATE
DSETE_TEMP_TYPE::DSetEdgeImp(std::string id, PWLMap map_b, PWLMap map_d, DESC desc)
    : id_(id), map_d_(map_d), map_b_(map_b), desc_(desc) {}

member_imp_temp(DSET_EDGE_TEMPLATE, DSETE_TEMP_TYPE, std::string, id);
member_imp_temp(DSET_EDGE_TEMPLATE, DSETE_TEMP_TYPE, PWLMap, map_b);
member_imp_temp(DSET_EDGE_TEMPLATE, DSETE_TEMP_TYPE, PWLMap, map_d);
member_imp_temp(DSET_EDGE_TEMPLATE, DSETE_TEMP_TYPE, DESC, desc);

DSET_EDGE_TEMPLATE
DSETE_TEMP_TYPE DSETE_TEMP_TYPE::restrictEdge(Set dom)
{
  PWLMap resb = map_b_ref().restrictMap(dom);
  PWLMap resd = map_d_ref().restrictMap(dom);

  return DSetEdgeImp(id(), resb, resd, desc());
}

DSET_EDGE_TEMPLATE
Set DSETE_TEMP_TYPE::dom() { return map_b_ref().wholeDom().cap(map_d_ref().wholeDom()); }

DSET_EDGE_TEMPLATE
bool DSETE_TEMP_TYPE::operator==(const DSETE_TEMP_TYPE &other) const { return id() == other.id(); }

template struct DSetEdgeImp<SEDesc>;

DSET_EDGE_TEMPLATE
std::ostream &operator<<(std::ostream &out, const DSETE_TEMP_TYPE &E)
{
  DSetEdge auxE = E;

  std::string Enm = E.id();
  OrdCT<Set> dom = auxE.map_d_ref().dom();
  OrdCT<Set>::iterator itdom = dom.begin();

  if(dom.size() > 0) {
    out << Enm << " dom: ";
    out << "[";
    for (; next(itdom, 1) != dom.end(); ++itdom) out << *itdom << ", ";
    out << *itdom << "]\n";

    OrdCT<LMap> lmb = auxE.map_b_ref().lmap();
    OrdCT<LMap>::iterator itb = lmb.begin();
    OrdCT<LMap> lmd = auxE.map_d_ref().lmap();
    OrdCT<LMap>::iterator itd = lmd.begin();

    out << Enm << " left | right: ";
    out << "[";
    for (; next(itb, 1) != lmb.end(); ++itb) out << *itb << ", ";
    out << *itb << "] | ";

    out << "[";
    for (; next(itd, 1) != lmd.end(); ++itd) out << *itd << ", ";
    out << *itd << "]";
  }

  return out;
}

template std::ostream &operator<<(std::ostream &out, const DSetEdge &e);

// Graphs ------------------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream &out, SBGraph &g)
{
  BOOST_FOREACH (SetVertexDesc vd, vertices(g))
    out << g[vd] << "\n";

  out << "\n";
  BOOST_FOREACH (SetEdgeDesc ed, edges(g))
    out << g[ed] << "\n\n";

  return out;
}

std::ostream &operator<<(std::ostream &out, DSBGraph &g)
{
  BOOST_FOREACH (DSetVertexDesc vd, vertices(g))
    out << g[vd] << "\n";

  out << "\n";
  BOOST_FOREACH (DSetEdgeDesc ed, edges(g))
    out << g[ed] << "\n\n";

  return out;
}

std::string get_name(Set s, SBGraph g)
{
  std::string result;

  BOOST_FOREACH (SetVertexDesc vd, vertices(g)) {
    Set v_range = g[vd].range();
    if (!v_range.cap(s).empty())
      result = g[vd].id();
  }

  return result;
}

std::optional<SetVertexDesc> get_vertex(Set s, SBGraph &g)
{
  BOOST_FOREACH (SetVertexDesc vd, vertices(g)) {
    Set v_range = g[vd].range();
    if (!v_range.cap(s).empty()) 
      return vd;
  }

  return std::nullopt;
}

std::optional<SetEdgeDesc> get_edge(MultiInterval e, SBGraph &g)
{
  BOOST_FOREACH (SetEdgeDesc ed, edges(g)) {
    Set e_range = g[ed].dom();
    if (!e_range.cap(Set(e)).empty()) 
      return ed;
  }

  return std::nullopt;
}

std::string get_name(Set s, DSBGraph dg)
{
  std::string result;

  BOOST_FOREACH (DSetVertexDesc vd, vertices(dg)) {
    Set v_range = dg[vd].range();
    if (!v_range.cap(s).empty())
      result = dg[vd].id();
  }

  return result;
}

std::optional<DSetVertexDesc> get_vertex(Set s, DSBGraph &dg)
{
  BOOST_FOREACH (DSetVertexDesc vd, vertices(dg)) {
    Set v_range = dg[vd].range();
    if (!v_range.cap(s).empty()) 
      return vd;
  }

  return std::nullopt;
}

// Additionals structures --------------------------------------------------------------------------

std::ostream &operator<<(std::ostream &out, VertexOrder &vo)
{
  int sz = vo.size(), i = 0;
  if (sz > 0) {
    for (; i < sz - 1; i++)
      out << vo[i] << " - ";
    out << vo[i];
  }

  return out;
}

}  // namespace SBG
