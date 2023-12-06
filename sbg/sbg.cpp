/*******************************************************************************

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

#include "sbg/sbg.hpp"

namespace SBG {

namespace LIB {

// SBG -------------------------------------------------------------------------

template<typename Set>
SBGraph<Set>::SBGraph() : V_(), Vmap_(), E_(), map1_(), map2_(), Emap_() {}
template<typename Set>
SBGraph<Set>::SBGraph(Set V, PW Vmap, PW map1, PW map2, PW Emap)
  : V_(V), Vmap_(Vmap), E_(map1.dom().intersection(map2.dom()))
    , map1_(map1), map2_(map2), Emap_(Emap) {}

member_imp_temp(template<typename Set>, SBGraph<Set>, Set, V);
member_imp_temp(template<typename Set>, SBGraph<Set>, PWMap<Set>, Vmap);
member_imp_temp(template<typename Set>, SBGraph<Set>, Set, E);
member_imp_temp(template<typename Set>, SBGraph<Set>, PWMap<Set>, map1);
member_imp_temp(template<typename Set>, SBGraph<Set>, PWMap<Set>, map2);
member_imp_temp(template<typename Set>, SBGraph<Set>, PWMap<Set>, Emap);

template<typename Set>
std::ostream &operator<<(std::ostream &out, const SBGraph<Set> &g)
{
  out << "V = " << g.V() << ";\n";
  out << "Vmap = " << g.Vmap() << ";\n\n";
  out << "E = " << g.E() << "\n";
  out << "map1 = " << g.map1() << "\n";
  out << "map2 = " << g.map2() << "\n";
  out << "Emap = " << g.Emap() << "\n";

  return out;
}

template<typename Set>
SBGraph<Set> SBGraph<Set>::addSV(const Set &vertices) const
{
  if (!vertices.isEmpty() && vertices.intersection(V_).isEmpty()) {
    SBGraph res = *this;

    res.set_V(V_.cup(vertices));

    Set SV = Vmap_.image(); // Identifiers of SV
    std::size_t dims = vertices.begin()->size();
    Util::MD_NAT max = SV.isEmpty() ? Util::MD_NAT(dims, 0) : SV.maxElem();
    for (unsigned int j = 0; j < dims; ++j)
      max[j] = max[j] + 1;
    PW new_Vmap(SBGMap(vertices, max));
    res.set_Vmap(Vmap_.concatenation(new_Vmap));

    return res;
  }

  else
    Util::ERROR("LIB::SBG::addSV: vertices should be non-empty and disjoint from current vertices");

  return SBGraph();
}

template<typename Set>
unsigned int SBGraph<Set>::nmbrSV() const { return Vmap_.image().size(); }

template<typename Set>
SBGraph<Set> SBGraph<Set>::addSE(const PW &pw1, const PW &pw2) const
{
  Set edges, edges1 = pw1.dom(), edges2 = pw2.dom();
  if (edges1 == edges2) {
    edges = edges1.intersection(edges2);
    if (!edges.isEmpty() && edges.intersection(E_).isEmpty()) {
      SBGraph res = *this;

      res.set_E(E_.cup(edges));

      Set SE = Emap_.image(); // Identifiers of SV
      std::size_t dims = edges.begin()->size();
      Util::MD_NAT max = SE.isEmpty() ? Util::MD_NAT(dims, 0) : SE.maxElem();
      for (unsigned int j = 0; j < dims; ++j)
        max[j] = max[j] + 1;
      PW new_Emap(SBGMap(edges, max));
      res.set_Emap(Emap_.concatenation(new_Emap));

      res.set_map1(map1_.concatenation(pw1));
      res.set_map2(map2_.concatenation(pw2));

      return res;
    }

    else Util::ERROR("LIB::SBG::addSE: edges should be non-empty and disjoint from current vertices");
  }

  else Util::ERROR("LIB::SBG::addSE: maps domains should coincide");

  return SBGraph<Set>();
}

template<typename Set>
SBGraph<Set> SBGraph<Set>::copy(unsigned int times) const
{
  Set V_ith = V_, V_new = V_ith;
  PW Vmap_ith = Vmap_, Vmap_new = Vmap_ith;
  PW map1_ith = map1_, map1_new = map1_ith;
  PW map2_ith = map2_, map2_new = map2_ith;
  PW Emap_ith = Emap_, Emap_new = Emap_ith;

  Util::MD_NAT maxv = V_ith.maxElem();
  Util::MD_NAT maxV = Vmap_ith.image().maxElem();
  Util::MD_NAT maxe = E_.maxElem();
  Util::MD_NAT maxE = Emap_ith.image().maxElem();

  Exp off;
  for (unsigned int j = 0; j < maxv.size(); ++j) {
    Util::RATIONAL o = Util::RATIONAL(maxv[j]) - Util::RATIONAL(maxe[j]);
    off.emplaceBack(LExp(0, o));
  }

  for (unsigned int j = 0; j < times; ++j) {
    if (j > 0) {
      V_new = V_new.concatenation(V_ith);
      Vmap_new = Vmap_new.concatenation(Vmap_ith);
      map1_new = map1_new.concatenation(map1_ith);
      map2_new = map2_new.concatenation(map2_ith);
      Emap_new = Emap_new.concatenation(Emap_ith);
    }

    V_ith = V_ith.offset(maxv);
    Vmap_ith = Vmap_ith.offsetDom(maxv);
    Vmap_ith = Vmap_ith.offsetImage(maxV);

    map1_ith = map1_ith.offsetDom(maxe);
    map1_ith = map1_ith.offsetImage(off);
    map2_ith = map2_ith.offsetDom(maxe);
    map2_ith = map2_ith.offsetImage(off);
    Emap_ith = Emap_ith.offsetDom(maxe);
    Emap_ith = Emap_ith.offsetImage(maxE);
  }

  SBGraph<Set> res(V_new, Vmap_new, map1_new, map2_new, Emap_new);
  return SBGraph<Set>(V_new, Vmap_new, map1_new, map2_new, Emap_new);
}

// Directed SBG ----------------------------------------------------------------

template<typename Set>
DSBGraph<Set>::DSBGraph() : V_(), Vmap_(), E_(), mapB_(), mapD_(), Emap_() {}
template<typename Set>
DSBGraph<Set>::DSBGraph(Set V, PW Vmap, PW mapB, PW mapD, PW Emap)
  : V_(V), Vmap_(Vmap), E_(mapB.dom().intersection(mapD.dom()))
    , mapB_(mapB), mapD_(mapD), Emap_(Emap) {}

member_imp_temp(template<typename Set>, DSBGraph<Set>, Set, V);
member_imp_temp(template<typename Set>, DSBGraph<Set>, PWMap<Set>, Vmap);
member_imp_temp(template<typename Set>, DSBGraph<Set>, Set, E);
member_imp_temp(template<typename Set>, DSBGraph<Set>, PWMap<Set>, mapB);
member_imp_temp(template<typename Set>, DSBGraph<Set>, PWMap<Set>, mapD);
member_imp_temp(template<typename Set>, DSBGraph<Set>, PWMap<Set>, Emap);

template<typename Set>
std::ostream &operator<<(std::ostream &out, const DSBGraph<Set> &g)
{
  out << "V = " << g.V() << ";\n";
  out << "Vmap = " << g.Vmap() << ";\n\n";
  out << "E = " << g.E() << "\n";
  out << "mapB = " << g.mapB() << "\n";
  out << "mapD = " << g.mapD() << "\n";
  out << "Emap = " << g.Emap() << "\n";

  return out;
}

template<typename Set>
DSBGraph<Set> DSBGraph<Set>::addSV(const Set &vertices) const
{
  if (!vertices.isEmpty() && vertices.intersection(V_).isEmpty()) {
    DSBGraph res = *this;

    res.set_V(V_.cup(vertices));

    Set SV = Vmap_.image(); // Identifiers of SV
    std::size_t dims = vertices.begin()->size();
    Util::MD_NAT max = SV.isEmpty() ? Util::MD_NAT(dims, 0) : SV.maxElem();
    for (unsigned int j = 0; j < dims; ++j)
      max[j] = max[j] + 1;
    PW new_Vmap(SBGMap<Set>(vertices, max));
    res.set_Vmap(Vmap_.concatenation(new_Vmap));

    return res;
  }

  else Util::ERROR("LIB::SBG::addSV: vertices should be non-empty and disjoint from current vertices");

  return DSBGraph<Set>();
}

template<typename Set>
unsigned int DSBGraph<Set>::nmbrSV() const { return Vmap_.image().size(); }

template<typename Set>
DSBGraph<Set> DSBGraph<Set>::addSE(const PW &pw1, const PW &pw2) const
{
  Set edges, edges1 = pw1.dom(), edges2 = pw2.dom();
  if (edges1 == edges2) {
    edges = edges1.intersection(edges2);
    if (!edges.isEmpty() && edges.intersection(E_).isEmpty()) {
      DSBGraph res;

      res.set_E(E_.cup(edges));

      Set SE = Emap_.image(); // Identifiers of SV
      std::size_t dims = edges.begin()->size();
      Util::MD_NAT max = SE.isEmpty() ? Util::MD_NAT(dims, 0) : SE.maxElem();
      for (unsigned int j = 0; j < dims; ++j)
        max[j] = max[j] + 1;
      PW new_Emap(SBGMap<Set>(edges, max));
      res.set_Emap(Emap_.concatenation(new_Emap));

      res.set_mapB(mapB_.concatenation(pw1));
      res.set_mapD(mapD_.concatenation(pw2));
   
      return res;
    }

    else Util::ERROR("LIB::SBG::addSE: edges should be non-empty and disjoint from current vertices");
  }

  else Util::ERROR("LIB::SBG::addSE: maps domains should coincide");

  return DSBGraph<Set>();
}

// Template instantiations -----------------------------------------------------
template struct SBGraph<UnordSet>;
template std::ostream &operator<<(std::ostream &out, const BaseSBG &g);
template struct SBGraph<OrdSet>;
template std::ostream &operator<<(std::ostream &out, const CanonSBG &g);
template struct DSBGraph<UnordSet>;
template std::ostream &operator<<(std::ostream &out, const BaseDSBG &g);
template struct DSBGraph<OrdSet>;
template std::ostream &operator<<(std::ostream &out, const CanonDSBG &g);

} // namespace LIB

} // namespace SBG
