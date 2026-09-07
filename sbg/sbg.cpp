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
#include "sbg/integer.hpp"
#include "util/debug.hpp"

#include <iostream>

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// SBG -------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

// Constructors/Destructors ----------------------------------------------------

SBG::SBG() 
  : _V(), _Vmap(), _E(), _map1(), _map2(), _Emap() {}

SBG::SBG(const Set& V, const PWMap& Vmap
  , const PWMap& map1, const PWMap& map2
  , const PWMap& Emap)
  : _V(V), _Vmap(Vmap), _E(map1.domain().intersection(map2.domain()))
    , _map1(map1), _map2(map2), _Emap(Emap) {}

// Getters ---------------------------------------------------------------------

const Set& SBG::V() const { return _V; }

const PWMap& SBG::Vmap() const { return _Vmap; }

const Set& SBG::E() const { return _E; }

const PWMap& SBG::map1() const { return _map1; }

const PWMap& SBG::map2() const { return _map2; }

const PWMap& SBG::Emap() const { return _Emap; }

// Setters ---------------------------------------------------------------------

void SBG::addSetVertex(const Set& vertices)
{
  if (!vertices.intersection(_V).isEmpty()) {
    Util::ERROR("SBG::addSetVertex: trying to add existing vertices: ", vertices
      , " to SBG\n");
  } else if (!vertices.isEmpty()) {
    Set set_vertices = _Vmap.image();
    std::size_t arity = vertices.arity();
    IntTuple max = set_vertices.isEmpty() ? IntTuple{arity, 0}
      : set_vertices.maxElem();
    _Vmap.emplace(vertices, max + IntTuple{arity, 1});
    _V = std::move(_V).cup(std::move(vertices));
  }
}

void SBG::addSetEdge(const PWMap& pw1, const PWMap& pw2)
{
  Set edges1 = pw1.domain();
  Set edges2 = pw2.domain();
  if (edges1 != edges2) {
    Util::ERROR("SBG::addSetEdge: ", edges1, " is different from ", edges2
      , "\n");
  } else if (edges1.intersection(_E).isEmpty()) {
    Set edges = edges1;
    if (!edges.isEmpty()) {
      Set set_edges = _Emap.image();
      std::size_t arity = edges.arity();
      IntTuple max = set_edges.isEmpty() ? IntTuple{arity, 0}
        : set_edges.maxElem();
      _map1 = std::move(_map1).concatenation(pw1);
      _map2 = std::move(_map2).concatenation(pw2);
      _Emap.emplace(edges, max + IntTuple{arity, 1});
      _E = std::move(_E).cup(std::move(edges));
    }
  } else {
    Util::ERROR("SBG::addSetEdge: trying to add existing edges: ", edges1
      , " to SBG\n");
  }
}

// Operators -------------------------------------------------------------------

std::ostream& operator<<(std::ostream& out, const SBG& g)
{
  out << "V: " << g.V() << "\n";
  out << "Vmap: " << g.Vmap() << "\n\n";
  out << "E: " << g.E() << "\n";
  out << "map1: " << g.map1() << "\n";
  out << "map2: " << g.map2() << "\n";
  out << "Emap: " << g.Emap() << "\n";

  return out;
}

} // namespace LIB

} // namespace SBG
