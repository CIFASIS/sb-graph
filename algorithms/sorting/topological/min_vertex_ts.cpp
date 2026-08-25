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

#include "algorithms/sorting/topological/min_vertex_ts.hpp"
#include "sbg/natural.hpp"
#include "util/debug.hpp"
#include "util/logger.hpp"

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Minimum Vertex Topological Sort Algorithm -----------------------------------
////////////////////////////////////////////////////////////////////////////////

MinVertexTS::MinVertexTS()
  : _smap(), _dsbg(), _visitedSV(), _priority(), _independent()
    , _max_repetition_depth(0) {}

// Repetition ------------------------------------------------------------------

std::tuple<Set, PWMap> MinVertexTS::detectRepetition(Set Vj
  , const DirectedSBG& dsbg)
{
  Set repetition_vertices;
  PWMap smap_plus;

  PWMap Vmap = dsbg.Vmap();
  Set init_SV = Vmap.image(Vj);
  bool repetition = false;
  unsigned int n = 0;
  do {
    // Calculate jth result. 
    repetition_vertices = std::move(repetition_vertices).disjointCup(Vj);
    Set Vj_plus = Vmap.preImage(Vmap.image(Vj));
    smap_plus = std::move(smap_plus)
      .concatenation(Map{Vj_plus, (*(_smap.restrict(Vj).begin())).law()});

    // Values for the next iteration.
    Vj = _smap.image(Vj);
    repetition = !Vmap.image(Vj).intersection(init_SV).isEmpty();
    ++n;
  } while (!repetition && n < _max_repetition_depth);

  if (!repetition) {
    repetition_vertices = Set{};
    smap_plus = PWMap{};
  }

  return {repetition_vertices, smap_plus};
}

PWMap MinVertexTS::repetition(const Set& init_V, const DirectedSBG& dsbg)
{
  // Identify repetition and extend it to vertices in the same set-vertex.
  auto [repetition_vertices, smap_plus] = detectRepetition(init_V, dsbg);
  smap_plus = smap_plus.restrict(smap_plus.domain().difference(_smap.domain()));

  // Get erased edges up to the repetition, and erase edges that belong to the
  // same set-edge. 
  Set outgoing_edges = dsbg.mapB().preImage(repetition_vertices);
  PWMap Emap = dsbg.Emap();
  Set E_plus = Emap.preImage(Emap.image(outgoing_edges));
  DirectedSBG dsbg_copy = _dsbg;
  dsbg_copy.eraseEdges(E_plus);

  // Identify vertices with ingoing edges after the previous step. These are
  // "cut" vertices for the repetition.
  PWMap not_independent{dsbg_copy.mapD().image()
    .intersection(smap_plus.domain())};
  not_independent = not_independent.combine(smap_plus);
  not_independent = not_independent.combine(_smap); 
  PWMap rmap = not_independent.mapInf();

  Set reach_end_vertices = rmap.preImage(_start);
  PWMap result = smap_plus.restrict(reach_end_vertices);

  return result;
}

// getVertex -------------------------------------------------------------------

MD_NAT MinVertexTS::getVertex()
{
  Set V = _dsbg.V(); 
  PWMap mapD = _dsbg.mapD();
  _independent = V.difference(mapD.image());

  // Independent vertices with no incoming edges.
  Set Vj = _independent;
  if (Vj.isEmpty()) {
    Util::ERROR("MinVertexTS::getVertex: the SBG is not acyclic\n");
  }

  // Independent vertices with priority treatment.
  Set independent_priority = Vj.intersection(_priority);
  if (!independent_priority.isEmpty()) {
    Vj = independent_priority;
  }

  // Independent vertices with priority treatment, that belong to a visited
  // set-vertex.
  Set Vj_repeated_SV = Vj;
  Set Vj_set_vertex = _dsbg.Vmap().image(Vj);
  for (auto rit = _visitedSV.rbegin(); rit != _visitedSV.rend(); ++rit) {
    Set repeatedSV = (*rit).intersection(Vj_set_vertex);
    if (!repeatedSV.isEmpty()) {
      Vj_repeated_SV = _dsbg.Vmap().preImage(repeatedSV).intersection(Vj);
      break;
    }
  }
  if (!Vj_repeated_SV.isEmpty()) {
    Vj = Vj_repeated_SV;
  }

  return Vj.minElem();
}

// Algorithm -------------------------------------------------------------------

/**
 * @brief Checks that the returned map is a topological sort of the dsbg. 
 */
void checkSort(PWMap& smap, const DirectedSBG& dsbg)
{
  // Check if it is injective, that is, if is an actual order.
  Set fixed_points = smap.fixedPoints();
  Set visited_img;
  for (const Map& m : smap) {
    Set m_image = m.image().difference(fixed_points);
    if (!visited_img.intersection(m_image).isEmpty()) {
      Util::ERROR("checkSort: proposed result is not an order\n");
    }
    visited_img = std::move(visited_img).disjointCup(std::move(m_image));
  }

  // Check if the sort is total.
  Util::ERROR_UNLESS(smap.domain() == dsbg.V()
    , "checkSort: the order is partial\n");
}

PWMap MinVertexTS::calculate(const DirectedSBG& dsbg
  , const PWMap& scc_map)
{
  Util::DEBUG_LOG << "Topological sort dsbg:\n" << dsbg << "\n\n";

  Set V = dsbg.V();
  _dsbg = dsbg;

  _smap = PWMap{};
  _priority = V;

  if (V.isEmpty()) {
    return _smap;
  }

  std::size_t arity = V.arity();
  Expression successor_expr{arity, 1, 0};
  MD_NAT vj;
  MD_NAT old_vj = V.difference(_dsbg.mapD().image()).minElem();
  _start = old_vj;
  do {
    // Find new vertex without dependencies, and add it to the sorting.
    vj = getVertex();
    Set vj_set{vj};
    Util::DEBUG_LOG << "vj_set: " << vj_set << "\n";
    successor_expr = Expression{vj, old_vj};
    _smap.emplace(vj_set, successor_expr);

    // Handle repetition.
    PWMap Vmap = _dsbg.Vmap();
    Set vj_set_vertex = Vmap.image(vj_set);
    Set repeatedSV;
    for (auto rit = _visitedSV.rbegin(); rit != _visitedSV.rend(); ++rit) {
      repeatedSV = (*rit).intersection(vj_set_vertex);
      if (!repeatedSV.isEmpty()) {
        break;
      }
    }
    if (!repeatedSV.isEmpty()) {
      PWMap smap_plus = repetition(vj_set, dsbg);
      _smap = std::move(smap_plus).concatenation(std::move(_smap));
      vj = _smap.domain().difference(_smap.image()).minElem();
      _max_repetition_depth = 0;
    } else {
      _visitedSV.push_back(Vmap.image(vj_set));
      _max_repetition_depth++;
    } 

    // Update values for new iteration.
    _dsbg.eraseVertices(_smap.domain());
    old_vj = vj;
    _priority = scc_map.preImage(scc_map.image(vj_set));

    Util::DEBUG_LOG << "smap: " << _smap << "\n";
    Util::DEBUG_LOG << "dsbg: " << _dsbg << "\n\n";
  } while (!_dsbg.V().isEmpty());

  _smap.compact();
  Util::DEBUG_LOG << "Topological sort result:\n" << _smap << "\n\n";
  checkSort(_smap, dsbg);
  return _smap;
}

} // namespace detail

} // namespace LIB

} // namespace SBG
