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
  if (!g.V().isEmpty()) {
    unsigned int dims = g.V().begin()->size();
    SBGMap<Set> id(g.V(), Exp(dims));
    PWMap<Set> rmap(id), old_rmap;

    if (g.E().isEmpty())
      return rmap;

    do {
      old_rmap = rmap;

      PWMap<Set> ermap1 = rmap.composition(g.map1());
      PWMap<Set> ermap2 = rmap.composition(g.map2());

      PWMap<Set> rmap1 = ermap1.minAdjMap(ermap2);
      PWMap<Set> rmap2 = ermap2.minAdjMap(ermap1);
      rmap1 = rmap1.combine(rmap);
      rmap2 = rmap2.combine(rmap);

      PWMap aux_rmap = rmap1.minMap(rmap2);
      rmap = rmap.minMap(aux_rmap);

      if (!(rmap == old_rmap)) {
        rmap = aux_rmap;
        rmap = rmap.mapInf();
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
PathInfo<Set>::PathInfo(PWMap<Set> succs, PWMap<Set> reps)
  : succs_(succs), reps_(reps) {}

member_imp_temp(template<typename Set>, PathInfo<Set>, PWMap<Set>, succs);
member_imp_temp(template<typename Set>, PathInfo<Set>, PWMap<Set>, reps);

template<typename Set>
bool eqId(const SBGMap<Set> &sbgmap) { return sbgmap.isId(); }

template<typename Set>
bool notEqId(const SBGMap<Set> &sbgmap) { return !(sbgmap.isId()); }

// Update rep iff new is less than current. Without this function, if there are
// vertices v1, v2, v3 and edges v1->v2, v1->v3 where rmap(v2) = rmap(v3) the
// algorithm could switch infinitely between those two reps.
template<typename Set>
PWMap<Set> updateMap(const Set &V, const PWMap<Set> &smap
                     , const PWMap<Set> &new_smap, const PWMap<Set> &rmap)
{
  PWMap<Set> res;

  if (!V.isEmpty()) {
    PWMap<Set> vto_rep = rmap.composition(smap);
    PWMap<Set> new_vto_rep = rmap.composition(new_smap);
    Set not_updated = vto_rep.equalImage(new_vto_rep);

    res = smap.restrict(not_updated);
    res = res.combine(new_smap);
  }

  return res;
}

template<typename Set>
MinReach<Set>::MinReach() : dsbg_() {}
template<typename Set>
MinReach<Set>::MinReach(DSBGraph<Set> sbg) : dsbg_(sbg) {}

member_imp_temp(template<typename Set>, MinReach<Set>, DSBGraph<Set>, dsbg);

template<typename Set>
PWMap<Set> MinReach<Set>::minReach1(
  const Set &reach, const PW &smap, const PW &rmap
) const
{
  DSBGraph<Set> dg = dsbg_;
  Set V = dg.V();
  PW mapB = dg.mapB(), mapD = dg.mapD();

  PW auto_succs = smap.filterMap([](const SBGMap<Set> &sbgmap) {
    return eqId(sbgmap);
  });
  Set auto_reps = auto_succs.equalImage(rmap);
  Set not_auto_succs = smap.filterMap([](const SBGMap<Set> &sbgmap) {
    return notEqId(sbgmap);
  }).dom();
  Set usable_verts = auto_reps.cup(not_auto_succs);

  PWMap auxB = mapB.restrict(reach), auxD = mapD.restrict(reach);
  // Get minimum adjacent vertex with minimum representative
  PWMap adj_smap = auxB.minAdjMap(auxD, rmap);
  // Get minimum between current rep and adjacent reps
  PWMap new_smap = rmap.minMap(adj_smap, smap.restrict(usable_verts), rmap);
  // Update rep iff new is less than current
  new_smap = updateMap(V, smap, new_smap, rmap);
  new_smap = new_smap.combine(adj_smap);
  new_smap = new_smap.combine(smap);

  return new_smap;
}

// Handle ONE recursion
template<typename Set>
PathInfo<Set> MinReach<Set>::recursion(
  unsigned int n, const Set &ER, const Set &rv
  , const PW &semap, const PW &smap, const PW &rmap
) const
{
  PW mapB = dsbg().mapB(), mapD = dsbg().mapD()
     , Vmap = dsbg().Vmap(), Emap = dsbg().Emap(), subE_map = dsbg().subE_map();
  PW ERB = mapB.restrict(ER), ERD = mapD.restrict(ER);

  Set start = ERB.image(), end = ERD.image(), aux_start = start;
  start = start.difference(end), end = end.difference(aux_start);

  Set ER_plus = Emap.preImage(Emap.image(ER));
  Set VR_plus = mapB.image(ER_plus).cup(mapD.image(ER_plus));

  PW new_smap;
  Set repeated = start.intersection(end);
  Set available_vertices = dsbg().V();
  while (repeated.isEmpty()) {
    Set ER_side = mapB.preImage(start);
    Set ER_plus_side = subE_map.preImage(subE_map.image(ER_side));

    PW sideB  = mapB.restrict(ER_plus_side)
      , sideD = mapD.restrict(ER_plus_side);
    PW ith = sideD.composition(sideB.minInv()).restrict(available_vertices);
    new_smap = ith.combine(new_smap);
 
    Set side = VR_plus.intersection(Vmap.preImage(Vmap.image(start)));
    available_vertices = available_vertices.difference(side);
    start = smap.image(start);
    repeated = start.intersection(end);
  }

  new_smap = smap.restrict(rv).combine(new_smap);
  new_smap = new_smap.combine(smap);
  // Solve recursion
  PW rmap_plus = rmap.composition(new_smap.mapInf(n));
  PW new_rmap = rmap.minMap(rmap_plus);
  new_rmap = new_rmap.combine(rmap);

  return PathInfo<Set>(new_smap, new_rmap);
}

template<typename Set>
PathInfo<Set> MinReach<Set>::calculate(const Set &unmatched_V) const
{
  DSBGraph dg = dsbg();

  if (!dg.V().isEmpty()) {
    Set V = dg.V(), E = dg.E();
    PW mapB = dg.mapB(), mapD = dg.mapD(), Emap = dg.Emap();

    PW old_smap, old_semap; // Old vertex and edge successors maps
    PW new_smap(V), new_semap(E); // New vertex and edge successors maps
    PW new_rmap(V); // New vertex reps map

    Set Vc; // Vertices that changed sucessor
    Set Ec; // Edges that changed successor

    Set unmatched_D = unmatched_V.intersection(mapD.image())
        , unmatched_B = unmatched_V.intersection(mapB.image());
    Set reach_vertices = unmatched_D;
    Set reach_edges;

    // If a path changes, we have to check if this allows new vertices to
    // reach a new rep. The condition of the loop corresponds to the fact
    // that paths are uniquely defined by the sequence of vertices they traverse
    do {
      reach_edges = mapD.preImage(reach_vertices);
      reach_vertices = reach_vertices.cup(mapB.image(reach_edges));

      old_smap = new_smap;
      // Find adjacent vertex that reaches a minor vertex than the current one
      new_smap = minReach1(reach_edges, new_smap, new_rmap); 
      Vc = V.difference(old_smap.equalImage(new_smap));

      // If the condition is met, unmatched "backward" vertices reach unmatched
      // "forward" vertices
      if (!reach_vertices.intersection(unmatched_B).isEmpty()) {
        new_rmap = new_smap.mapInf(); // Get minimum reachable following path
        return PathInfo<Set>(new_smap, new_rmap);
      }

      if (!Vc.isEmpty()) {
        new_rmap = new_smap.mapInf(); // Get minimum reachable following path

        Set E_succ = mapD.equalImage(new_smap.composition(mapB));
        if (!E_succ.isEmpty()) { // A new path was chosen
          old_semap = new_semap;
          PW mapB_succ = mapB.restrict(E_succ)
                     , mapD_succ = mapD.restrict(E_succ);
          new_semap = mapB_succ.minInv(E_succ).composition(mapD_succ);
          new_semap = new_semap.combine(old_semap);

          Set not_changed = new_semap.equalImage(old_semap);
          Ec = E.difference(not_changed);

          if (!Ec.isEmpty()) {
            Set ER;  // Recursive edges that changed its successor
            PW old_semap_nth , semap_nth = new_semap.composition(new_semap);
            unsigned int j = 0;
            do { // The max depth of recursion is the number of SVs
              PW other = semap_nth.filterMap([](const SBGMap<Set> &sbgmap) {
                return notEqId(sbgmap);
              });
              ER = Emap.equalImage(Emap.composition(other));
              old_semap_nth = semap_nth;
              semap_nth = new_semap.composition(semap_nth);

              ++j;
            } while (ER.isEmpty() && j < dg.nmbrSV()
                     && !(old_semap_nth == semap_nth));
            ER = ER.intersection(Ec);

            semap_nth = new_semap;
            for (unsigned int k = 0; k < j; ++k) {
              ER = ER.cup(semap_nth.image(ER));
              semap_nth = new_semap.composition(semap_nth);
            }

            if (!ER.isEmpty()) { // There are recursions, lets handle one of them
              PathInfo<Set> res;
              res = recursion(j, ER, reach_vertices, new_semap, new_smap, new_rmap);
              new_smap = res.succs();
              new_rmap = res.reps();
            }
          }
        }
      }
    } while (!Vc.isEmpty());

    return PathInfo<Set>(new_smap, new_rmap);
  }

  return PathInfo<Set>();
}

// -----------------------------------------------------------------------------
// Matching --------------------------------------------------------------------
// -----------------------------------------------------------------------------

// Short path ------------------------------------------------------------------

bool moreThanOne(SetPiece mdi) { return mdi.cardinal() > 1; }

template<typename Set>
Set SBGMatching<Set>::getManyToOne() const
{
  Set res;

  for (const Map &sbgmapB : mapB()) {
    for (const Map &sbgmapD : mapD()) {
      Exp expB = sbgmapB.exp(), expD = sbgmapD.exp();
      if (expD.isConstant()) {
        Set dom = sbgmapB.dom().intersection(sbgmapD.dom());
        if (!dom.isEmpty()) {
          dom = dom.intersection(paths_edges());
          if (!dom.isEmpty()) {
            Set multi_sized = dom.filterSet([](const SetPiece &mdi) {
              return moreThanOne(mdi);
            });
            res = res.cup(multi_sized);
          }
        }
      }
    }
  }

  return res;
}

template<typename Set>
void SBGMatching<Set>::shortPathDirection(const Set &endings, Direction dir)
{
  PW auxB = mapB().restrict(paths_edges()), auxD = mapD().restrict(paths_edges());

  set_smap(PW(V_));
  set_rmap(PW(V_));

  Set reach_end = endings; // Vertices that reach endings
  Set P; // Vertices adjacent to reach_end
  unsigned int k = 0; // Current length of path

  do {
    Set pe = paths_edges();

    if (dir == forward) {
      auxB = auxB.restrict(pe);
      auxD = auxD.restrict(pe);
    }

    // Start of edges entering reach_end
    P = auxB.image(auxD.preImage(reach_end));
    P = P.difference(reach_end); // Take out vertices that are self-reps
    // Edges that reach reach_end
    Set reach_edges = auxB.preImage(P).intersection(auxD.preImage(reach_end));
    // Map from starts to the edges that take them to reach_end
    PW starts = auxB.minInv(reach_edges);
    PW smap_reach = auxD.composition(starts);
    set_smap(smap_reach.combine(smap()));
    PW rmap_reach = rmap().composition(smap_reach);
    set_rmap(rmap_reach.combine(rmap()));

    reach_end = reach_end.cup(P);
    k++;
  } while (!P.isEmpty() && k < sbg().nmbrSV());

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
  pe = pe.intersection(getAllowedEdges());
  set_paths_edges(pe);

  // *** Initial direction
  set_mapD(mapB());
  set_mapB(auxB);

  shortPathDirection(unmatched_F(), forward);
  pe = pe.intersection(getAllowedEdges());
  set_paths_edges(pe);

  // *** Update structures to reflect new matched edges
  updatePaths(); 
 
  return;
}

template<typename Set>
void SBGMatching<Set>::shortPath()
{
  Set allowed_edges = E().difference(getManyToOne());

  // Finish if all unknowns are matched or there aren't available edges
  do {
    set_paths_edges(allowed_edges);
    //Util::SBG_LOG << "short step smap: " << smap() << "\n\n";
    shortPathStep();
  } while(!fullyMatchedU() && !paths_edges().isEmpty());

  // In this case we only offset vertices here because shortPath isn't looking
  // for any minimum reachable vertex
  updateOffset();

  return;
}

// Minimum reachable -----------------------------------------------------------

template<typename Set>
PWMap<Set> SBGMatching<Set>::directedOffset(const PW &dir_map) const
{
  Set unmatched_side = dir_map.image(paths_edges());
  unmatched_side = unmatched_side.intersection(unmatched_V());
  PW res = omap().restrict(unmatched_side);
  res = res.offsetImage(max_V());

  return res.combine(omap());
}

template<typename Set>
DSBGraph<Set> SBGMatching<Set>::offsetGraph(const PW &dir_omap) const
{
  Set _V = dir_omap.image();
  PWMap<Set> _Vmap = Vmap().offsetDom(dir_omap);

  PWMap<Set> _mapB = dir_omap.composition(mapB())
            , _mapD = dir_omap.composition(mapD());
  _mapB = _mapB.restrict(paths_edges());
  _mapD = _mapD.restrict(paths_edges());

  return DSBGraph<Set>(_V, _Vmap, _mapB, _mapD, Emap().restrict(paths_edges()));
}

template<typename Set>
void SBGMatching<Set>::directedMinReach(const PW &dir_map)
{
  PW dir_omap = directedOffset(dir_map);
  DSBGraph<Set> dsbg = offsetGraph(dir_omap);
  MinReach min_reach(dsbg);
  PathInfo<Set> res = min_reach.calculate(dir_omap.image(unmatched_V()));

  PW aux_omap = dir_omap.combine(omap()), to_normal = aux_omap.minInv();
  PW aux_succs = res.succs().composition(aux_omap);
  set_smap(to_normal.composition(aux_succs));
  PW aux_reps = res.reps().composition(aux_omap);
  set_rmap(to_normal.composition(aux_reps));
}

template<typename Set>
void SBGMatching<Set>::minReachableStep()
{
  // *** Forward direction
  directedMinReach(mapU());
  Set pe = getAllowedEdges();
  Set reach_unmatched = rmap().preImage(unmatched_V());
  reach_unmatched = mapB().preImage(reach_unmatched);
  set_paths_edges(pe.intersection(reach_unmatched));

  // *** Backward direction
  PWMap<Set> auxB = mapB();
  set_mapB(mapD());
  set_mapD(auxB);

  directedMinReach(mapF());
  pe = paths_edges().intersection(getAllowedEdges());

  // *** Edges that connect unmatched left vertices with unmatched right ones 
  reach_unmatched = rmap().preImage(unmatched_V());
  set_rmap(rmap().restrict(reach_unmatched));
  set_smap(smap().restrict(reach_unmatched));

  pe = pe.intersection(mapB().preImage(reach_unmatched));
  pe = pe.intersection(mapD().preImage(reach_unmatched));
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
    //Util::SBG_LOG << "minimum reachable step smap: " << smap() << "\n\n";
    minReachableStep();
  } while (!fullyMatchedU() && !paths_edges().isEmpty());

  return;
}

// Matching --------------------------------------------------------------------

template<typename Set>
MatchInfo<Set>::MatchInfo() : matched_edges_(), fully_matchedU_() {}
template<typename Set>
MatchInfo<Set>::MatchInfo(Set matched_edges, bool fully_matchedU) 
  : matched_edges_(matched_edges), fully_matchedU_(fully_matchedU) {}

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
SBGMatching<Set>::SBGMatching() 
  : sbg_(), V_(), Vmap_(), E_(), Emap_(), smap_(), rmap_(), omap_()
    , max_V_(), F_(), U_(), mapF_(), mapU_(), mapB_(), mapD_(), paths_edges_()
    , matched_E_(), unmatched_E_(), matched_V_(), unmatched_V_(), unmatched_F_()
    , matched_U_(), unmatched_U_() {}
template<typename Set>
SBGMatching<Set>::SBGMatching(SBGraph<Set> sbg) 
  : sbg_(sbg), V_(sbg.V()), Vmap_(sbg.Vmap()), Emap_(sbg.Emap()) {
  set_E(Emap_.dom());

  PW id_vertex(V_);
  set_smap(id_vertex);
  set_rmap(id_vertex);

  set_omap(id_vertex);
  set_max_V(V_.maxElem());

  set_F(sbg.map1().image());
  set_U(sbg.map2().image());
  set_mapF(sbg.map1());
  set_mapU(sbg.map2());

  set_mapB(sbg.map2());
  set_mapD(sbg.map1());

  set_paths_edges(E_);
  set_matched_E(Set());
  set_unmatched_E(E_);

  set_matched_V(Set());
  set_unmatched_V(V_);
  set_unmatched_F(F_);
  set_matched_U(Set());
  set_unmatched_U(U_);
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
Set SBGMatching<Set>::getAllowedEdges() const
{
  // Vertices that are successor of other vertices in a path
  Set succs = smap().filterMap([](const SBGMap<Set> &sbgmap) {
    return notEqId(sbgmap);
  }).image();
  // Edges whose endings are successors 
  Set ending_edges = mapD().preImage(succs);
  // Map from a 'successor' edge to its start
  PWMap<Set> auxB = mapB().restrict(ending_edges);
  // Map from edge to the successor of its start
  PWMap<Set> map_succs = smap().composition(auxB);
 
  // Edges that connect vertices with their successors
  return map_succs.equalImage(mapD());
}

template<typename Set>
bool SBGMatching<Set>::fullyMatchedU() const
{
  return U().difference(matched_U()).isEmpty();
}

template<typename Set>
void SBGMatching<Set>::updatePaths()
{
  // *** Revert match and unmatched edges in augmenting paths
  PW paths_mapB = mapB().restrict(paths_edges())
     , paths_mapD = mapD().restrict(paths_edges());
  set_mapB(paths_mapD.combine(mapB()));
  set_mapD(paths_mapB.combine(mapD()));

  // *** Update matched edges
  set_matched_E(mapD().preImage(U()));
  set_unmatched_E(mapD().preImage(F()));

  // *** Update matched vertices
  Set matchedB = mapB().image(matched_E())
      , matchedD = mapD().image(matched_E());
  set_matched_V(matchedB.cup(matchedD));
  set_matched_U(U().intersection(matched_V()));

  set_unmatched_V(V().difference(matched_V()));
  set_unmatched_F(unmatched_V().intersection(F()));
  set_unmatched_U(unmatched_V().intersection(U()));

  return;
}

template<typename Set>
void SBGMatching<Set>::updateOffset()
{
  PWMap<Set> aux = omap().restrict(matched_V());
  aux = aux.offsetImage(max_V());
  aux = aux.offsetImage(max_V());
  set_omap(aux.combine(omap()));

  return;
}

template<typename Set>
MatchInfo<Set> SBGMatching<Set>::calculate()
{
  //Util::SBG_LOG << sbg() << "\n";
  auto begin = std::chrono::high_resolution_clock::now();
  shortPath();
  std::cout << "shortPath: " << matched_E() << "\n";

  if (!fullyMatchedU()) {
    minReachable();
    std::cout << "minReachable: " << matched_E() << "\n\n";
  }
  else
    std::cout << "\n";
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
    end - begin
  );
  std::cout << "Exec time: " << duration.count() << " [ms]\n";

  std::cout << MatchInfo(matched_E(), fullyMatchedU()) << "\n\n";
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
