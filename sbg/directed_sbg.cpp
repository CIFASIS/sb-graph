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
#include "sbg/integer.hpp"
#include "sbg/map.hpp"
#include "sbg/rational.hpp"
#include "util/debug.hpp"

#include <iostream>

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Directed SBG ----------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

// Constructors/Destructors ----------------------------------------------------

DirectedSBG::DirectedSBG() : _V(), _Vmap(), _E(), _mapB(), _mapD(), _Emap() {}

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
    Set set_vertices = _Vmap.image();
    std::size_t arity = vertices.arity();
    IntTuple max = set_vertices.isEmpty() ? IntTuple{arity, 0}
      : set_vertices.maxElem();
    _Vmap.emplace(vertices, max + IntTuple{arity, 1});
    _V = std::move(_V).cup(std::move(vertices));
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
      Set set_edges = _Emap.image();
      std::size_t arity = edges.arity();
      IntTuple max = set_edges.isEmpty() ? IntTuple{arity, 0} : set_edges.maxElem();
      _mapB = std::move(_mapB).concatenation(pwB);
      _mapD = std::move(_mapD).concatenation(pwD);
      _Emap.emplace(edges, max + IntTuple{arity, 1});
      _E = std::move(_E).cup(std::move(edges));
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

// Non-member functions --------------------------------------------------------

DirectedSBG copy(unsigned int copies, DirectedSBG dsbg)
{

  if (copies == 0) {
    Util::ERROR("DirectedSBG::copy: zeros copies is not allowed\n");
  }

  Set V = dsbg.V();
  PWMap Vmap = dsbg.Vmap();
  PWMap mapB = dsbg.mapB();
  PWMap mapD = dsbg.mapD();
  PWMap Emap = dsbg.Emap();
  Set E = dsbg.E();

  DirectedSBG result = dsbg;
  for (unsigned int j = 1; j < copies; ++j) {
    IntTuple max_v = result.V().maxElem();
    dsbg.foreachSetVertex([&](const IntTuple& SV) {
      Set vertices = Vmap.preImage(Set{SV});
     result.addSetVertex(vertices.translate(max_v));
    });

    Expression offset_v;
    for (std::size_t k = 0; k < max_v.arity(); ++k) {
      offset_v = offset_v.cartesianProduct(Expression{Rational{1}
        , Rational{max_v[k]}});
    }
    PWMap offset_pw_v{Map{V, offset_v}};

    IntTuple max_e = result.E().maxElem();
    Expression offset_e;
    for (std::size_t k = 0; k < max_e.arity(); ++k) {
      offset_e = offset_e.cartesianProduct(Expression{Rational{1}
        , Rational{max_e[k]}});
    }
    PWMap offset_pw_e{Map{E, offset_e}};
    PWMap inverse_offset_pw_e = offset_pw_e.inverse();

    Set set_edges = Emap.image();
    dsbg.foreachSetEdge([&](const IntTuple& SE) {
      Set edges = Emap.preImage(Set{SE});
      PWMap pwB = mapB.restrict(edges);
      PWMap pwD = mapD.restrict(edges);
      pwB = offset_pw_v.composition(pwB.composition(inverse_offset_pw_e));
      pwD = offset_pw_v.composition(pwD.composition(inverse_offset_pw_e));
      result.addSetEdge(pwB, pwD);
    });
  }

  return result;
}

} // namespace LIB

} // namespace SBG
