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
SBGraph<Set>::SBGraph(Set V, PWMap<Set> Vmap, PWMap<Set> map1, PWMap<Set> map2, PWMap<Set> Emap)
  : V_(V), Vmap_(Vmap), E_(intersection(dom(map1), dom(map2))), map1_(map1), map2_(map2), Emap_(Emap) {}

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
SBGraph<Set> addSV(Set vertices, SBGraph<Set> g)
{
  if (!isEmpty(vertices) && isEmpty(intersection(vertices, g.V()))) {
    SBGraph res = g;

    res.set_V(cup(g.V(), vertices));

    Set SV = image(g.Vmap()); // Identifiers of SV
    Util::NAT max = maxElem(SV)[0];
    std::size_t dims = maxElem(SV).size();
    MDLExp to_max(dims, LExp(0, Util::RATIONAL(max + 1)));
    PWMap<Set> new_Vmap(SBGMap<Set>(vertices, to_max));
    res.set_Vmap(concatenation(g.Vmap(), new_Vmap));

    return res;
  }

  else Util::ERROR("LIB::SBG::addSV: vertices should be non-empty and disjoint from current vertices");

  return SBGraph<Set>();
}

template<typename Set>
unsigned int nmbrSV(SBGraph<Set> g) { return image(g.Vmap()).size(); }

template<typename Set>
SBGraph<Set> addSE(PWMap<Set> pw1, PWMap<Set> pw2, SBGraph<Set> g)
{
  Set edges, edges1 = dom(pw1), edges2 = dom(pw2);
  if (edges1 == edges2) {
    edges = intersection(edges1, edges2);
    if (!isEmpty(edges) && isEmpty(intersection(edges, g.E()))) {
      SBGraph<Set> res = g;

      res.set_E(cup(g.E(), edges));

      Set SE = image(g.Emap()); // Identifiers of SV
      Util::NAT max = maxElem(SE)[0];
      std::size_t dims = maxElem(SE).size();
      MDLExp to_max(dims, LExp(0, Util::RATIONAL(max + 1)));
      PWMap<Set> new_Emap(SBGMap<Set>(edges, to_max));
      res.set_Emap(concatenation(g.Emap(), new_Emap));

      res.set_map1(concatenation(g.map1(), pw1));
      res.set_map2(concatenation(g.map2(), pw2));

      return res;
    }

    else Util::ERROR("LIB::SBG::addSE: edges should be non-empty and disjoint from current vertices");
  }

  else Util::ERROR("LIB::SBG::addSE: maps domains should coincide");

  return SBGraph<Set>();
}

template<typename Set>
SBGraph<Set> copy(unsigned int times, SBGraph<Set> g)
{
  Set V_ith = g.V(), V_new = V_ith;
  PWMap<Set> Vmap_ith = g.Vmap(), Vmap_new = Vmap_ith;
  PWMap<Set> map1_ith = g.map1(), map1_new = map1_ith;
  PWMap<Set> map2_ith = g.map2(), map2_new = map2_ith;
  PWMap<Set> Emap_ith = g.Emap(), Emap_new = Emap_ith;

  Util::MD_NAT maxv = maxElem(V_ith);
  Util::MD_NAT maxV = maxElem(image(Vmap_ith));
  Util::MD_NAT maxe = maxElem(g.E());
  Util::MD_NAT maxE = maxElem(image(Emap_ith));

  Exp off;
  parallel_foreach2 (maxv.value_ref(), maxe.value_ref()) {
    Util::NAT v = boost::get<0>(items), e = boost::get<1>(items);
    Util::RATIONAL o = Util::RATIONAL(v) - Util::RATIONAL(e);
    off.emplaceBack(LExp(0, o));
  }

  for (unsigned int j = 0; j < times; ++j) {
    if (j > 0) {
      V_new = concatenation(V_new, V_ith);
      Vmap_new = concatenation(Vmap_new, Vmap_ith);
      map1_new = concatenation(map1_new, map1_ith);
      map2_new = concatenation(map2_new, map2_ith);
      Emap_new = concatenation(Emap_new, Emap_ith);
    }

    V_ith = offset(maxv, V_ith);
    Vmap_ith = offsetDom(maxv, Vmap_ith);
    Vmap_ith = offsetImage(maxV, Vmap_ith);

    map1_ith = offsetDom(maxe, map1_ith);
    map1_ith = offsetImage(off, map1_ith);
    map2_ith = offsetDom(maxe, map2_ith);
    map2_ith = offsetImage(off, map2_ith);
    Emap_ith = offsetDom(maxv, Emap_ith);
    Emap_ith = offsetImage(maxE, Emap_ith);
  }

  return SBGraph<Set>(V_new, Vmap_new, map1_new, map2_new, Emap_new);
}

// Directed SBG ----------------------------------------------------------------

template<typename Set>
DSBGraph<Set>::DSBGraph() : V_(), Vmap_(), E_(), mapB_(), mapD_(), Emap_() {}
template<typename Set>
DSBGraph<Set>::DSBGraph(Set V, PWMap<Set> Vmap, PWMap<Set> mapB, PWMap<Set> mapD, PWMap<Set> Emap)
  : V_(V), Vmap_(Vmap), E_(intersection(dom(mapB), dom(mapD))), mapB_(mapB), mapD_(mapD), Emap_(Emap) {}

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
DSBGraph<Set> addSV(Set vertices, DSBGraph<Set> g)
{
  if (!isEmpty(vertices) && isEmpty(intersection(vertices, g.V()))) {
    DSBGraph res = g;

    res.set_V(cup(g.V(), vertices));

    Set SV = image(g.Vmap()); // Identifiers of SV
    Util::NAT max = maxElem(SV)[0];
    std::size_t dims = maxElem(SV).size();
    MDLExp to_max(dims, LExp(0, Util::RATIONAL(max + 1)));
    PWMap<Set> new_Vmap(SBGMap<Set>(vertices, to_max));
    res.set_Vmap(concatenation(g.Vmap(), new_Vmap));

    return res;
  }

  else Util::ERROR("LIB::SBG::addSV: vertices should be non-empty and disjoint from current vertices");

  return DSBGraph<Set>();
}

template<typename Set>
unsigned int nmbrSV(DSBGraph<Set> g) { return image(g.Vmap()).size(); }

template<typename Set>
DSBGraph<Set> addSE(PWMap<Set> pw1, PWMap<Set> pw2, DSBGraph<Set> g)
{
  Set edges, edges1 = dom(pw1), edges2 = dom(pw2);
  if (edges1 == edges2) {
    edges = intersection(edges1, edges2);
    if (!isEmpty(edges) && isEmpty(intersection(edges, g.E()))) {
      DSBGraph<Set> res;

      g.set_E(cup(g.E(), edges));

      Set SE = image(g.Emap()); // Identifiers of SV
      Util::NAT max = maxElem(SE)[0];
      std::size_t dims = maxElem(SE).size();
      MDLExp to_max(dims, LExp(0, Util::RATIONAL(max + 1)));
      PWMap<Set> new_Emap(SBGMap<Set>(edges, to_max));
      g.set_Emap(concatenation(g.Emap(), new_Emap));

      g.set_mapB(concatenation(g.mapB(), pw1));
      g.set_mapD(concatenation(g.mapD(), pw2));
   
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
template BaseSBG addSV<UnordSet>(UnordSet vertices, BaseSBG g);
template unsigned int nmbrSV<UnordSet>(BaseSBG g);
template BaseSBG addSE<UnordSet>(BasePWMap pw1, BasePWMap pw2, BaseSBG g);
template BaseSBG copy<UnordSet>(unsigned int times, BaseSBG g);

template struct SBGraph<OrdSet>;
template std::ostream &operator<<(std::ostream &out, const CanonSBG &g);
template CanonSBG addSV<OrdSet>(OrdSet vertices, CanonSBG g);
template unsigned int nmbrSV<OrdSet>(CanonSBG g);
template CanonSBG addSE<OrdSet>(CanonPWMap pw1, CanonPWMap pw2, CanonSBG g);
template CanonSBG copy<OrdSet>(unsigned int times, CanonSBG g);

template struct DSBGraph<UnordSet>;
template std::ostream &operator<<(std::ostream &out, const BaseDSBG &g);
template BaseDSBG addSV<UnordSet>(UnordSet vertices, BaseDSBG g);
template unsigned int nmbrSV<UnordSet>(BaseDSBG g);
template BaseDSBG addSE<UnordSet>(BasePWMap pw1, BasePWMap pw2, BaseDSBG g);

template struct DSBGraph<OrdSet>;
template std::ostream &operator<<(std::ostream &out, const CanonDSBG &g);
template CanonDSBG addSV<OrdSet>(OrdSet vertices, CanonDSBG g);
template unsigned int nmbrSV<OrdSet>(CanonDSBG g);
template CanonDSBG addSE<OrdSet>(CanonPWMap pw1, CanonPWMap pw2, CanonDSBG g);

} // namespace LIB

} // namespace SBG
