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

namespace SBG {

namespace LIB {

template<typename Set>
SBGraph<Set>::SBGraph() : V_(), Vmap_(), E_(), map1_(), map2_(), Emap_() {}
template<typename Set>
SBGraph<Set>::SBGraph(Set V, PWMap<Set> Vmap, PWMap<Set> map1, PWMap<Set> map2, PWMap<Set> Emap)
  : V_(V), Vmap_(Vmap), map1_(map1), map2_(map2), Emap_(Emap) {}

template<typename Set>
PWMap<Set> connectedComponents(SBGraph<Set> g)
{
  PWMap<Set> rmap, old_rmap;

  if (isEmpty(g.E())) return rmap;

  do {
    old_rmap = rmap;

    PWMap<Set> ermap1 = composition(rmap, g.map1());
    PWMap<Set> ermap2 = composition(rmap, g.map2());

    PWMap<Set> rmap1 = minAdjMap(ermap1, ermap2);
    PWMap<Set> rmap2 = minAdjMap(ermap2, ermap1);
    rmap1 = rmap1.combine(rmap);
    rmap2 = rmap2.combine(rmap);

    PWMap aux_rmap = minMap(rmap1, rmap2);
    rmap = minMap(rmap, aux_rmap);

    if (!(rmap == old_rmap)) {
      rmap = aux_rmap;
      rmap = mapInf(rmap);
    }
  } while (rmap == old_rmap); 

  return rmap;
}

} // namespace LIB

} // namespace SBG
