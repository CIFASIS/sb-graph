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

#include "sbg/natural.hpp"
#include "sbg/directed_sbg.hpp"
#include "sbg/pwmap_fact.hpp"
#include "sbg/set_fact.hpp"
#include "util/debug.hpp"

#include <iostream>

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Directed SBG ----------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

// Constructors/Destructors ----------------------------------------------------

DirectedSBG::DirectedSBG()
  : _V(SET_FACT.createSet()), _Vmap(PWMAP_FACT.createPWMap())
  , _E(SET_FACT.createSet()), _mapB(PWMAP_FACT.createPWMap())
  , _mapD(PWMAP_FACT.createPWMap()), _Emap(PWMAP_FACT.createPWMap()) {}

DirectedSBG::DirectedSBG(const Set& V, const PWMap& Vmap
  , const PWMap& mapB, const PWMap& mapD
  , const PWMap& Emap)
  : _V(V), _Vmap(Vmap)
    , _E(mapB.domain().intersection(mapD.domain()))
    , _mapB(mapB), _mapD(mapD), _Emap(Emap) {}

// Getters ---------------------------------------------------------------------

const Set& DirectedSBG::V() const { return _V; }

const PWMap& DirectedSBG::Vmap() const { return _Vmap; }

const Set& DirectedSBG::E() const { return _E; }

const PWMap& DirectedSBG::mapB() const { return _mapB; }

const PWMap& DirectedSBG::mapD() const { return _mapD; }

const PWMap& DirectedSBG::Emap() const { return _Emap; }

// Setters ---------------------------------------------------------------------

void DirectedSBG::addSetVertex(const Set& vertices)
{
  if (!vertices.intersection(_V).isEmpty()){
    Util::ERROR("Trying to add existing vertices: ", vertices
    , " to DirectedSBG\n");
  } else if (!vertices.isEmpty()) {
    _V = std::move(_V).cup(vertices);
    Set set_vertices = _Vmap.image();
    std::size_t arity = vertices.arity();
    MD_NAT max = set_vertices.isEmpty() ? MD_NAT{arity, 0}
      : set_vertices.maxElem();
    MD_NAT one_all_dims{arity, 1};
    _Vmap.emplace(vertices, max + one_all_dims);
  }
}

void DirectedSBG::addSetEdge(const PWMap& pwB, const PWMap& pwD)
{
  Set edgesB = pwB.domain();
  Set edgesD = pwD.domain();
  if (edgesB != edgesD) {
    // TODO
    Util::ERROR("The domain of ", edgesB, " is different from ", edgesD, "\n");
  } else if (edgesB.intersection(_E).isEmpty()) {
    Set edges = edgesB;
    if (!edges.isEmpty()) {
      _E = std::move(_E).cup(std::move(edges));
      Set set_edges = _Emap.image();
      std::size_t arity = edges.arity();
      MD_NAT max = set_edges.isEmpty() ? MD_NAT(arity, 0) : set_edges.maxElem();
      MD_NAT one_all_dims{arity, 1};
      _mapB = std::move(_mapB).concatenation(pwB);
      _mapD = std::move(_mapD).concatenation(pwD);
      _Emap.emplace(edges, max + one_all_dims);
    }
  } else {
    Util::ERROR("Trying to add existing edges: ", edgesB, " to SBG\n");
  }
}

void DirectedSBG::eraseVertices(const Set& V)
{
  _V = _V.difference(V);
  _Vmap = _Vmap.restrict(_V);

  Set eraseE = _mapB.preImage(V).cup(_mapD.preImage(V));
  _E = _E.difference(eraseE);
  _mapB = _mapB.restrict(_E);
  _mapD = _mapD.restrict(_E);
  _Emap = _Emap.restrict(_E);
}

void DirectedSBG::eraseEdges(const Set& E)
{
  _E = _E.difference(E);
  _mapB = _mapB.restrict(_E);
  _mapD = _mapD.restrict(_E);
  _Emap = _Emap.restrict(_E);
}

// Operators -------------------------------------------------------------------

std::ostream& operator<<(std::ostream& out, const DirectedSBG& dg)
{
  out << "V: " << dg.V() << "\n";
  out << "Vmap: " << dg.Vmap() << "\n\n";
  out << "E: " << dg.E() << "\n";
  out << "mapB: " << dg.mapB() << "\n";
  out << "mapD: " << dg.mapD() << "\n";
  out << "Emap: " << dg.Emap() << "\n";

  return out;
}

} // namespace LIB

} // namespace SBG
