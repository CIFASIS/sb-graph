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
#include "sbg/natural.hpp"
#include "sbg/pwmap_fact.hpp"
#include "sbg/set_fact.hpp"
#include "util/debug.hpp"

#include <iostream>

namespace SBG {

namespace LIB {

// Constructors/Destructors ----------------------------------------------------

BipartiteSBG::BipartiteSBG()
  : _V(SET_FACT.createSet()), _Vmap(PWMAP_FACT.createPWMap())
  , _E(SET_FACT.createSet()), _map1(PWMAP_FACT.createPWMap())
  , _map2(PWMAP_FACT.createPWMap()), _Emap(PWMAP_FACT.createPWMap())
  , _X(SET_FACT.createSet()), _Y(SET_FACT.createSet()) {}

BipartiteSBG::BipartiteSBG(const Set& V, const PWMap& Vmap
  , const PWMap& map1, const PWMap& map2, const PWMap& Emap
  , const Set& X, const Set& Y)
  : _V(V), _Vmap(Vmap), _E(map1.domain().intersection(map2.domain()))
    , _map1(map1), _map2(map2), _Emap(Emap), _X(X), _Y(Y) {}

// Getters ---------------------------------------------------------------------

const Set& BipartiteSBG::V() const { return _V; }

const PWMap& BipartiteSBG::Vmap() const { return _Vmap; }

const Set& BipartiteSBG::E() const { return _E; }

const PWMap& BipartiteSBG::map1() const { return _map1; }

const PWMap& BipartiteSBG::map2() const { return _map2; }

const PWMap& BipartiteSBG::Emap() const { return _Emap; }

const Set& BipartiteSBG::X() const { return _X; }

const Set& BipartiteSBG::Y() const { return _Y; }

// Setters ---------------------------------------------------------------------

void BipartiteSBG::addSetVertex(const Set& X, const Set& Y)
{
  Set vertices = X.cup(Y);
  if (!vertices.intersection(_V).isEmpty()) {
    Util::ERROR("BipartiteSBG::addSetVertex: trying to add existing vertices: "
      , vertices, " to SBG\n");
  } else if (!vertices.isEmpty()) {
    _V = _V.cup(vertices);
    Set set_vertices = _Vmap.image();
    std::size_t arity = vertices.arity();
    MD_NAT max = set_vertices.isEmpty() ? MD_NAT{arity, 0}
      : set_vertices.maxElem();
    MD_NAT one_all_dims{arity, 1};
    _Vmap.emplace(vertices, max + one_all_dims);
    _X = _X.cup(X);
    _Y = _Y.cup(Y);
  }
}

void BipartiteSBG::addSetEdge(const PWMap& pw1, const PWMap& pw2)
{
  Set edges1 = pw1.domain();
  Set edges2 = pw2.domain();
  if (edges1 != edges2) {
    Util::ERROR("BipartiteSBG::addSetEdge: ", edges1, " is different from "
      , edges2, "\n");
  } else if (edges1.intersection(_E).isEmpty()) {
    Set edges = edges1;
    if (!edges.isEmpty()) {
      _E = std::move(_E).cup(std::move(edges));
      Set set_edges = _Emap.image();
      std::size_t arity = edges.arity();
      MD_NAT max = set_edges.isEmpty() ? MD_NAT{arity, 0}
        : set_edges.maxElem();
      MD_NAT one_all_dims{arity, 1};
      _map1 = std::move(_map1).concatenation(pw1);
      _map2 = std::move(_map2).concatenation(pw2);
      _Emap.emplace(edges, max + one_all_dims);
    }
  } else {
    Util::ERROR("BipartiteSBG::addSetEdge: trying to add existing edges: "
      , edges1, " to SBG\n");
  }
}

// Operators -------------------------------------------------------------------

std::ostream& operator<<(std::ostream& out, const BipartiteSBG& g)
{
  out << "V: " << g.V() << "\n";
  out << "Vmap: " << g.Vmap() << "\n\n";
  out << "E: " << g.E() << "\n";
  out << "map1: " << g.map1() << "\n";
  out << "map2: " << g.map2() << "\n";
  out << "Emap: " << g.Emap() << "\n";
  out << "X: " << g.X() << "\n";
  out << "Y: " << g.Y() << "\n";

  return out;
}

// Extra operations ------------------------------------------------------------

BipartiteSBG copy(unsigned int copies, BipartiteSBG sbg)
{
  if (copies == 0) {
    Util::ERROR("BipartiteSBG::copy: zeros copies is not allowed\n");
  }

  Set X = sbg.X();
  Set Y = sbg.Y();
  Set V = sbg.V();
  PWMap Vmap = sbg.Vmap();
  PWMap map1 = sbg.map1();
  PWMap map2 = sbg.map2();
  PWMap Emap = sbg.Emap();
  Set E = sbg.E();

  for (unsigned int j = 1; j < copies; ++j) {
    MD_NAT max_v = sbg.V().maxElem();
    Set set_vertices = Vmap.image();
    while (!set_vertices.isEmpty()) {
      Set min_elem_set = SET_FACT.createSet(set_vertices.minElem());
      Set vertices = Vmap.preImage(min_elem_set);
      Set jth_X = vertices.intersection(X);
      Set jth_Y = vertices.intersection(Y);
      sbg.addSetVertex(jth_X.offset(max_v), jth_Y.offset(max_v));

      set_vertices = set_vertices.difference(min_elem_set);
    }

    Expression offset_v;
    for (std::size_t k = 0; k < max_v.arity(); ++k) {
      offset_v = offset_v.cartesianProduct(Expression{1, max_v[k]});
    }
    PWMap offset_pw_v = PWMAP_FACT.createPWMap(Map{V, offset_v});

    MD_NAT max_e = sbg.E().maxElem();
    Expression offset_e;
    for (std::size_t k = 0; k < max_e.arity(); ++k) {
      offset_e = offset_e.cartesianProduct(Expression{1, max_e[k]});
    }
    PWMap offset_pw_e = PWMAP_FACT.createPWMap(Map{E, offset_e});
    PWMap inverse_offset_pw_e = offset_pw_e.inverse();

    Set set_edges = Emap.image();
    while (!set_edges.isEmpty()) {
      Set min_elem_set = SET_FACT.createSet(set_edges.minElem());
      Set edges = Emap.preImage(min_elem_set);
      PWMap pw1 = map1.restrict(edges);
      PWMap pw2 = map2.restrict(edges);
      pw1 = offset_pw_v.composition(pw1.composition(inverse_offset_pw_e));
      pw2 = offset_pw_v.composition(pw2.composition(inverse_offset_pw_e));
      sbg.addSetEdge(pw1, pw2);

      set_edges = set_edges.difference(min_elem_set);
    }
  }

  return sbg;
}

} // namespace LIB

} // namespace SBG
