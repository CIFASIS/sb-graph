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

////////////////////////////////////////////////////////////////////////////////
// SBG -------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

SBGraph::SBGraph(const PWMapAF &fact) 
  : fact_(fact), V_(fact_.createSet()), Vmap_(fact_.createPWMap())
  , E_(fact_.createSet()), map1_(fact_.createPWMap())
  , map2_(fact_.createPWMap()), Emap_(fact_.createPWMap())
  , subEmap_(fact_.createPWMap()) {}
SBGraph::SBGraph(const PWMapAF &fact, const Set &V, const PWMap &Vmap
  , const PWMap &map1, const PWMap &map2
  , const PWMap &Emap, const PWMap &subEmap)
  : fact_(fact), V_(V), Vmap_(Vmap), E_(map1.dom().intersection(map2.dom()))
    , map1_(map1), map2_(map2), Emap_(Emap), subEmap_(subEmap) {}

member_imp(SBGraph, Set, V);
member_imp(SBGraph, PWMap, Vmap);
member_imp(SBGraph, Set, E);
member_imp(SBGraph, PWMap, map1);
member_imp(SBGraph, PWMap, map2);
member_imp(SBGraph, PWMap, Emap);
member_imp(SBGraph, PWMap, subEmap);

std::ostream &operator<<(std::ostream &out, const SBGraph &g)
{
  out << "V: " << g.V() << "\n";
  out << "Vmap: " << g.Vmap() << "\n\n";
  out << "E: " << g.E() << "\n";
  out << "map1: " << g.map1() << "\n";
  out << "map2: " << g.map2() << "\n";
  out << "Emap: " << g.Emap() << "\n";
  out << "subEmap: " << g.subEmap() << "\n";

  return out;
}

SBGraph SBGraph::addSV(const Set &vertices) const
{
  if (!vertices.isEmpty() && vertices.intersection(V_).isEmpty()) {
    PWMap new_Vmap(std::move(Vmap_));
    PWMap new_map1(std::move(map1_)), new_map2(std::move(map2_));
    PWMap new_Emap(std::move(Emap_)), new_subE(std::move(subEmap_));

    Set new_V = new_V.cup(vertices);

    Set SV = new_Vmap.image(); // Identifiers of SV
    std::size_t dims = vertices.arity();
    MD_NAT max = SV.isEmpty() ? MD_NAT(dims, 0) : SV.maxElem();
    for (unsigned int j = 0; j < dims; ++j)
      max[j] = max[j] + 1;
    Map m = fact_.createMap(vertices, Exp(max));
    new_Vmap.emplaceBack(m);

    return SBGraph(fact_, new_V, new_Vmap, new_map1, new_map2
      , new_Emap, new_subE);
  }

  else if (!vertices.intersection(V_).isEmpty())
    Debug::ERROR("Trying to add existing vertices: ", vertices, " to SBG\n");

  return SBGraph(fact_);
}

SBGraph SBGraph::addSE(const PWMap &pw1, const PWMap &pw2) const
{
  Set edges = fact_.createSet(), edges1 = pw1.dom(), edges2 = pw2.dom();
  if (edges1 == edges2) {
    edges = edges1;
    if (!edges.isEmpty() && edges.intersection(E_).isEmpty()) {
      Set new_V(std::move(V_));
      PWMap new_Vmap(std::move(Vmap_));
      PWMap new_map1(std::move(map1_)), new_map2(std::move(map2_));
      PWMap new_Emap(std::move(Emap_)), new_subE(std::move(subEmap_));

      Set SE = Emap_.image(); // Identifiers of SV
      std::size_t dims = edges.arity();
      MD_NAT max = SE.isEmpty() ? MD_NAT(dims, 0) : SE.maxElem();
      for (unsigned int j = 0; j < dims; ++j)
        max[j] = max[j] + 1;
      Map m = fact_.createMap(edges, max);  
      new_Emap.emplaceBack(m);

      new_map1 = new_map1.concatenation(pw1);
      new_map2 = new_map2.concatenation(pw2);

      return SBGraph(fact_, new_V, new_Vmap, new_map1, new_map2
        , new_Emap, new_subE);
    }

    else if (!edges.intersection(E_).isEmpty())
      Debug::ERROR("Trying to add existing edges: ", edges, " to SBG\n");
  }

  return SBGraph(fact_);
}

SBGraph SBGraph::copy(unsigned int times) const
{
  Set ith_V = V_, new_V = ith_V;
  PWMap ith_Vmap = Vmap_, new_Vmap = ith_Vmap;
  PWMap ith_map1 = map1_, new_map1 = ith_map1;
  PWMap ith_map2 = map2_, new_map2 = ith_map2;
  PWMap ith_Emap = Emap_, new_Emap = ith_Emap;
  PWMap ith_subE = subEmap_, new_subE = ith_subE;

  if (!ith_V.isEmpty()) {
    MD_NAT maxv = ith_V.maxElem();
    auto dims = maxv.arity();
    MD_NAT maxV
      = ith_Vmap.isEmpty() ? MD_NAT(dims, 0) : ith_Vmap.image().maxElem();
    MD_NAT maxe = E_.isEmpty() ? MD_NAT(dims, 0) : E_.maxElem();
    MD_NAT maxE
      = ith_Emap.isEmpty() ? MD_NAT(dims, 0) : ith_Emap.image().maxElem();

    Exp off;
    for (unsigned int j = 0; j < dims; ++j) {
      RATIONAL o = RATIONAL(maxv[j]) - RATIONAL(maxe[j]);
      off.emplaceBack(LExp(0, o));
    }

    for (unsigned int j = 0; j < times; ++j) {
      if (j > 0) {
        new_V = new_V.disjointCup(ith_V);
        new_Vmap = new_Vmap.concatenation(ith_Vmap);
        new_map1 = new_map1.concatenation(ith_map1);
        new_map2 = new_map2.concatenation(ith_map2);
        new_Emap = new_Emap.concatenation(ith_Emap);
        new_subE = new_subE.concatenation(ith_subE);
      }

      ith_V = ith_V.offset(maxv);
      ith_Vmap = ith_Vmap.offsetDom(maxv);
      ith_Vmap = ith_Vmap.offsetImage(maxV);

      ith_map1 = ith_map1.offsetDom(maxe);
      ith_map1 = ith_map1.offsetImage(off);
      ith_map2 = ith_map2.offsetDom(maxe);
      ith_map2 = ith_map2.offsetImage(off);
      ith_Emap = ith_Emap.offsetDom(maxe);
      ith_Emap = ith_Emap.offsetImage(maxE);
      ith_subE = ith_subE.offsetDom(maxe);
      ith_subE = ith_subE.offsetImage(maxE);
    }
  }

  SBGraph res(fact_, new_V, new_Vmap, new_map1, new_map2, new_Emap, new_subE);
  return res;
}

////////////////////////////////////////////////////////////////////////////////
// Directed SBG ----------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

DSBGraph::DSBGraph(const PWMapAF &fact)
  : fact_(fact), V_(fact_.createSet()), Vmap_(fact_.createPWMap())
  , E_(fact_.createSet()), mapB_(fact_.createPWMap())
  , mapD_(fact_.createPWMap()), Emap_(fact_.createPWMap())
  , subEmap_(fact_.createPWMap()) {}
DSBGraph::DSBGraph(const PWMapAF &fact, const Set &V, const PWMap &Vmap
  , const PWMap &mapB, const PWMap &mapD
  , const PWMap &Emap, const PWMap &subEmap)
  : fact_(fact), V_(V), Vmap_(Vmap)
    , E_(mapB.dom().intersection(mapD.dom()))
    , mapB_(mapB), mapD_(mapD), Emap_(Emap), subEmap_(subEmap) {}

member_imp(DSBGraph, Set, V);
member_imp(DSBGraph, PWMap, Vmap);
member_imp(DSBGraph, Set, E);
member_imp(DSBGraph, PWMap, mapB);
member_imp(DSBGraph, PWMap, mapD);
member_imp(DSBGraph, PWMap, Emap);
member_imp(DSBGraph, PWMap, subEmap);

std::ostream &operator<<(std::ostream &out, const DSBGraph &dg)
{
  out << "V: " << dg.V() << "\n";
  out << "Vmap: " << dg.Vmap() << "\n\n";
  out << "E: " << dg.E() << "\n";
  out << "mapB: " << dg.mapB() << "\n";
  out << "mapD: " << dg.mapD() << "\n";
  out << "Emap: " << dg.Emap() << "\n";
  out << "subEmap: " << dg.subEmap() << "\n";

  return out;
}

DSBGraph DSBGraph::addSV(const Set &vertices) const
{
  if (!vertices.isEmpty() && vertices.intersection(V_).isEmpty()) {
    PWMap new_Vmap(std::move(Vmap_));
    PWMap new_mapB(std::move(mapB_)), new_mapD(std::move(mapD_));
    PWMap new_Emap(std::move(Emap_)), new_subE(std::move(subEmap_));

    Set new_V = new_V.cup(vertices);

    Set SV = new_Vmap.image(); // Identifiers of SV
    std::size_t dims = vertices.arity();
    MD_NAT max = SV.isEmpty() ? MD_NAT(dims, 0) : SV.maxElem();
    for (unsigned int j = 0; j < dims; ++j)
      max[j] = max[j] + 1;
    Map m = fact_.createMap(vertices, Exp(max));
    new_Vmap.emplaceBack(m);

    return DSBGraph(fact_, new_V, new_Vmap, new_mapB, new_mapD
      , new_Emap, new_subE);
  }

  else if (!vertices.intersection(V_).isEmpty())
    Debug::ERROR("Trying to add existing vertices: ", vertices, " to DSBG\n");

  return DSBGraph(fact_);
}

DSBGraph DSBGraph::addSE(const PWMap &pw1, const PWMap &pw2) const
{
  Set edges = fact_.createSet(), edges1 = pw1.dom(), edges2 = pw2.dom();
  if (edges1 == edges2) {
    edges = edges1;
    if (!edges.isEmpty() && edges.intersection(E_).isEmpty()) {
      Set new_V(std::move(V_));
      PWMap new_Vmap(std::move(Vmap_));
      PWMap new_mapB(std::move(mapB_)), new_mapD(std::move(mapD_));
      PWMap new_Emap(std::move(Emap_)), new_subE(std::move(subEmap_));

      Set SE = Emap_.image(); // Identifiers of SV
      std::size_t dims = edges.arity();
      MD_NAT max = SE.isEmpty() ? MD_NAT(dims, 0) : SE.maxElem();
      for (unsigned int j = 0; j < dims; ++j)
        max[j] = max[j] + 1;
      Map m = fact_.createMap(edges, max);
      new_Emap.emplaceBack(m);

      new_mapB = new_mapB.concatenation(pw1);
      new_mapD = new_mapD.concatenation(pw2);
   
      return DSBGraph(fact_, new_V, new_Vmap, new_mapB, new_mapD
        , new_Emap, new_subE);
    }

    else if (!edges.intersection(E_).isEmpty())
      Debug::ERROR("Trying to add existing edges: ", edges, " to DSBG\n");
  }

  return DSBGraph(fact_);
}

DSBGraph DSBGraph::eraseVertices(const Set &vs) const
{
  Set new_V = V_.difference(vs);
  PWMap new_Vmap = Vmap_.restrict(V_);

  Set eraseE = mapB_.preImage(vs).cup(mapD_.preImage(vs));
  Set new_E = E_.difference(eraseE);
  PWMap new_mapB = mapB_.restrict(new_E);
  PWMap new_mapD = mapD_.restrict(new_E);
  PWMap new_Emap = Emap_.restrict(new_E);
  PWMap new_subE = subEmap_.restrict(new_E);

  return DSBGraph(fact_, new_V, new_Vmap, new_mapB, new_mapD
    , new_Emap, new_subE);
}

} // namespace LIB

} // namespace SBG
