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
#include "sbg/pwmap_fact.hpp"
#include "sbg/set_fact.hpp"
#include "util/debug.hpp"
#include "util/logger.hpp"

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Minimum Vertex Topological Sort Algorithm -----------------------------------
////////////////////////////////////////////////////////////////////////////////

MinVertexTS::MinVertexTS()
  : _smap(PWMAP_FACT.createPWMap()), _dsbg(), _visitedSV(SET_FACT.createSet())
    , _priority(SET_FACT.createSet()), _same_SV(SET_FACT.createSet())
    , _independent(SET_FACT.createSet()) {}

/*
 * @brief Given any two vertices u and v of the repetition, it checks that any
 * (u, v) edge (if it exists) satisfies that u comes before v in the sorting. 
 */
void checkSorting(const PWMap& result, const DirectedSBG& dsbg)
{
  PWMap mapB = dsbg.mapB(); 
  PWMap mapD = dsbg.mapD(); 
  Set result_vertices = result.domain().cup(result.image());
  Set ED = mapD.preImage(result_vertices);
  Set EB = mapB.preImage(result_vertices);
  Set E = EB.intersection(ED);
  PWMap jth_result = result;
  Set E_old = E;
  while (!E.isEmpty()) {
    Set Ej = jth_result.composition(mapD).equalImage(mapB); 
    E = E.difference(Ej);
    Util::ERROR_UNLESS(E_old != E, "checkSorting: the sorting ", result
      , " is incorrect\n");

    jth_result = result.composition(jth_result);
  }
}

PWMap MinVertexTS::repetition(const Set& init_V
  , const DirectedSBG& dsbg) const
{
  PWMap result = PWMAP_FACT.createPWMap();

  PWMap Vmap = dsbg.Vmap();
  Set Vj = init_V;
  Set init_SV = Vmap.image(init_V);
  bool repetition = true;
  do {
    PWMap jth_smap = _smap.restrict(Vj);
    Set V_plus = Vmap.preImage(Vmap.image(Vj)).difference(_smap.domain());
    result.emplace(V_plus, (*jth_smap.begin()).law());
    Vj = _smap.image(Vj);
    repetition = !Vmap.image(Vj).intersection(init_SV).isEmpty();
  } while (!repetition);

  checkSorting(result, dsbg);

  return result;
}

MD_NAT MinVertexTS::getMinVertex()
{
  Set V = _dsbg.V(); 
  PWMap mapD = _dsbg.mapD();
  _independent = V.difference(mapD.image());

  Set Vj = _independent;
  if (Vj.isEmpty()) {
    Util::ERROR("MinVertexTS::getMinVertex: the SBG is not acyclic\n");
  }
  Set independent_priority = Vj.intersection(_priority);
  if (!independent_priority.isEmpty()) {
    Vj = independent_priority;
  }
  Set Vj_same_SV = Vj.intersection(_same_SV);
  if (!Vj_same_SV.isEmpty()) {
    Vj = Vj_same_SV;
  }

  return Vj.minElem();
}

PWMap MinVertexTS::calculate(const DirectedSBG& dsbg
  , const PWMap& pmap)
{
  Util::DEBUG_LOG << "Topological sort dsbg:\n" << dsbg << "\n\n";

  _dsbg = dsbg;

  _smap = PWMAP_FACT.createPWMap();

  if (dsbg.V().isEmpty()) {
    return _smap;
  }

  _priority = _dsbg.V();
  _same_SV = _dsbg.V();
  Set visited_SV = SET_FACT.createSet();
  Expression successor_expr{_dsbg.V().arity(), 1, 0};
  MD_NAT vj;
  MD_NAT old_vj = _dsbg.V().difference(_dsbg.mapD().image()).minElem();
  do {
    // Find new minimum vertex, and add it to the sorting
    vj = getMinVertex();
    Set vj_set = SET_FACT.createSet(vj);
    successor_expr = Expression{vj, old_vj};
    _smap.emplace(vj_set, successor_expr);

    // Handle repetition
    PWMap Vmap = _dsbg.Vmap();
    Set repeatedSV = _visitedSV.intersection(Vmap.image(vj_set));
    if (!repeatedSV.isEmpty()) {
      PWMap smap_plus = repetition(vj_set, dsbg);
      _smap = std::move(smap_plus).combine(std::move(_smap));
      vj = _smap.domain().difference(_smap.image()).minElem();
    } else {
      _visitedSV = std::move(_visitedSV).disjointCup(Vmap.image(vj_set));
    } 

    // Update values for new iteration
    _dsbg.eraseVertices(_smap.domain());
    old_vj = vj;
    _priority = pmap.preImage(pmap.image(vj_set));
    _same_SV = Vmap.preImage(Vmap.image(vj_set));
  } while (!_dsbg.V().isEmpty());

  _smap.compact();
  Util::DEBUG_LOG << "Topological sort result:\n" << _smap << "\n\n";
  return _smap;
}

} // namespace detail

} // namespace LIB

} // namespace SBG
