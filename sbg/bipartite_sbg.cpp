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
    Util::ERROR("Trying to add existing vertices: ", vertices, " to SBG\n");
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
    // TODO
    Util::ERROR("The domain of ", edges1, " is different from ", edges2, "\n");
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
    Util::ERROR("Trying to add existing edges: ", edges1, " to SBG\n");
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

} // namespace LIB

} // namespace SBG
