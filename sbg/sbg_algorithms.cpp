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

// -----------------------------------------------------------------------------
// Connected components --------------------------------------------------------
// -----------------------------------------------------------------------------

PWMap connectedComponents(SBGraph g)
{
  if (!isEmpty(g.V())) {
    unsigned int dims = g.V()[0].size();
    SBGMap id(g.V(), Exp(dims));
    PWMap rmap(id), old_rmap;

    if (isEmpty(g.E())) return rmap;

    do {
      old_rmap = rmap;

      PWMap ermap1 = composition(rmap, g.map1());
      PWMap ermap2 = composition(rmap, g.map2());

      PWMap rmap1 = minAdjMap(ermap1, ermap2);
      PWMap rmap2 = minAdjMap(ermap2, ermap1);
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

  return PWMap();
}

// -----------------------------------------------------------------------------
// Minimum reachable -----------------------------------------------------------
// -----------------------------------------------------------------------------

PathInfo::PathInfo() : succs_(), reps_() {}
PathInfo::PathInfo(PWMap succs, PWMap reps) : succs_(succs), reps_(reps) {}

member_imp(PathInfo, PWMap, succs);
member_imp(PathInfo, PWMap, reps);

bool eqId(SBGMap sbgmap) { return isId(sbgmap); }

bool notEqId(SBGMap sbgmap) { return !(isId(sbgmap)); }

// Update rep iff new is less than current. Without this function, if there are
// vertices v1, v2, v3 and edges v1->v2, v1->v3 where rmap(v2) = rmap(v3) the
// algorithm could switch infinitely between those two reps.
PWMap updateMap(Set V, PWMap smap, PWMap new_smap, PWMap rmap)
{
  SetPiece mi_zero(V[0].size(), Interval(0));
  Set zero(mi_zero);

  PWMap res;
  PWMap vto_rep = composition(rmap, smap), new_vto_rep = composition(rmap, new_smap);
  Set not_updated = equalImage(vto_rep, new_vto_rep);

  res = restrict(not_updated, smap);
  res = combine(res, new_smap);

  return res;
}

PWMap minReach1(Set V, Set E, PWMap mapB, PWMap mapD, PWMap smap, PWMap rmap)
{
  PWMap auto_succs = filterMap(eqId, smap);
  Set auto_reps = equalImage(auto_succs, rmap);
  Set usable_verts = cup(auto_reps, dom(filterMap(notEqId, smap)));

  PWMap auxB = restrict(E, mapB), auxD = restrict(E, mapD); 
  PWMap adj_smap = minAdjMap(auxB, auxD, rmap); // Get minimum adjacent vertex with minimum representative
  PWMap new_smap = minMap(rmap, adj_smap, restrict(usable_verts, smap), rmap); // Get minimum between current rep and adjacent reps
  new_smap = updateMap(V, smap, new_smap, rmap); // Update rep iff new is less than current
  new_smap = combine(new_smap, adj_smap); 
  new_smap = combine(new_smap, smap);

  return new_smap;
}

// Handle ONE recursion
PathInfo MinReach::recursion(unsigned int n, Set ER, Set rv, PWMap smap, PWMap rmap)
{
  PWMap mapB = dsbg().mapB(), mapD = dsbg().mapD(), Emap = dsbg().Emap(), Vmap = dsbg().Vmap();
  PWMap ERB = restrict(ER, mapB), ERD = restrict(ER, mapD);

  Set start = image(ERB), end = image(ERD), aux_start = start;
  start = difference(start, end), end = difference(end, aux_start);

  Set ER_plus = preImage(image(ER, Emap), Emap);
  Set VR_plus = cup(image(ER_plus, mapB), image(ER_plus, mapD));

  PWMap new_smap;
  Set repeated = intersection(start, end);
  Set available_vertices = dsbg().V();
  while (isEmpty(repeated)) {
    Set side = intersection(VR_plus, preImage(image(start, Vmap), Vmap));
    Set ER_side = preImage(start, mapB);
    Set ER_plus_side = preImage(image(ER_side, Emap), Emap);

    PWMap sideB  = restrict(ER_plus_side, mapB), sideD = restrict(ER_plus_side, mapD);
    PWMap ith = restrict(available_vertices, composition(sideD, minInv(sideB)));
    new_smap = combine(ith, new_smap);
 
    start = image(start, smap);
    repeated = intersection(start, end);
    available_vertices = difference(available_vertices, side);
  }

  new_smap = combine(restrict(rv, smap), new_smap);
  new_smap = combine(new_smap, smap);
  PWMap rmap_plus = composition(rmap, mapInf(new_smap, n)); // Solve recursion
  PWMap new_rmap = minMap(rmap, rmap_plus);
  new_rmap = combine(new_rmap, rmap);

  return PathInfo(new_smap, new_rmap);
}

MinReach::MinReach() : dsbg_() {}
MinReach::MinReach(DSBGraph sbg) : dsbg_(sbg) {}

member_imp(MinReach, DSBGraph, dsbg);

PathInfo MinReach::calculate(Set unmatched_V)
{
  DSBGraph dg = dsbg();

  if (!isEmpty(dg.V())) {
    Set V = dg.V(), E = dg.E();
    PWMap mapB = dg.mapB(), mapD = dg.mapD(), Emap = dg.Emap();

    PWMap old_smap, old_semap; // Old vertex and edge successors maps
    PWMap new_smap(V), new_semap(E); // New vertex and edge successors maps
    PWMap new_rmap(V); // New vertex reps map

    Set Vc; // Vertices that changed sucessor
    Set Ec; // Edges that changed successor

    Set unmatched_D = intersection(unmatched_V, image(mapD)), unmatched_B = intersection(unmatched_V, image(mapB));
    Set reach_vertices = unmatched_D;
    Set reach_edges;

    // If a path changes, we have to check if this allows new vertices to
    // reach a new rep. The condition of the loop corresponds to the fact
    // that paths are uniquely defined by the sequence of vertices they traverse.
    do {
      reach_edges = preImage(reach_vertices, mapD);
      reach_vertices = cup(reach_vertices, image(reach_edges, mapB));

      old_smap = new_smap;
      new_smap = minReach1(V, reach_edges, mapB, mapD, new_smap, new_rmap); // Find adjacent vertex that reaches a minor vertex than current one
      Vc = difference(V, equalImage(old_smap, new_smap));

      if (!isEmpty(intersection(reach_vertices, unmatched_B))) {
        new_rmap = mapInf(new_smap); // Get minimum reachable following path
        return PathInfo(new_smap, new_rmap);
      }

      if (!isEmpty(Vc)) {
        new_rmap = mapInf(new_smap); // Get minimum reachable following path

        Set E_succ = equalImage(mapD, composition(new_smap, mapB));
        if (!isEmpty(E_succ)) { // A new path was chosen
          old_semap = new_semap;
          PWMap mapB_succ = restrict(E_succ, mapB), mapD_succ = restrict(E_succ, mapD);
          new_semap = composition(minInv(E_succ, mapB_succ), mapD_succ);
          new_semap = combine(new_semap, old_semap);

          Set not_changed = equalImage(new_semap, old_semap);
          Ec = difference(E, not_changed);

          if (!isEmpty(Ec)) {
            Set ER;  // Recursive edges that changed its successor
            PWMap old_semap_nth, semap_nth = composition(new_semap, new_semap);
            unsigned int j = 0;
            do {  // The max depth of recursion is the number of SVs
              ER = equalImage(Emap, composition(Emap, filterMap(notEqId, semap_nth)));
              old_semap_nth = semap_nth;
              semap_nth = composition(new_semap, semap_nth);

              j++;
            } while (isEmpty(ER) && j < nmbrSV(dg) && !(old_semap_nth == semap_nth));
            ER = intersection(ER, Ec);

            semap_nth = new_semap;
            for (unsigned int k = 0; k < j; k++) {
              ER = cup(ER, image(ER, semap_nth));
              semap_nth = composition(new_semap, semap_nth);
            }

            if (!isEmpty(ER)) {  // There are recursions, lets handle one of them
              PathInfo res = recursion(j, ER, reach_vertices, new_smap, new_rmap);
              new_smap = res.succs();
              new_rmap = res.reps();
            }
          }
        }
      }
    } while (!isEmpty(Vc));

    return PathInfo(new_smap, new_rmap);
  }

  return PathInfo();
}

// -----------------------------------------------------------------------------
// Matching --------------------------------------------------------------------
// -----------------------------------------------------------------------------

// Short path ------------------------------------------------------------------

bool moreThanOne(SetPiece mdi) { return cardinal(mdi) > 1; }

Set SBGMatching::getManyToOne()
{
  Set res;

  for (SBGMap sbgmapB : mapB_ref()) {
    for (SBGMap sbgmapD : mapD_ref()) {
      Exp expB = sbgmapB.exp(), expD = sbgmapD.exp();
      if (isConstant(expD)) {
        Set dom = intersection(sbgmapB.dom(), sbgmapD.dom());
        if (!isEmpty(dom)) {
          dom = intersection(dom, paths_edges());
          if (!isEmpty(dom)) {
            Set multi_sized = filterSet(moreThanOne, dom);
            res = cup(res, multi_sized);
          }
        }
      }
    }
  }

  return res;
}

void SBGMatching::shortPathDirection(Set endings, Direction dir)
{
  PWMap auxB = restrict(paths_edges(), mapB()), auxD = restrict(paths_edges(), mapD());

  set_smap(PWMap(V()));
  set_rmap(PWMap(V()));

  Set reach_end = endings; // Vertices that reach endings
  Set P; // Vertices adjacent to reach_end
  unsigned int k = 0; // Current length of path

  do {
    Set pe = paths_edges();

    if (dir == forward) {
      auxB = restrict(pe, auxB);
      auxD = restrict(pe, auxD);
    }

    P = image(preImage(reach_end, auxD), auxB); // Start of edges entering reach_end
    P = difference(P, reach_end); // Take out vertices that are self-reps
    Set reach_edges = intersection(preImage(P, auxB), preImage(reach_end, auxD)); // Edges that reach reach_end
    PWMap starts = minInv(reach_edges, auxB); // Map from starts to the edges that take them to reach_end
    PWMap smap_reach = composition(auxD, starts);
    set_smap(combine(smap_reach, smap()));
    PWMap rmap_reach = composition(rmap(), smap_reach);
    set_rmap(combine(rmap_reach, rmap()));

    reach_end = cup(reach_end, P);
    if (dir == forward) {
      Set used_subset_edges = image(P, composition(Emap(), starts));
      set_paths_edges(difference(pe, preImage(used_subset_edges, Emap())));
    }
    k++;
  } while (!isEmpty(P) && k < nmbrSV(sbg()));

  return;
}

void SBGMatching::shortPathStep()
{
  // *** Forward direction
  shortPathDirection(unmatched_F(), forward);
  Set pe = getAllowedEdges();
  set_paths_edges(pe);

  // *** Backward direction
  PWMap auxB = mapB();
  set_mapB(mapD());
  set_mapD(auxB);

  shortPathDirection(unmatched_U(), backward);
  pe = intersection(pe, getAllowedEdges());
  set_paths_edges(pe);

  // *** Initial direction
  set_mapD(mapB());
  set_mapB(auxB);

  // *** Update structures to reflect new matched edges
  updatePaths(); 
 
  return;
}

void SBGMatching::shortPath()
{
  Set allowed_edges = difference(E(), getManyToOne());

  // Finish if all unknowns are matched, or there aren't any available edges left
  do {
    set_paths_edges(allowed_edges);
    shortPathStep();
  } while(!fullyMatchedU() && !isEmpty(paths_edges()));

  // In this case we only offset vertices here because shortPath isn't looking
  // for any minimum reachable vertex
  updateOffset();

  return;
}

// Minimum reachable -----------------------------------------------------------

PWMap SBGMatching::directedOffset(PWMap dir_map)
{
  Set unmatched_side = image(paths_edges(), dir_map);
  unmatched_side = intersection(unmatched_side, unmatched_V());
  Util::MD_NAT max_dirV = maxElem(image(omap()));
  PWMap res = restrict(unmatched_side, omap());
  res = offsetImage(max_dirV, res);

  return combine(res, omap());
}

DSBGraph SBGMatching::offsetGraph(PWMap dir_omap)
{
  Set _V = image(dir_omap);
  PWMap _Vmap = offsetDom(dir_omap, Vmap());

  PWMap _mapB = composition(dir_omap, mapB()), _mapD = composition(dir_omap, mapD());
  _mapB = restrict(paths_edges(), _mapB);
  _mapD = restrict(paths_edges(), _mapD);

  return DSBGraph(_V, _Vmap, _mapB, _mapD, restrict(paths_edges(), Emap()));
}

void SBGMatching::directedMinReach(PWMap dir_map)
{
  PWMap dir_omap = directedOffset(dir_map);
  DSBGraph dsbg = offsetGraph(dir_omap);
  MinReach min_reach(dsbg);
  PathInfo res = min_reach.calculate(image(unmatched_V(), dir_omap));

  PWMap aux_omap = combine(dir_omap, omap()), to_normal = minInv(aux_omap);
  PWMap aux_succs = composition(res.succs(), aux_omap);
  set_smap(composition(to_normal, aux_succs));
  PWMap aux_reps = composition(res.reps(), aux_omap);
  set_rmap(composition(to_normal, aux_reps));
}

void SBGMatching::minReachableStep()
{
  // *** Forward direction
  directedMinReach(mapU());
  Set pe = getAllowedEdges();
  Set reach_unmatched = preImage(unmatched_V(), rmap()); 
  reach_unmatched = preImage(reach_unmatched, mapB());
  set_paths_edges(intersection(pe, reach_unmatched));

  // *** Backward direction
  PWMap auxB = mapB();
  set_mapB(mapD());
  set_mapD(auxB);

  directedMinReach(mapF());
  pe = intersection(paths_edges(), getAllowedEdges());

  // *** Leave edges that connect unmatched left vertices with unmatched right vertices 
  reach_unmatched = preImage(unmatched_V(), rmap()); 
  set_rmap(restrict(reach_unmatched, rmap()));
  set_smap(restrict(reach_unmatched, smap()));

  pe = intersection(pe, preImage(reach_unmatched, mapB()));
  pe = intersection(pe, preImage(reach_unmatched, mapD()));
  set_paths_edges(pe);

  // *** Initial direction
  set_mapD(mapB());
  set_mapB(auxB);

  // *** Update structures to reflect new matched edges
  updatePaths(); 

  // *** Offset matched vertices
  updateOffset();

  return;
}

void SBGMatching::minReachable()
{
  do {
    set_paths_edges(E());
    minReachableStep();
  } while (!fullyMatchedU() && !isEmpty(paths_edges()));

  return;
}

// Matching --------------------------------------------------------------------

MatchInfo::MatchInfo() : matched_edges_(), fully_matchedU_() {}
MatchInfo::MatchInfo(Set matched_edges, bool fully_matchedU) : matched_edges_(matched_edges), fully_matchedU_(fully_matchedU) {}

member_imp(MatchInfo, Set, matched_edges);
member_imp(MatchInfo, bool, fully_matchedU);

std::ostream &operator<<(std::ostream &out, const MatchInfo &m_info)
{
  out << m_info.matched_edges();
  if (m_info.fully_matchedU()) out << " [FULLY MATCHED]";

   else out << " [UNMATCHED]";

  return out;
}

SBGMatching::SBGMatching() : sbg_(), V_(), Vmap_(), E_(), Emap_(), smap_(), rmap_(), omap_(),
  max_V_(), F_(), U_(), mapF_(), mapU_(), mapB_(), mapD_(), paths_edges_(), matched_E_(),
  unmatched_E_(), matched_V_(), unmatched_V_(), unmatched_F_(), matched_U_(), unmatched_U_() {}
SBGMatching::SBGMatching(SBGraph sbg) : sbg_(sbg), V_(sbg.V()), Vmap_(sbg.Vmap()),
  Emap_(sbg.Emap()) {
  set_E(dom(Emap()));

  PWMap id_vertex(V());
  set_smap(id_vertex);
  set_rmap(id_vertex);

  set_omap(id_vertex);
  set_max_V(maxElem(V()));

  set_F(image(sbg.map1()));
  set_U(image(sbg.map2()));
  set_mapF(sbg.map1());
  set_mapU(sbg.map2());

  set_mapB(sbg.map2());
  set_mapD(sbg.map1());

  set_paths_edges(E());
  set_matched_E(Set());
  set_unmatched_E(E());

  set_matched_V(Set());
  set_unmatched_V(V());
  set_unmatched_F(F());
  set_matched_U(Set());
  set_unmatched_U(U());
}

member_imp(SBGMatching, SBGraph, sbg);
member_imp(SBGMatching, Set, V);
member_imp(SBGMatching, PWMap, Vmap);
member_imp(SBGMatching, Set, E);
member_imp(SBGMatching, PWMap, Emap);

member_imp(SBGMatching, PWMap, smap);
member_imp(SBGMatching, PWMap, rmap);

member_imp(SBGMatching, PWMap, omap);
member_imp(SBGMatching, Util::MD_NAT, max_V);

member_imp(SBGMatching, Set, F);
member_imp(SBGMatching, Set, U);
member_imp(SBGMatching, PWMap, mapF);
member_imp(SBGMatching, PWMap, mapU);

member_imp(SBGMatching, PWMap, mapB);
member_imp(SBGMatching, PWMap, mapD);

member_imp(SBGMatching, Set, paths_edges);
member_imp(SBGMatching, Set, matched_E);
member_imp(SBGMatching, Set, unmatched_E);

member_imp(SBGMatching, Set, matched_V);
member_imp(SBGMatching, Set, unmatched_V);
member_imp(SBGMatching, Set, unmatched_F);
member_imp(SBGMatching, Set, matched_U);
member_imp(SBGMatching, Set, unmatched_U);

Set SBGMatching::getAllowedEdges()
{
  Set succs = image(filterMap(notEqId, smap())); // Vertices that are successor of other vertices in a path
  Set ending_edges = preImage(succs, mapD()); // Edges whose endings are successors 
  PWMap auxB = restrict(ending_edges, mapB()); // Map from a 'successor' edge to its start
  PWMap map_succs = composition(smap(), auxB); // Map from edge to the successor of its start
  
  return equalImage(map_succs, mapD()); // Edges that connect vertices with their successors
}

bool SBGMatching::fullyMatchedU() { return isEmpty(difference(U(), matched_U())); }

void SBGMatching::updatePaths()
{
  // *** Revert match and unmatched edges in augmenting paths
  PWMap paths_mapB = restrict(paths_edges(), mapB()), paths_mapD = restrict(paths_edges(), mapD());
  set_mapB(combine(paths_mapD, mapB()));
  set_mapD(combine(paths_mapB, mapD()));

  // *** Update matched edges
  set_matched_E(preImage(U(), mapD()));
  set_unmatched_E(preImage(F(), mapD()));

  // *** Update matched vertices
  Set matchedB = image(matched_E(), mapB()), matchedD = image(matched_E(), mapD());
  set_matched_V(cup(matchedB, matchedD));
  set_matched_U(intersection(U(), matched_V()));

  set_unmatched_V(difference(V(), matched_V()));
  set_unmatched_F(intersection(unmatched_V(), F()));
  set_unmatched_U(intersection(unmatched_V(), U()));

  return;
}

void SBGMatching::updateOffset()
{
  PWMap aux = restrict(matched_V(), omap());
  aux = offsetImage(max_V(), aux);  
  aux = offsetImage(max_V(), aux);  
  set_omap(combine(aux, omap()));

  return;
}

MatchInfo SBGMatching::calculate()
{
  auto begin = std::chrono::high_resolution_clock::now();
  shortPath();
  std::cout << "shortPath: " << matched_E() << "\n";

  if (!fullyMatchedU()) minReachable();
  std::cout << "minReachable: " << matched_E() << "\n\n";
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
  std::cout << "Exec time: " << duration.count() << " [ms]\n";

  std::cout << MatchInfo(matched_E(), fullyMatchedU()) << "\n\n";
  return MatchInfo(matched_E(), fullyMatchedU());
}

} // namespace LIB

} // namespace SBG
