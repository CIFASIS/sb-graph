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
SETV_TEMP_TYPE::SetVertexImp() : name_(""), id_(-1), index_(0), range_(), desc_() {}

SET_VERTEX_TEMPLATE
SETV_TEMP_TYPE::SetVertexImp(std::string name, Set range) : name_(name), id_(-1), range_(range), index_(0), desc_() {}

SET_VERTEX_TEMPLATE
SETV_TEMP_TYPE::SetVertexImp(std::string name, int id, Set range, int index) : name_(name), id_(id), range_(range), index_(index), desc_() {}

SET_VERTEX_TEMPLATE
SETV_TEMP_TYPE::SetVertexImp(std::string name, int id, Set range, int index, DESC desc)
    : name_(name), id_(id), range_(range), index_(index), desc_(desc) {}

member_imp_temp(SET_VERTEX_TEMPLATE, SETV_TEMP_TYPE, std::string, name);
member_imp_temp(SET_VERTEX_TEMPLATE, SETV_TEMP_TYPE, int, id);
member_imp_temp(SET_VERTEX_TEMPLATE, SETV_TEMP_TYPE, Set, range);
member_imp_temp(SET_VERTEX_TEMPLATE, SETV_TEMP_TYPE, int, index);
member_imp_temp(SET_VERTEX_TEMPLATE, SETV_TEMP_TYPE, DESC, desc);

SET_VERTEX_TEMPLATE
bool SETV_TEMP_TYPE::operator==(const SETV_TEMP_TYPE &other) const { return id() == other.id(); }

template struct SetVertexImp<SVDesc>;

SET_VERTEX_TEMPLATE
std::ostream &operator<<(std::ostream &out, const SETV_TEMP_TYPE &v)
{
  out << v.name() << ": " << v.range();

  return out;
}

template std::ostream &operator<<(std::ostream &out, const SetVertex &v);

// Set-edge ----------------------------------------------------------------------------------------

SET_EDGE_TEMPLATE
SETE_TEMP_TYPE::SetEdgeImp() : name_(""), id_(-1), map_f_(), map_u_(), index_(0), desc_() {}

SET_EDGE_TEMPLATE
SETE_TEMP_TYPE::SetEdgeImp(std::string name, PWLMap map_f, PWLMap map_u)
    : name_(name), id_(-1), map_f_(map_f), map_u_(map_u), index_(0), desc_() {}

SET_EDGE_TEMPLATE
SETE_TEMP_TYPE::SetEdgeImp(std::string name, PWLMap map_f, PWLMap map_u, DESC desc)
    : name_(name), id_(-1), map_f_(map_f), map_u_(map_u), index_(0), desc_(desc) {}

SET_EDGE_TEMPLATE
SETE_TEMP_TYPE::SetEdgeImp(std::string name, int id, PWLMap map_f, PWLMap map_u, int index)
    : name_(name), id_(id), map_f_(map_f), map_u_(map_u), index_(index), desc_() {}

SET_EDGE_TEMPLATE
SETE_TEMP_TYPE::SetEdgeImp(std::string name, int id, PWLMap map_f, PWLMap map_u, int index, DESC desc)
    : name_(name), id_(id), map_f_(map_f), map_u_(map_u), index_(index), desc_(desc) {}

member_imp_temp(SET_EDGE_TEMPLATE, SETE_TEMP_TYPE, std::string, name);
member_imp_temp(SET_EDGE_TEMPLATE, SETE_TEMP_TYPE, int, id);
member_imp_temp(SET_EDGE_TEMPLATE, SETE_TEMP_TYPE, PWLMap, map_f);
member_imp_temp(SET_EDGE_TEMPLATE, SETE_TEMP_TYPE, PWLMap, map_u);
member_imp_temp(SET_EDGE_TEMPLATE, SETE_TEMP_TYPE, int, index);
member_imp_temp(SET_EDGE_TEMPLATE, SETE_TEMP_TYPE, DESC, desc);

SET_EDGE_TEMPLATE
SETE_TEMP_TYPE SETE_TEMP_TYPE::restrictEdge(Set dom)
{
  PWLMap resf = map_f_ref().restrictMap(dom);
  PWLMap resu = map_u_ref().restrictMap(dom);

  return SetEdgeImp(name(), id(), resf, resu, index(), desc());
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

  std::string Enm = E.name();
  OrdCT<Set> dom = auxE.map_f_ref().dom();
  OrdCT<Set>::iterator itdom = dom.begin();

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

  return out;
}

template std::ostream &operator<<(std::ostream &out, const SetEdge &e);

// Directed Set-edge -------------------------------------------------------------------------------

DSET_EDGE_TEMPLATE
DSETE_TEMP_TYPE::DSetEdgeImp() : name_(""), id_(-1), map_b_(), map_d_(), index_(0), desc_() {}

DSET_EDGE_TEMPLATE
DSETE_TEMP_TYPE::DSetEdgeImp(std::string name, PWLMap map_b, PWLMap map_d)
    : name_(name), id_(-1), map_d_(map_d), map_b_(map_b), index_(0), desc_() {}

DSET_EDGE_TEMPLATE
DSETE_TEMP_TYPE::DSetEdgeImp(std::string name, PWLMap map_b, PWLMap map_d, DESC desc)
    : name_(name), id_(-1), map_d_(map_d), map_b_(map_b), index_(0), desc_(desc) {}

DSET_EDGE_TEMPLATE
DSETE_TEMP_TYPE::DSetEdgeImp(std::string name, int id, PWLMap map_b, PWLMap map_d, int index)
    : name_(name), id_(id), map_d_(map_d), map_b_(map_b), index_(index), desc_() {}

DSET_EDGE_TEMPLATE
DSETE_TEMP_TYPE::DSetEdgeImp(std::string name, int id, PWLMap map_b, PWLMap map_d, int index, DESC desc)
    : name_(name), id_(id), map_d_(map_d), map_b_(map_b), index_(index), desc_(desc) {}

member_imp_temp(DSET_EDGE_TEMPLATE, DSETE_TEMP_TYPE, std::string, name);
member_imp_temp(DSET_EDGE_TEMPLATE, DSETE_TEMP_TYPE, int, id);
member_imp_temp(DSET_EDGE_TEMPLATE, DSETE_TEMP_TYPE, PWLMap, map_b);
member_imp_temp(DSET_EDGE_TEMPLATE, DSETE_TEMP_TYPE, PWLMap, map_d);
member_imp_temp(DSET_EDGE_TEMPLATE, DSETE_TEMP_TYPE, int, index);
member_imp_temp(DSET_EDGE_TEMPLATE, DSETE_TEMP_TYPE, DESC, desc);

DSET_EDGE_TEMPLATE
DSETE_TEMP_TYPE DSETE_TEMP_TYPE::restrictEdge(Set dom)
{
  PWLMap resb = map_b_ref().restrictMap(dom);
  PWLMap resd = map_d_ref().restrictMap(dom);

  return DSetEdgeImp(name(), id(), resb, resd, index(), desc());
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

  std::string Enm = E.name();
  OrdCT<Set> dom = auxE.map_d_ref().dom();
  OrdCT<Set>::iterator itdom = dom.begin();

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

}  // namespace SBG
