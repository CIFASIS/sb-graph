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
    , _n(0) {}

Expression calculateExpr(const MD_NAT& old_value, const MD_NAT& new_value)
{
  return Expression{old_value};
}

PWMap MinVertexTS::independentRepetition(const Set& independent_V_plus,
  const Expression& successor_expr) const
{
  PWMap result = PWMAP_FACT.createPWMap();

  result.emplace(independent_V_plus, successor_expr);
  bool independent_image = result.image().difference(independent_V_plus).isEmpty();
  Util::ERROR_UNLESS(independent_image/*&& result.isInjective()*/
    , "MinVertexTS::independentRepetition: incorrect guessing ", result);

  return result;
}

PWMap MinVertexTS::dependentRepetition(const Set& init_V) const
{
  PWMap result = PWMAP_FACT.createPWMap();

  Set Vi = init_V;
  Set V = SET_FACT.createSet();
  for (unsigned int j = 0; j < _n; ++j) {
    Vi = _smap.image(Vi);
    V = std::move(V).disjointCup(std::move(Vi));
  }
  PWMap smap_repetition = _smap.restrict(V);
  PWMap mapB = _dsbg.mapB();
  PWMap mapD = _dsbg.mapD();
  Set E_repetition = smap_repetition.composition(mapB).equalImage(mapD);

  PWMap Emap = _dsbg.Emap();
  Set E_plus = Emap.preImage(Emap.image(E_repetition));
  PWMap mapB_plus = mapB.restrict(E_plus);
  PWMap mapD_plus = mapD.restrict(E_plus);
  result = mapB_plus.minAdj(mapD_plus);

  /*TODO: check well-formed result*/

  return result;
}

PWMap MinVertexTS::calculate(const DirectedSBG& dsbg
  , const PWMap& pmap)
{
  Util::DEBUG_LOG << "Topological sort dsbg:\n" << dsbg << "\n\n";

  _dsbg = dsbg;

  PWMap _smap = PWMAP_FACT.createPWMap();

  Set same_SCC = _dsbg.V();
  Set same_SV = _dsbg.V();
  Set visited_SV = SET_FACT.createSet();
  Expression successor_expr{_dsbg.V().arity(), 1, 0};
  MD_NAT old_vi;
  do {
    Set V = _dsbg.V(); 
    PWMap mapD = _dsbg.mapD();
    Set independent = V.difference(mapD.image());

    Set Vi = independent;
    if (Vi.isEmpty()) {
      Util::ERROR("Topological sorting: the SBG is not acyclic");
    }
    Set independent_same_SCC = Vi.intersection(same_SCC);
    if (independent_same_SCC.isEmpty()) {
      Vi = independent_same_SCC;
    }
    Set Vi_same_SV = Vi.intersection(same_SV);
    if (Vi_same_SV.isEmpty()) {
      Vi = Vi_same_SV;
    }

    // Handle repetition
    PWMap Vmap = _dsbg.Vmap();
    MD_NAT vi = Vi.minElem();
    Set vi_set = SET_FACT.createSet(vi);
    Set repeatedSV = _visitedSV.intersection(Vmap.image(vi_set));
    if (!repeatedSV.isEmpty()) {
      Set V_plus = Vmap.preImage(Vmap.image(vi_set));
      Set independent_V_plus = independent.intersection(V_plus);
      PWMap smap_plus = PWMAP_FACT.createPWMap();
      if (!independent_V_plus.isEmpty()) {
        smap_plus = independentRepetition(independent_V_plus, successor_expr);
      } else {
        smap_plus = dependentRepetition(vi_set);
      }
      _smap = std::move(smap_plus).combine(std::move(_smap));
      _n = 0;
    } else  {
      _smap.emplace(vi_set, successor_expr);

      _visitedSV = std::move(_visitedSV).disjointCup(Vmap.image(vi_set));
      ++_n;
    } 

    // Update values for new iteration
    _dsbg.eraseVertices(_smap.domain());
    old_vi = vi;
    successor_expr = calculateExpr(old_vi, vi);
    same_SCC = pmap.preImage(pmap.image(vi_set));
    same_SV = Vmap.preImage(Vmap.image(vi_set));
  } while (!_dsbg.V().isEmpty());

  _smap.compact();
  Util::DEBUG_LOG << "Topological sort result:\n" << _smap << "\n\n";
  return _smap;
}

} // namespace detail

} // namespace LIB

} // namespace SBG
