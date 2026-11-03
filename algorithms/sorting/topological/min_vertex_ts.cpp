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

#include <sbgraph/detail/algorithms/sorting/topological/min_vertex_ts.hpp>
#include <sbgraph/detail/algorithms/sorting/topological/ts_utils.hpp>
#include "util/debug.hpp"
#include "util/logger.hpp"

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Minimum Vertex Topological Sort Algorithm -----------------------------------
////////////////////////////////////////////////////////////////////////////////

MinVertexTS::MinVertexTS() : _depth(0) {}

// Repetition ------------------------------------------------------------------

PWMap MinVertexTS::extendRepetition()
{
  PWMap Vmap = _dsbg.Vmap();

  // Extend repetition to whole set-vertices.
  PWMap smap_plus;
  for (std::size_t k = 1; _depth >= k; ++k) {
    std::size_t sz = _repetitionSV.size();
    Set domain_plus = Vmap.preImage(_repetitionSV[sz - k]);
    sz = _repetition_sort.size();
    smap_plus.emplace(domain_plus, _repetition_sort[sz - k].law());
  }

  // Take out vertices that are assigned a value that is not in V.
  smap_plus = smap_plus.restrict(smap_plus.preImage(_dsbg.V()));

  // Take out already sorted elements.
  Set sorted = _smap.domain().cup(smap_plus.preImage(_smap.image()));
  Set plus_domain = smap_plus.domain().difference(sorted);
  smap_plus = smap_plus.restrict(plus_domain);

  return smap_plus;
}

PWMap MinVertexTS::repetition(const PWMap& Emap)
{
  // Identify repetition and extend it to vertices in the same set-vertex,
  // taking out sorted vertices in the domain, and also vertices that map to
  // another sorted vertex, i.e, checking the successor map is injective.
  PWMap smap_plus = extendRepetition();
  if (smap_plus.isEmpty()) {
    return PWMap{};
  }
  Set plus_domain = smap_plus.domain();

  // Delete set-edges that had an edge that was deleted up to the repetition.
  Set Eplus = Emap.preImage(_repetitionSE.accumulated(_depth));
  DirectedSBG dsbg_copy = _dsbg;
  dsbg_copy.eraseEdges(Eplus);

  // Identify vertices with ingoing edges after the previous step. These are
  // "cut" vertices for the repetition.
  Set dependent_vertices = dsbg_copy.mapD().image().intersection(plus_domain);
  PWMap rmap = PWMap{dependent_vertices}.combine(smap_plus).mapInf();

  Set indep_vertices = plus_domain.difference(rmap.preImage(dependent_vertices));
  PWMap result = smap_plus.restrict(indep_vertices);

  return result;
}

// getVertex -------------------------------------------------------------------

IntTuple MinVertexTS::getVertex(Set old_Vj)
{
  Set V = _dsbg.V();
  PWMap mapD = _dsbg.mapD();

  // Independent vertices with no incoming edges.
  Set Vj = V.difference(mapD.image());
  if (Vj.isEmpty()) {
    Util::ERROR("MinVertexTS::getVertex: the SBG is not acyclic\n");
  }
  Vj = Vj.difference(old_Vj);

  // Independent vertices with priority treatment.
  Set independent_priority = Vj.intersection(_priority);
  if (!independent_priority.isEmpty()) {
    Vj = independent_priority;
  }

  // Independent vertices with priority treatment, that belong to a visited
  // set-vertex.
  Set Vj_set_vertex = _dsbg.Vmap().image(Vj);
  Set Vj_repeated_SV = _repetitionSV.accumulated().intersection(Vj_set_vertex);
  if (!Vj_repeated_SV.isEmpty()) {
    Vj = _dsbg.Vmap().preImage(Vj_repeated_SV).intersection(Vj);
  }

  return Vj.minElem();
}

// Algorithm -------------------------------------------------------------------

PWMap MinVertexTS::calculate(const DirectedSBG& dsbg, const PWMap& scc_map)
{
  Util::DEBUG_LOG << "Topological sort dsbg:\n" << dsbg << "\n\n";

  Set V = dsbg.V();
  if (V.isEmpty()) {
    return PWMap{};
  }
  _dsbg = dsbg;
  PWMap mapB = _dsbg.mapB();
  PWMap Emap = _dsbg.Emap();

  IntTuple vj;
  IntTuple old_vj = V.difference(_dsbg.mapD().image()).minElem();
  Set old_vj_set{old_vj};
  _smap = PWMap{old_vj_set};
  _repetitionSV.pushBack(dsbg.Vmap().image(old_vj_set));
  _priority = scc_map.preImage(scc_map.image(old_vj_set));
  _repetitionSE.pushBack(Emap.image(mapB.preImage(old_vj_set)));
  _dsbg.eraseVertices(_smap.domain());
  do {
    // Find new vertex without dependencies, and add it to the sorting.
    vj = getVertex(Set{old_vj});
    Set vj_set{vj};
    Util::DEBUG_LOG << "vj_set: " << vj_set << "\n";
    Map new_map{vj_set, Expression{vj, old_vj}};
    _smap.emplace(new_map);
    _repetition_sort.push_back(new_map);
    _repetitionSE.pushBack(Emap.image(mapB.preImage(vj_set)));

    // Handle repetition.
    Set vj_set_vertex = _dsbg.Vmap().image(vj_set);
    Set repeatedSV = _repetitionSV.accumulated().intersection(vj_set_vertex);
    _repetitionSV.pushBack(_dsbg.Vmap().image(vj_set));
    if (!repeatedSV.isEmpty()) {
      _depth = _repetitionSV.repetitionDepth();
      PWMap smap_plus = repetition(dsbg.Emap());
      _smap = std::move(smap_plus).concatenation(std::move(_smap));
      vj = _smap.domain().difference(_smap.image()).minElem();
      _depth = 0;
    }

    // Update values for new iteration.
    Set starts = _smap.domain().difference(_smap.image());
    if (starts.cardinal() == 1) {
      _dsbg.eraseVertices(_smap.domain());
    } else {
      _dsbg.eraseVertices(_smap.domain().intersection(_smap.image()));
    }
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

}  // namespace detail

}  // namespace LIB

}  // namespace SBG
