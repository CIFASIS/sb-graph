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
void addSV(Set vertices, SBGraph<Set> &g)
{
  if (!isEmpty(vertices) && isEmpty(intersection(vertices, g.V()))) {
    g.set_V(cup(g.V(), vertices));

    Set SV = image(g.Vmap()); // Identifiers of SV
    Util::NAT max = maxElem(SV)[0];
    std::size_t dims = maxElem(SV).size();
    MDLExp to_max(dims, LExp(0, Util::RATIONAL(max + 1)));
    PWMap<Set> new_Vmap(SBGMap<Set>(vertices, to_max));
    g.set_Vmap(concatenation(g.Vmap(), new_Vmap));
  }

  else Util::ERROR("LIB::SBG::addSV: vertices should be non-empty and disjoint from current vertices");

  return;
}

template<typename Set>
void addSE(PWMap<Set> pw1, PWMap<Set> pw2, SBGraph<Set> &g)
{
  Set edges, edges1 = dom(pw1), edges2 = dom(pw2);
  if (edges1 == edges2) {
    edges = intersection(edges1, edges2);
    if (!isEmpty(edges) && isEmpty(intersection(edges, g.E()))) {
      g.set_E(cup(g.E(), edges));

      Set SE = image(g.Emap()); // Identifiers of SV
      Util::NAT max = maxElem(SE)[0];
      std::size_t dims = maxElem(SE).size();
      MDLExp to_max(dims, LExp(0, Util::RATIONAL(max + 1)));
      PWMap<Set> new_Emap(SBGMap<Set>(edges, to_max));
      g.set_Emap(concatenation(g.Emap(), new_Emap));

      g.set_map1(concatenation(g.map1(), pw1));
      g.set_map2(concatenation(g.map2(), pw2));
    }

    else Util::ERROR("LIB::SBG::addSE: edges should be non-empty and disjoint from current vertices");
  }

  else Util::ERROR("LIB::SBG::addSE: maps domains should coincide");

  return;
}

// Functions -------------------------------------------------------------------

template<typename Set>
PWMap<Set> connectedComponents(SBGraph<Set> g)
{
  if (!isEmpty(g.V())) {
    unsigned int dims = g.V()[0].size();
    SBGMap<Set> id(g.V(), Exp(dims));
    PWMap<Set> rmap(id), old_rmap;

    if (isEmpty(g.E())) return rmap;

    do {
      old_rmap = rmap;

      PWMap<Set> ermap1 = composition(rmap, g.map1());
      PWMap<Set> ermap2 = composition(rmap, g.map2());

      PWMap<Set> rmap1 = minAdjMap(ermap1, ermap2);
      PWMap<Set> rmap2 = minAdjMap(ermap2, ermap1);
      rmap1 = combine(rmap1, rmap);
      rmap2 = combine(rmap2, rmap);

      PWMap aux_rmap = minMap(rmap1, rmap2);
      rmap = minMap(rmap, aux_rmap);

      if (!(rmap == old_rmap)) {
        rmap = aux_rmap;
        rmap = mapInf(rmap);
      }
    } while (rmap != old_rmap); 

    return rmap;
  }

  return PWMap<Set>();
}

// Template instantiations -----------------------------------------------------

template struct SBGraph<UnordSet>;
template std::ostream &operator<<(std::ostream &out, const BaseSBG &g);
template void addSV<UnordSet>(UnordSet vertices, BaseSBG &g);
template void addSE<UnordSet>(BasePWMap pw1, BasePWMap pw2, BaseSBG &g);
template BasePWMap connectedComponents<UnordSet>(BaseSBG g);

template struct SBGraph<OrdSet>;
template std::ostream &operator<<(std::ostream &out, const CanonSBG &g);
template void addSV<OrdSet>(OrdSet vertices, CanonSBG &g);
template void addSE<OrdSet>(CanonPWMap pw1, CanonPWMap pw2, CanonSBG &g);
template CanonPWMap connectedComponents<OrdSet>(CanonSBG g);

} // namespace LIB

} // namespace SBG
