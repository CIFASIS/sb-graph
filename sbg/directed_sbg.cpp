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

#include "sbg/directed_sbg.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Directed SBG ----------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

member_imp(DSBG, Set, V);
member_imp(DSBG, PWMap, Vmap);
member_imp(DSBG, Set, E);
member_imp(DSBG, PWMap, mapB);
member_imp(DSBG, PWMap, mapD);
member_imp(DSBG, PWMap, Emap);
member_imp(DSBG, PWMap, subEmap);

DSBG::DSBG()
  : V_(SET_FACT.createSet()), Vmap_(PW_FACT.createPWMap())
  , E_(SET_FACT.createSet()), mapB_(PW_FACT.createPWMap())
  , mapD_(PW_FACT.createPWMap()), Emap_(PW_FACT.createPWMap())
  , subEmap_(PW_FACT.createPWMap()) {}
DSBG::DSBG(const Set &V, const PWMap &Vmap
  , const PWMap &mapB, const PWMap &mapD
  , const PWMap &Emap, const PWMap &subEmap)
  : V_(V), Vmap_(Vmap)
    , E_(mapB.dom().intersection(mapD.dom()))
    , mapB_(mapB), mapD_(mapD), Emap_(Emap), subEmap_(subEmap) {}

DSBG &DSBG::operator=(const DSBG &other)
{
  V_ = other.V_;
  Vmap_ = other.Vmap_;
  E_ = other.E_;
  mapB_ = other.mapB_;
  mapD_ = other.mapD_;
  Emap_ = other.Emap_;
  subEmap_ = other.subEmap_;

  return *this;
}

std::ostream &operator<<(std::ostream &out, const DSBG &dg)
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

DSBG DSBG::addSV(const Set &vertices) const
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
    Map m(vertices, Exp(max));
    new_Vmap.emplaceBack(m);

    return DSBG(new_V, new_Vmap, new_mapB, new_mapD, new_Emap, new_subE);
  }

  else if (!vertices.intersection(V_).isEmpty())
    Util::ERROR("Trying to add existing vertices: ", vertices, " to DSBG\n");

  return DSBG();
}

DSBG DSBG::addSE(const PWMap &pw1, const PWMap &pw2) const
{
  Set edges = SET_FACT.createSet(), edges1 = pw1.dom(), edges2 = pw2.dom();
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
      Map m(edges, max);
      new_Emap.emplaceBack(m);

      new_mapB = new_mapB.concatenation(pw1);
      new_mapD = new_mapD.concatenation(pw2);
   
      return DSBG(new_V, new_Vmap, new_mapB, new_mapD, new_Emap, new_subE);
    }

    else if (!edges.intersection(E_).isEmpty())
      Util::ERROR("Trying to add existing edges: ", edges, " to DSBG\n");
  }

  return DSBG();
}

DSBG DSBG::eraseVertices(const Set &vs) const
{
  Set new_V = V_.difference(vs);
  PWMap new_Vmap = Vmap_.restrict(V_);

  Set eraseE = mapB_.preImage(vs).cup(mapD_.preImage(vs));
  Set new_E = E_.difference(eraseE);
  PWMap new_mapB = mapB_.restrict(new_E);
  PWMap new_mapD = mapD_.restrict(new_E);
  PWMap new_Emap = Emap_.restrict(new_E);
  PWMap new_subE = subEmap_.restrict(new_E);

  return DSBG(new_V, new_Vmap, new_mapB, new_mapD, new_Emap, new_subE);
}

} // namespace LIB

} // namespace SBG
