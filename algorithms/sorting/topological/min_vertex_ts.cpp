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

  if (repetition) {
    // Take out vertices that are mapped to a value that doesn't belong to any
    // vertex.
    smap_plus = smap_plus.restrict(smap_plus.preImage(dsbg.V()));
  } else {
    repetition_vertices = Set{};
    smap_plus = PWMap{};
  }

  return {repetition_vertices, smap_plus};
}

PWMap sortPartial(const PWMap& smap_plus, const Set& ordered, const DirectedSBG& dsbg)
{
  PWMap result;

  Set plus_domain = smap_plus.domain();
  Set plus_image = smap_plus.image();
  Set starts = plus_domain.difference(plus_image);
  if (starts.cardinal() > 1) {
    Set ends = plus_image.difference(plus_domain);
    PWMap rmap = smap_plus.combine(PWMap{ends}).mapInf();
    PWMap ends_to_starts = rmap.restrict(starts).inverse();

    Set V = starts;
    PWMap Vmap = dsbg.Vmap().restrict(V);
    MinVertexTS ts;
    PWMap smap = ts.calculate(DirectedSBG{V, Vmap, PWMap{}, PWMap{}, PWMap{}}
      , PWMap{});
    Set not_ordered = ends_to_starts.domain().difference(ordered);
    result = smap.composition(ends_to_starts.restrict(not_ordered));
    result = smap_plus.combine(std::move(result));
  } else {
    result = smap_plus;
  }

  return result;
}

PWMap MinVertexTS::repetition(const Set& init_V, const DirectedSBG& dsbg)
{
  // Identify repetition and extend it to vertices in the same set-vertex,
  // taking out sorted vertices in the domain, and also vertices that map to
  // another sorted vertex, i.e, checking the successor map is injective.
  auto [repetition_vertices, smap_plus] = detectRepetition(init_V, dsbg);
  if (smap_plus.isEmpty()) {
    return PWMap{};
  }

  Set ordered = _smap.domain().cup(smap_plus.preImage(_smap.image()));
  Set plus_domain = smap_plus.domain().difference(ordered);
  smap_plus = smap_plus.restrict(plus_domain);
  smap_plus = sortPartial(smap_plus, _smap.domain(), dsbg);
  plus_domain = smap_plus.domain();

  // Get erased edges of the repetition, and erase edges that belong to the
  // same set-edge. 
  Set outgoing_edges = dsbg.mapB().preImage(repetition_vertices);
  PWMap Emap = dsbg.Emap();
  Set E_plus = Emap.preImage(Emap.image(outgoing_edges));
  DirectedSBG dsbg_copy = _dsbg;
  dsbg_copy.eraseEdges(E_plus);

  // Identify vertices with ingoing edges after the previous step. These are
  // "cut" vertices for the repetition.
  Set dependent_vertices = dsbg_copy.mapD().image().intersection(plus_domain);
  PWMap rmap = PWMap{dependent_vertices}.combine(smap_plus).mapInf();

  Set indep_vertices = plus_domain
    .difference(rmap.preImage(dependent_vertices));
  PWMap result = smap_plus.restrict(indep_vertices);

  return result;
}

// getVertex -------------------------------------------------------------------

IntTuple MinVertexTS::getVertex()
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
  for (auto it = _visitedSV.begin(); it != _visitedSV.end(); ++it) {
    Set repeatedSV = (*it).intersection(Vj_set_vertex);
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
      Util::ERROR("checkSort: proposed result ", smap, " is not an order\n");
    }
    visited_img = std::move(visited_img).disjointCup(std::move(m_image));
  }

  // Check if the sort is total.
  Util::ERROR_UNLESS(smap.domain() == dsbg.V()
    , "checkSort: the order ", smap, " is partial\n");
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
  IntTuple vj;
  IntTuple old_vj = V.difference(_dsbg.mapD().image()).minElem();
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
    _smap.compact();

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
