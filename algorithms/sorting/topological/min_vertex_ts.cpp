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

/*
 * @brief Checks that the returned map is a topological sort of the dsbg. 
 */
void checkSort(const PWMap& result, const DirectedSBG& dsbg)
{
  // Check unique start.

  // Check if it is injective.

  // Check that, if there is and edge (u, v) then u is before v in the sort.
}

PWMap MinVertexTS::repetition(const Set& init_V, const DirectedSBG& dsbg) const
{
  PWMap result;

  PWMap Vmap = dsbg.Vmap();
  Set Vj = init_V;
  Set init_SV = Vmap.image(init_V);
  Expression final_expr;
  bool repetition = true;
  unsigned int n = 0;
  do {
    PWMap jth_smap = _smap.restrict(Vj);
    Set V_plus = Vmap.preImage(Vmap.image(Vj)).difference(_smap.domain());
    final_expr = (*jth_smap.begin()).law();
    result.emplace(V_plus, final_expr);
    Vj = _smap.image(Vj);
    repetition = !Vmap.image(Vj).intersection(init_SV).isEmpty();
    ++n;
  } while (!repetition && n < _max_repetition_depth);

  if (!repetition) {
    result = PWMap{};
  }

  return result;
}

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

PWMap MinVertexTS::calculate(const DirectedSBG& dsbg
  , const PWMap& pmap)
{
  Util::DEBUG_LOG << "Topological sort dsbg:\n" << dsbg << "\n\n";

  _dsbg = dsbg;

  _smap = PWMap{};

  if (dsbg.V().isEmpty()) {
    return _smap;
  }

  _priority = _dsbg.V();
  Expression successor_expr{_dsbg.V().arity(), 1, 0};
  MD_NAT vj;
  MD_NAT old_vj = _dsbg.V().difference(_dsbg.mapD().image()).minElem();
  do {
    // Find new without dependencies, and add it to the sorting
    vj = getVertex();
    Set vj_set{vj};
    Util::DEBUG_LOG << "vj_set: " << vj_set << "\n";
    successor_expr = Expression{vj, old_vj};
    _smap.emplace(vj_set, successor_expr);

    // Handle repetition
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
      _smap = std::move(smap_plus).combine(std::move(_smap));
      vj = _smap.domain().difference(_smap.image()).minElem();
      _max_repetition_depth = 0;
    } else {
      _visitedSV.push_back(Vmap.image(vj_set));
      _max_repetition_depth++;
    } 

    // Update values for new iteration
    _dsbg.eraseVertices(_smap.domain());
    old_vj = vj;
    _priority = pmap.preImage(pmap.image(vj_set));
    Util::DEBUG_LOG << "smap: " << _smap << "\n";
    Util::DEBUG_LOG << "dsbg: " << _dsbg << "\n\n";
  } while (!_dsbg.V().isEmpty());

  _smap.compact();
  Util::DEBUG_LOG << "Topological sort result:\n" << _smap << "\n\n";
  return _smap;
}

} // namespace detail

} // namespace LIB

} // namespace SBG
