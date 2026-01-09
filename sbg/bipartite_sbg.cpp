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

#include "sbg/bipartite_sbg.hpp"

namespace SBG {

namespace LIB {

BipartiteSBG::BipartiteSBG()
  : _V(SET_FACT.createSet()), _Vmap(PW_FACT.createPWMap())
  , _E(SET_FACT.createSet()), _map1(PW_FACT.createPWMap())
  , _map2(PW_FACT.createPWMap()), _Emap(PW_FACT.createPWMap())
  , _subEmap(PW_FACT.createPWMap())
  , _X(SET_FACT.createSet()), _Y(SET_FACT.createSet()) {}

BipartiteSBG::BipartiteSBG(const Set& V, const PWMap& Vmap
  , const PWMap& map1, const PWMap& map2
  , const PWMap& Emap, const PWMap& subEmap
  , const Set& X, const Set& Y)
  : _V(V), _Vmap(Vmap), _E(map1.dom().intersection(map2.dom()))
    , _map1(map1), _map2(map2), _Emap(Emap), _subEmap(subEmap)
    , _X(X), _Y(Y) {}

const Set& BipartiteSBG::V() const { return _V; }

const PWMap& BipartiteSBG::Vmap() const { return _Vmap; }

const Set& BipartiteSBG::E() const { return _E; }

const PWMap& BipartiteSBG::map1() const { return _map1; }

const PWMap& BipartiteSBG::map2() const { return _map2; }

const PWMap& BipartiteSBG::Emap() const { return _Emap; }

const PWMap& BipartiteSBG::subEmap() const { return _subEmap; }

const Set& BipartiteSBG::X() const { return _X; }

const Set& BipartiteSBG::Y() const { return _Y; }

BipartiteSBG& BipartiteSBG::operator=(const BipartiteSBG& other)
{
  _V = other._V;
  _Vmap = other._Vmap;
  _E = other._E;
  _map1 = other._map1;
  _map2 = other._map2;
  _Emap = other._Emap;
  _subEmap = other._subEmap;
  _X = other._X;
  _Y = other._Y;

  return *this;
}

std::ostream& operator<<(std::ostream& out, const BipartiteSBG& g)
{
  out << "V: " << g.V() << "\n";
  out << "Vmap: " << g.Vmap() << "\n\n";
  out << "E: " << g.E() << "\n";
  out << "map1: " << g.map1() << "\n";
  out << "map2: " << g.map2() << "\n";
  out << "Emap: " << g.Emap() << "\n";
  out << "subEmap: " << g.subEmap() << "\n";
  out << "X: " << g.X() << "\n";
  out << "Y: " << g.Y() << "\n";

  return out;
}

void BipartiteSBG::addSV(const Set& X, const Set& Y)
{
  Set vertices = X.cup(Y);
  if (!vertices.intersection(_V).isEmpty()) {
    Util::ERROR("Trying to add existing vertices: ", vertices, " to SBG\n");
  } else if (!vertices.isEmpty() && vertices.intersection(_V).isEmpty()) {
    _V = _V.cup(vertices);
    Set SV = _Vmap.image();
    std::size_t dims = vertices.arity();
    MD_NAT max = SV.isEmpty() ? MD_NAT(dims, 0) : SV.maxElem();
    for (unsigned int j = 0; j < dims; ++j) {
      max[j] = max[j] + 1;
    }
    _Vmap.emplaceBack(Map(vertices, Exp(max)));
    _X = _X.cup(X);
    _Y = _Y.cup(Y);
  }
}

void BipartiteSBG::addSE(const PWMap& pw1, const PWMap& pw2)
{
  Set edges = SET_FACT.createSet(), edges1 = pw1.dom(), edges2 = pw2.dom();
  if (!edges.intersection(_E).isEmpty()) {
    Util::ERROR("Trying to add existing edges: ", edges, " to SBG\n");
  }
  else if (edges1 == edges2) {
    edges = edges1;
    if (!edges.isEmpty() && edges.intersection(_E).isEmpty()) {
      Set SE = _Emap.image();
      std::size_t dims = edges.arity();
      MD_NAT max = SE.isEmpty() ? MD_NAT(dims, 0) : SE.maxElem();
      for (unsigned int j = 0; j < dims; ++j) {
        max[j] = max[j] + 1;
      }
      _map1 = _map1.concatenation(pw1);
      _map2 = _map2.concatenation(pw2);
      _Emap.emplaceBack(Map(edges, max));
    }
  }
}

BipartiteSBG BipartiteSBG::copy(unsigned int times) const
{
  Set ith_V = _V;
  Set new_V = ith_V;
  PWMap ith_Vmap = _Vmap;
  PWMap new_Vmap = ith_Vmap;
  PWMap ith_map1 = _map1;
  PWMap new_map1 = ith_map1;
  PWMap ith_map2 = _map2;
  PWMap new_map2 = ith_map2;
  PWMap ith_Emap = _Emap;
  PWMap new_Emap = ith_Emap;
  PWMap ith_subE = _subEmap;
  PWMap new_subE = ith_subE;
  Set ith_X = _X;
  Set new_X = ith_X;
  Set ith_Y = _Y;
  Set new_Y = ith_Y;

  if (!ith_V.isEmpty()) {
    MD_NAT maxv = ith_V.maxElem();
    auto dims = maxv.arity();
    MD_NAT maxV
      = ith_Vmap.isEmpty() ? MD_NAT(dims, 0) : ith_Vmap.image().maxElem();
    MD_NAT maxe = _E.isEmpty() ? MD_NAT(dims, 0) : _E.maxElem();
    MD_NAT maxE
      = ith_Emap.isEmpty() ? MD_NAT(dims, 0) : ith_Emap.image().maxElem();
    MD_NAT maxx = ith_X.maxElem();
    MD_NAT maxy = ith_Y.maxElem();

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
        new_X = new_X.disjointCup(ith_X);
        new_Y = new_Y.disjointCup(ith_Y);
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

      ith_X = ith_X.offset(maxx);
      ith_Y = ith_Y.offset(maxy);
    }
  }

  return BipartiteSBG(new_V, new_Vmap, new_map1, new_map2, new_Emap, new_subE
    , new_X, new_Y);
}

} // namespace LIB

} // namespace SBG
