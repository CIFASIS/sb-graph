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

#include "sbg/sbg_algorithms.hpp"

namespace SBG {

namespace LIB {

// Connected components --------------------------------------------------------

template<typename Set>
PWMap<Set> connectedComponents(SBGraph<Set> g)
{
  if (!isEmpty(g.V())) {
    unsigned int dims = g.V()[0].size();
    SBGMap<Set> id(g.V(), Exp(dims));
    PWMap<Set> rmap(id), old_rmap;

    if (isEmpty(g.E())) return rmap;

    do {
      old_rmap = rmap;

      PWMap<Set> ermap1 = composition(rmap, g.map1());
      PWMap<Set> ermap2 = composition(rmap, g.map2());

      PWMap<Set> rmap1 = minAdjMap(ermap1, ermap2);
      PWMap<Set> rmap2 = minAdjMap(ermap2, ermap1);
      rmap1 = combine(rmap1, rmap);
      rmap2 = combine(rmap2, rmap);

      PWMap aux_rmap = minMap(rmap1, rmap2);
      rmap = minMap(rmap, aux_rmap);

      if (!(rmap == old_rmap)) {
        rmap = aux_rmap;
        rmap = mapInf(rmap);
      }
    } while (rmap != old_rmap); 

    return rmap;
  }

  return PWMap<Set>();
}

// Minimum reachable -----------------------------------------------------------

template<typename Set>
PathInfo<Set>::PathInfo() : succs_(), reps_() {}
template<typename Set>
PathInfo<Set>::PathInfo(PWMap<Set> succs, PWMap<Set> reps) : succs_(succs), reps_(reps) {}

member_imp_temp(template<typename Set>, PathInfo<Set>, PWMap<Set>, succs);
member_imp_temp(template<typename Set>, PathInfo<Set>, PWMap<Set>, reps);

template<typename Set>
bool notEqId(SBGMap<Set> sbgmap) { return !(image(sbgmap) == sbgmap.dom()); }

// Update rep iff new is less than current. Without this function, if there are
// vertices v1, v2, v3 and edges v1->v2, v1->v3 where rmap(v2) = rmap(v3) the
// algorithm could switch infinitely between those two reps.
template<typename Set>
PWMap<Set> updateMap(Set V, PWMap<Set> smap, PWMap<Set> new_smap, PWMap<Set> rmap)
{
  SetPiece mi_zero(V[0].size(), Interval(0));
  Set zero(mi_zero);

  PWMap<Set> res;
  PWMap<Set> vto_rep = composition(rmap, smap), new_vto_rep = composition(rmap, new_smap);
  //PWMap<Set> delta = vto_rep - new_vto_rep;
  //Set not_updated = preImage(zero, delta);
  Set not_updated = equalImage(vto_rep, new_vto_rep);

  res = restrict(not_updated, smap);
  res = combine(res, new_smap);

  return res;
}

template<typename Set>
PWMap<Set> minReach1(Set V, PWMap<Set> mapB, PWMap<Set> mapD, PWMap<Set> smap, PWMap<Set> rmap)
{
  SetPiece mi_zero(V[0].size(), Interval(0));
  Set zero(mi_zero);

  PWMap<Set> adj_smap = minAdjMap(mapB, mapD, rmap); // Get minimum adjacent vertex with minimum representative
  PWMap new_smap = minMap(rmap, adj_smap, smap, rmap); // Get minimum between current rep and adjacent reps
  new_smap = updateMap(V, smap, new_smap, rmap); // Update rep iff new is less than current
  new_smap = combine(new_smap, smap); // Keep current rep if a new one wasn't found

  return new_smap;
}

// Handle ONE recursion
template<typename Set>
PWMap<Set> recursion(unsigned int n, Set ER, DSBGraph<Set> dsbg, PWMap<Set> smap, PWMap<Set> semap, PWMap<Set> rmap)
{
  PWMap<Set> mapB = dsbg.mapB(), mapD = dsbg.mapD(), Emap = dsbg.Emap();

  Set set_edges = image(ER, Emap); // Sub-set-edges in recursive paths
  Set ER_plus = preImage(set_edges, Emap); // Edges in the same sub-set-edges as E_recursive
 
  PWMap<Set> tildeB = restrict(ER_plus, mapB), tildeD = restrict(ER_plus, mapD); // Restrict edges to recursion
  PWMap<Set> smap_plus = composition(tildeD, minInv(tildeB));  // In a recursion the succ is the only adjacent vertex
  PWMap<Set> new_smap = combine(smap_plus, smap);

  PWMap<Set> rmap_plus = composition(rmap, mapInf(smap_plus, n)); // Solve recursion
  PWMap<Set> new_rmap = minMap(rmap, rmap_plus);
  new_rmap = combine(new_rmap, rmap);

  return new_rmap;
}

template<typename Set>
MinReach<Set>::MinReach() : sbg_() {}
template<typename Set>
MinReach<Set>::MinReach(DSBGraph<Set> sbg) : sbg_(sbg) {}

member_imp_temp(template<typename Set>, MinReach<Set>, DSBGraph<Set>, sbg);

template<typename Set>
PathInfo<Set> MinReach<Set>::calculate()
{
  DSBGraph dsbg = sbg();

  if (!isEmpty(dsbg.V())) {
    Set V = dsbg.V(), E = dsbg.E();
    PWMap<Set> mapB = dsbg.mapB(), mapD = dsbg.mapD(), Emap = dsbg.Emap();

    SetPiece mi_zero(V[0].size(), Interval(0));
    Set zero(mi_zero);

    PWMap<Set> old_smap, old_semap; // Old vertex and edge successors maps
    PWMap<Set> new_smap(V), new_semap(E); // New vertex and edge successors maps
    PWMap<Set> new_rmap(V); // New vertex reps map

    Set Vc; // Vertices that changed sucessor
    Set Ec; // Edges that changed successor

    // If a path changes, we have to check if this allows new vertices to
    // reach a new rep. The condition of the loop corresponds to the fact
    // that paths are uniquely defined by the sequence of vertices they traverse.
    do {
      old_smap = new_smap;
      new_smap = minReach1(V, mapB, mapD, new_smap, new_rmap); // Find adjacent vertex that reaches a minor vertex than current one
      Vc = difference(V, equalImage(old_smap, new_smap));

      if (!isEmpty(Vc)) {
        new_rmap = mapInf(new_smap); // Get minimum reachable following path

        Set E_succ = equalImage(mapD, composition(new_smap, mapB));
        if (!isEmpty(E_succ)) { // A new path was chosen
          old_semap = new_semap;
          PWMap<Set> mapB_succ = restrict(E_succ, mapB), mapD_succ = restrict(E_succ, mapD);
          new_semap = composition(minInv(mapB_succ), mapD_succ);
          new_semap = combine(new_semap, old_semap);

          Set not_changed = equalImage(new_semap, old_semap);
          Ec = difference(E, not_changed);    

          Set ER; // Recursive edges that changed its successor
          PWMap<Set> semap_nth = composition(new_semap, new_semap);
          unsigned int j = 0;
          for (; j < nmbrSV(dsbg); j++) { // The max depth of recursion is the number of SVs
            ER = equalImage(Emap, composition(Emap, filterMap(semap_nth, notEqId))); 
            semap_nth = composition(semap_nth, new_semap); 

            if (!isEmpty(ER)) break;
          }

          ER = intersection(ER, Ec);
          if (!isEmpty(ER)) { // There are recursions, lets handle one of them
            ER = Set(ER[0]);
            new_rmap = recursion(j, ER, dsbg, new_smap, new_semap, new_rmap);
          }
        }
      }
    } while (!isEmpty(Vc));

    return PathInfo<Set>(new_smap, new_rmap);
  }

  return PathInfo<Set>();
}

// Template instantiations -----------------------------------------------------

template BasePWMap connectedComponents<UnordSet>(BaseSBG g);
template CanonPWMap connectedComponents<OrdSet>(CanonSBG g);

template struct PathInfo<UnordSet>;
template struct PathInfo<OrdSet>;

template struct MinReach<UnordSet>;
template struct MinReach<OrdSet>;

} // namespace LIB

} // namespace SBG
