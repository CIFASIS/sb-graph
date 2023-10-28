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

// -----------------------------------------------------------------------------
// Minimum reachable -----------------------------------------------------------
// -----------------------------------------------------------------------------

template<typename Set>
PathInfo<Set>::PathInfo() : succs_(), reps_() {}
template<typename Set>
PathInfo<Set>::PathInfo(PWMap<Set> succs, PWMap<Set> reps) : succs_(succs), reps_(reps) {}

member_imp_temp(template<typename Set>, PathInfo<Set>, PWMap<Set>, succs);
member_imp_temp(template<typename Set>, PathInfo<Set>, PWMap<Set>, reps);

template<typename Set>
bool eqId(SBGMap<Set> sbgmap) { return image(sbgmap) == sbgmap.dom(); }

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
  Set not_updated = equalImage(vto_rep, new_vto_rep);

  res = restrict(not_updated, smap);
  res = combine(res, new_smap);

  return res;
}

template<typename Set>
PWMap<Set> minReach1(Set V, Set E, PWMap<Set> mapB, PWMap<Set> mapD, PWMap<Set> smap, PWMap<Set> rmap)
{
  SetPiece mi_zero(V[0].size(), Interval(0));
  Set zero(mi_zero);

  PWMap<Set> auto_succs = filterMap(eqId, smap);
  Set auto_reps = equalImage(auto_succs, rmap);
  Set usable_verts = cup(auto_reps, dom(filterMap(notEqId, smap)));

  PWMap<Set> auxB = restrict(E, mapB), auxD = restrict(E, mapD); 
  PWMap<Set> adj_smap = minAdjMap(auxB, auxD, rmap); // Get minimum adjacent vertex with minimum representative
  PWMap new_smap = minMap(rmap, adj_smap, restrict(usable_verts, smap), rmap); // Get minimum between current rep and adjacent reps
  new_smap = updateMap(V, smap, new_smap, rmap); // Update rep iff new is less than current
  new_smap = combine(new_smap, adj_smap); 
  new_smap = combine(new_smap, smap); 

  return new_smap;
}

// Handle ONE recursion
template<typename Set>
PathInfo<Set> MinReach<Set>::recursion(unsigned int n, Set ER, Set rv, PWMap<Set> smap, PWMap<Set> rmap)
{
  PWMap<Set> mapB = dsbg().mapB(), mapD = dsbg().mapD(), Emap = dsbg().Emap(), Vmap = dsbg().Vmap();
  PWMap<Set> ERB = restrict(ER, mapB), ERD = restrict(ER, mapD);

  Set start = image(ERB), end = image(ERD), aux_start = start;
  start = difference(start, end), end = difference(end, aux_start);

  Set ER_plus = preImage(image(ER, Emap), Emap);
  Set VR_plus = cup(image(ER_plus, mapB), image(ER_plus, mapD));


  PWMap<Set> new_smap;
  Set repeated = intersection(start, end);
  Set available_vertices = dsbg().V();
  while (isEmpty(repeated)) {
    Set side = intersection(VR_plus, preImage(image(start, Vmap), Vmap));
    Set ER_side = preImage(start, mapB);
    Set ER_plus_side = preImage(image(ER_side, Emap), Emap);

    PWMap<Set> sideB  = restrict(ER_plus_side, mapB), sideD = restrict(ER_plus_side, mapD);
    PWMap<Set> ith = restrict(available_vertices, composition(sideD, minInv(sideB)));
    new_smap = combine(ith, new_smap);
 
    start = image(start, smap);
    repeated = intersection(start, end);
    available_vertices = difference(available_vertices, side);
  }

  new_smap = combine(restrict(rv, smap), new_smap);
  new_smap = combine(new_smap, smap);
  PWMap<Set> rmap_plus = composition(rmap, mapInf(new_smap, n)); // Solve recursion
  PWMap<Set> new_rmap = minMap(rmap, rmap_plus);
  new_rmap = combine(new_rmap, rmap);

  return PathInfo<Set>(new_smap, new_rmap);
}

template<typename Set>
MinReach<Set>::MinReach() : dsbg_() {}
template<typename Set>
MinReach<Set>::MinReach(DSBGraph<Set> sbg) : dsbg_(sbg) {}

member_imp_temp(template<typename Set>, MinReach<Set>, DSBGraph<Set>, dsbg);

template<typename Set>
PathInfo<Set> MinReach<Set>::calculate(Set unmatched_V)
{
  DSBGraph dg = dsbg();

  if (!isEmpty(dg.V())) {
    Set V = dg.V(), E = dg.E();
    PWMap<Set> mapB = dg.mapB(), mapD = dg.mapD(), Emap = dg.Emap();

    SetPiece mi_zero(V[0].size(), Interval(0));
    Set zero(mi_zero);

    PWMap<Set> old_smap, old_semap; // Old vertex and edge successors maps
    PWMap<Set> new_smap(V), new_semap(E); // New vertex and edge successors maps
    PWMap<Set> new_rmap(V); // New vertex reps map

    Set Vc; // Vertices that changed sucessor
    Set Ec; // Edges that changed successor

    Set unmatched_D = intersection(unmatched_V, image(mapD));
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

      if (!isEmpty(Vc)) {
        new_rmap = mapInf(new_smap); // Get minimum reachable following path

        Set E_succ = equalImage(mapD, composition(new_smap, mapB));
        if (!isEmpty(E_succ)) { // A new path was chosen
          old_semap = new_semap;
          PWMap<Set> mapB_succ = restrict(E_succ, mapB), mapD_succ = restrict(E_succ, mapD);
          new_semap = composition(minInv(E_succ, mapB_succ), mapD_succ);
          new_semap = combine(new_semap, old_semap);

          Set not_changed = equalImage(new_semap, old_semap);
          Ec = difference(E, not_changed);    

          Set ER; // Recursive edges that changed its successor
          Set visited_edges = cup(dom(new_semap), image(new_semap));
          Set old_subset_edges, visited_subset_edges = image(visited_edges, Emap);
          Set repeated_subset_edges;
          PWMap<Set> semap_nth = composition(new_semap, new_semap);
          unsigned int j = 0;
          do { // The max depth of recursion is the number of SVs
            ER = equalImage(Emap, composition(Emap, filterMap(notEqId, semap_nth))); 
            semap_nth = composition(semap_nth, new_semap); 

            visited_edges = image(visited_edges, new_semap);
            old_subset_edges = visited_subset_edges;
            visited_subset_edges = image(visited_edges, Emap); 
            repeated_subset_edges = intersection(old_subset_edges, visited_subset_edges);
            j++;
          } while(isEmpty(ER) && isEmpty(repeated_subset_edges));
          ER = intersection(ER, Ec);

          semap_nth = new_semap;
          for (unsigned int k = 0; k < j; k++) {
            ER = cup(ER, image(ER, semap_nth));
            semap_nth = composition(semap_nth, new_semap); 
          }

          if (!isEmpty(ER)) { // There are recursions, lets handle one of them
            PathInfo<Set> res = recursion(j, ER, reach_vertices, new_smap, new_rmap);
            new_smap = res.succs();
            new_rmap = res.reps();
          }
        }
      }
    } while (!isEmpty(Vc));

    return PathInfo<Set>(new_smap, new_rmap);
  }


  return PathInfo<Set>();
}

// -----------------------------------------------------------------------------
// Matching --------------------------------------------------------------------
// -----------------------------------------------------------------------------

// Short path ------------------------------------------------------------------

bool moreThanOne(SetPiece mdi) { return cardinal(mdi) > 1; }

template<typename Set>
Set SBGMatching<Set>::getManyToOne()
{
  Set res;

  BOOST_FOREACH (SBGMap sbgmapB, mapB_ref()) {
    BOOST_FOREACH (SBGMap sbgmapD, mapD_ref()) {
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

template<typename Set>
void SBGMatching<Set>::shortPathDirection(Set endings, Direction dir)
{
  PWMap<Set> auxB = restrict(paths_edges(), mapB()), auxD = restrict(paths_edges(), mapD());

  set_smap(PWMap<Set>(V()));
  set_rmap(PWMap<Set>(V()));

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
    PWMap<Set> starts = minInv(reach_edges, auxB); // Map from starts to the edges that take them to reach_end
    PWMap<Set> smap_reach = composition(auxD, starts);
    set_smap(combine(smap_reach, smap()));
    PWMap<Set> rmap_reach = composition(rmap(), smap_reach);
    set_rmap(combine(rmap_reach, rmap()));

    if (dir == forward) {
      reach_end = cup(reach_end, P);
      Set used_subset_edges = image(P, composition(Emap(), starts)); 
      set_paths_edges(difference(pe, preImage(used_subset_edges, Emap())));
    }
    k++;
  } while (!isEmpty(reach_end) && k < nmbrSV(sbg()));

  return;
}

template<typename Set>
void SBGMatching<Set>::shortPathStep()
{
  // *** Forward direction
  shortPathDirection(unmatched_F(), forward);
  Set pe = getAllowedEdges();
  set_paths_edges(pe);

  // *** Backward direction
  PWMap<Set> auxB = mapB();
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

template<typename Set>
void SBGMatching<Set>::shortPath()
{
  set_paths_edges(difference(E(), getManyToOne()));

  // Finish if all unknowns are matched, or there aren't any available edges left
  do { 
    shortPathStep();
  } while(!fullyMatchedU() && !isEmpty(paths_edges())); 

  // In this case we only offset vertices here because shortPath isn't looking
  // for any minimum reachable vertex
  updateOffset();

  return;
}

// Minimum reachable -----------------------------------------------------------

template<typename Set>
PWMap<Set> SBGMatching<Set>::directedOffset(PWMap<Set> dir_map)
{
  Set unmatched_side = image(paths_edges(), dir_map);
  unmatched_side = intersection(unmatched_side, unmatched_V());
  PWMap<Set> res = restrict(unmatched_side, omap());
  res = offsetImage(max_V(), res);

  return combine(res, omap());
}

template<typename Set>
DSBGraph<Set> SBGMatching<Set>::offsetGraph(PWMap<Set> dir_omap)
{
  Set _V = image(dir_omap);
  PWMap<Set> _Vmap = offsetDom(dir_omap, Vmap());

  PWMap<Set> _mapB = composition(dir_omap, mapB()), _mapD = composition(dir_omap, mapD());
  _mapB = restrict(paths_edges(), _mapB);
  _mapD = restrict(paths_edges(), _mapD);

  return DSBGraph<Set>(_V, _Vmap, _mapB, _mapD, restrict(paths_edges(), Emap()));
}

template<typename Set>
void SBGMatching<Set>::directedMinReach(PWMap<Set> dir_map)
{
  PWMap<Set> dir_omap = directedOffset(dir_map);
  DSBGraph<Set> dsbg = offsetGraph(dir_omap);
  MinReach min_reach(dsbg);
  PathInfo<Set> res = min_reach.calculate(unmatched_V());

  PWMap<Set> aux_omap = combine(dir_omap, omap()), to_normal = minInv(aux_omap);
  PWMap<Set> aux_succs = composition(res.succs(), aux_omap);
  set_smap(composition(to_normal, aux_succs));
  PWMap<Set> aux_reps = composition(res.reps(), aux_omap);
  set_rmap(composition(to_normal, aux_reps));
}

template<typename Set>
void SBGMatching<Set>::minReachableStep()
{
  // *** Forward direction
  directedMinReach(mapU());
  Set pe = getAllowedEdges();
  Set reach_unmatched = preImage(unmatched_V(), rmap()); 
  reach_unmatched = preImage(reach_unmatched, mapB());
  set_paths_edges(intersection(pe, reach_unmatched));

  // *** Backward direction
  PWMap<Set> auxB = mapB();
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

template<typename Set>
void SBGMatching<Set>::minReachable()
{
  do {
    set_paths_edges(E());
    minReachableStep();
  } while (!fullyMatchedU() && !isEmpty(paths_edges()));

  return;
}

// Matching --------------------------------------------------------------------

template<typename Set>
MatchInfo<Set>::MatchInfo() : matched_edges_(), fully_matchedU_() {}
template<typename Set>
MatchInfo<Set>::MatchInfo(Set matched_edges, bool fully_matchedU) : matched_edges_(matched_edges), fully_matchedU_(fully_matchedU) {}

member_imp_temp(template<typename Set>, MatchInfo<Set>, Set, matched_edges);
member_imp_temp(template<typename Set>, MatchInfo<Set>, bool, fully_matchedU);

template<typename Set>
std::ostream &operator<<(std::ostream &out, const MatchInfo<Set> &m_info)
{
  out << m_info.matched_edges();
  if (m_info.fully_matchedU()) out << " [FULLY MATCHED]";

   else out << " [UNMATCHED]";

  return out;
}

template<typename Set>
SBGMatching<Set>::SBGMatching() : sbg_(), V_(), Vmap_(), E_(), Emap_(), smap_(), rmap_(), omap_(),
  max_V_(), F_(), U_(), mapF_(), mapU_(), mapB_(), mapD_(), paths_edges_(), matched_E_(),
  unmatched_E_(), matched_V_(), unmatched_V_(), unmatched_F_(), matched_U_(), unmatched_U_() {}
template<typename Set>
SBGMatching<Set>::SBGMatching(SBGraph<Set> sbg) : sbg_(sbg), V_(sbg.V()), Vmap_(sbg.Vmap()),
  Emap_(sbg.Emap()) {
  set_E(dom(Emap()));

  PWMap<Set> id_vertex(V());
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

member_imp_temp(template<typename Set>, SBGMatching<Set>, SBGraph<Set>, sbg);
member_imp_temp(template<typename Set>, SBGMatching<Set>, Set, V);
member_imp_temp(template<typename Set>, SBGMatching<Set>, PWMap<Set>, Vmap);
member_imp_temp(template<typename Set>, SBGMatching<Set>, Set, E);
member_imp_temp(template<typename Set>, SBGMatching<Set>, PWMap<Set>, Emap);

member_imp_temp(template<typename Set>, SBGMatching<Set>, PWMap<Set>, smap);
member_imp_temp(template<typename Set>, SBGMatching<Set>, PWMap<Set>, rmap);

member_imp_temp(template<typename Set>, SBGMatching<Set>, PWMap<Set>, omap);
member_imp_temp(template<typename Set>, SBGMatching<Set>, Util::MD_NAT, max_V);

member_imp_temp(template<typename Set>, SBGMatching<Set>, Set, F);
member_imp_temp(template<typename Set>, SBGMatching<Set>, Set, U);
member_imp_temp(template<typename Set>, SBGMatching<Set>, PWMap<Set>, mapF);
member_imp_temp(template<typename Set>, SBGMatching<Set>, PWMap<Set>, mapU);

member_imp_temp(template<typename Set>, SBGMatching<Set>, PWMap<Set>, mapB);
member_imp_temp(template<typename Set>, SBGMatching<Set>, PWMap<Set>, mapD);

member_imp_temp(template<typename Set>, SBGMatching<Set>, Set, paths_edges);
member_imp_temp(template<typename Set>, SBGMatching<Set>, Set, matched_E);
member_imp_temp(template<typename Set>, SBGMatching<Set>, Set, unmatched_E);

member_imp_temp(template<typename Set>, SBGMatching<Set>, Set, matched_V);
member_imp_temp(template<typename Set>, SBGMatching<Set>, Set, unmatched_V);
member_imp_temp(template<typename Set>, SBGMatching<Set>, Set, unmatched_F);
member_imp_temp(template<typename Set>, SBGMatching<Set>, Set, matched_U);
member_imp_temp(template<typename Set>, SBGMatching<Set>, Set, unmatched_U);

template<typename Set>
Set SBGMatching<Set>::getAllowedEdges()
{
  Set succs = image(filterMap(notEqId, smap())); // Vertices that are successor of other vertices in a path
  Set ending_edges = preImage(succs, mapD()); // Edges whose endings are successors 
  PWMap<Set> auxB = restrict(ending_edges, mapB()); // Map from a 'successor' edge to its start
  PWMap<Set> map_succs = composition(smap(), auxB); // Map from edge to the successor of its start
  
  return equalImage(map_succs, mapD()); // Edges that connect vertices with their successors
}

template<typename Set>
bool SBGMatching<Set>::fullyMatchedU() { return isEmpty(difference(U(), matched_U())); }

template<typename Set>
void SBGMatching<Set>::updatePaths()
{
  // *** Revert match and unmatched edges in augmenting paths
  PWMap<Set> paths_mapB = restrict(paths_edges(), mapB()), paths_mapD = restrict(paths_edges(), mapD());
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

template<typename Set>
void SBGMatching<Set>::updateOffset()
{
  PWMap<Set> aux = restrict(matched_V(), omap());
  aux = offsetImage(max_V(), aux);  
  aux = offsetImage(max_V(), aux);  
  set_omap(combine(aux, omap()));

  return;
}

template<typename Set>
MatchInfo<Set> SBGMatching<Set>::calculate()
{
  shortPath();

  if (!fullyMatchedU()) minReachable();
  
  return MatchInfo(matched_E(), fullyMatchedU());
}

// Template instantiations -----------------------------------------------------

template BasePWMap connectedComponents<UnordSet>(BaseSBG g);
template CanonPWMap connectedComponents<OrdSet>(CanonSBG g);

template struct PathInfo<UnordSet>;
template struct PathInfo<OrdSet>;

template struct MinReach<UnordSet>;
template struct MinReach<OrdSet>;

template struct MatchInfo<UnordSet>;
template struct MatchInfo<OrdSet>;

template struct SBGMatching<UnordSet>;
template struct SBGMatching<OrdSet>;

} // namespace LIB

} // namespace SBG
