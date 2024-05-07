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
MinReach<Set>::MinReach() : dsbg_(), debug_(false) {}
template<typename Set>
MinReach<Set>::MinReach(DSBGraph<Set> sbg, bool debug)
  : dsbg_(sbg), debug_(debug) {}

member_imp_temp(template<typename Set>, MinReach<Set>, DSBGraph<Set>, dsbg);
member_imp_temp(template<typename Set>, MinReach<Set>, bool, debug);

template<typename Set>
PathInfo<Set> MinReach<Set>::calculate(
  const Set &starts, const Set &endings
) const
{
  DSBGraph dg = dsbg();

  if (!dg.V().isEmpty()) {
    Set V = dg.V(), E = dg.E();
    PW mapB = dg.mapB(), mapD = dg.mapD(), Emap = dg.Emap();

    PW old_semap, old_rmap; // Old vertex and edge successors maps
    PW new_smap(V), new_semap(E); // New vertex and edge successors maps
    PW new_rmap(V); // New vertex reps map

    Set Vc; // Vertices that changed sucessor
    Set Ec; // Edges that changed successor

    // Vertices and edges that reach unmatched vertices
    Set reach_vertices = endings, reach_edges;

    // If a vertex changes its successor, then a new path was found. If it is
    // an augmenting one, a new matching can be obtained. That is why the loop
    // iterates until Vc is empty.
    do {
    } while (!Vc.isEmpty());

    return PathInfo<Set>(new_smap, new_rmap);
  }

  return PathInfo<Set>();
}

// -----------------------------------------------------------------------------
// Matching --------------------------------------------------------------------
// -----------------------------------------------------------------------------

// Short path ------------------------------------------------------------------

template<typename Set>
void SBGMatching<Set>::shortPathDirection(const Set &endings, Direction dir)
{
  PW auxB = mapB().restrict(paths_edges())
    , auxD = mapD().restrict(paths_edges());
  
  Set unmatched;
  if (dir == forward)
    unmatched = unmatched_U();
  else
    unmatched = unmatched_F();

  set_smap(PW(V_));
  set_rmap(PW(V_));

  Set reach_end = endings; // Vertices that reach endings
  Set P; // Vertices adjacent to reach_end

  unsigned int j = 0; // Current length of path
  do {
    // Start of edges entering reach_end
    P = auxB.image(auxD.preImage(reach_end));
    P = P.difference(reach_end); // Take out vertices that are self-reps
    // Edges that reach reach_end
    Set reach_edges = auxB.preImage(P).intersection(auxD.preImage(reach_end));
    // Map from starts to the edges that take them to reach_end
    PW starts = auxB.firstInv(reach_edges);
    PW smap_reach = auxD.composition(starts);
    set_smap(smap_reach.combine(smap()));
    PW rmap_reach = rmap().composition(smap_reach);
    set_rmap(rmap_reach.combine(rmap()));

    reach_end = reach_end.cup(P);
    j++;
  } while (!P.isEmpty() && P.intersection(unmatched).isEmpty() 
           && j < k());

  return;
}

template<typename Set>
void SBGMatching<Set>::shortPathStep()
{
  // *** Forward direction
  shortPathDirection(unmatched_F(), forward);
  PW rmapd = rmap();
  Set pe = edgesInPaths();
  set_paths_edges(pe);

  // *** Backward direction
  PWMap<Set> auxB = mapB();
  set_mapB(mapD());
  set_mapD(auxB);

  shortPathDirection(unmatched_U(), backward);
  const PW &rmaprmapd = rmap().composition(rmapd);
  Set edgesb = edgesSameRepLR(rmaprmapd);
  pe = pe.intersection(edgesInPaths().intersection(edgesb));
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
  // Finish if all unknowns are matched or there aren't available edges
  do {
    set_paths_edges(E());
    shortPathStep();
    if (debug()) {
      Util::SBG_LOG << "short step smap: " << smap() << "\n";
      Util::SBG_LOG << "short step matched edges: " << matched_E() << "\n";
      Util::SBG_LOG << "short step unmatched V: " << unmatched_V() << "\n\n";
    }
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
  if (debug())
    Util::SBG_LOG << "directedOffset unmatched_V: " << unmatched_V() << "\n\n";
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
  if (debug())
    Util::SBG_LOG << "dir_omap: " << dir_omap << "\n";
  DSBGraph<Set> dsbg = offsetGraph(dir_omap);
  dsbg.set_subE_map(sbg().subE_map().restrict(paths_edges()));
  MinReach min_reach(dsbg, debug());
  Set unmatched_B = mapB().image().intersection(unmatched_V());
  Set unmatched_D = mapD().image().intersection(unmatched_V());
  PathInfo<Set> res = min_reach.calculate(
    dir_omap.image(unmatched_B), dir_omap.image(unmatched_D)
  );

  PW aux_omap = dir_omap.combine(omap()), to_normal = aux_omap.inverse();
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
  PW rmapd = rmap();
  Set reach_unmatched = rmap().preImage(unmatched_F());
  Set pe = edgesInPaths();
  set_paths_edges(pe);

  // *** Backward direction
  PWMap<Set> auxB = mapB();
  set_mapB(mapD());
  set_mapD(auxB);

  directedMinReach(mapF());
  // Vertices that reach unmatched left and right vertices 
  reach_unmatched = reach_unmatched.intersection(rmap().preImage(unmatched_U()));
  set_rmap(rmap().restrict(reach_unmatched));
  set_smap(smap().restrict(reach_unmatched));

  const PW &rmaprmapd = rmap().composition(rmapd);
  Set edgesb = edgesSameRepLR(rmaprmapd);
  pe = pe.intersection(edgesInPaths().intersection(edgesb));
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
    if (debug()) {
      Util::SBG_LOG << "minimum reachable step smap: " << smap() << "\n";
      Util::SBG_LOG << "minimum reachable matched_E: " << matched_E() << "\n\n";
    }
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
  if (m_info.fully_matchedU())
    out << " [FULLY MATCHED]";

  else 
    out << " [UNMATCHED]";

  return out;
}

template<typename Set>
SBGMatching<Set>::SBGMatching() 
  : sbg_(), V_(), Vmap_(), E_(), Emap_(), smap_(), rmap_(), omap_()
    , max_V_(), F_(), U_(), mapF_(), mapU_(), mapB_(), mapD_(), paths_edges_()
    , matched_E_(), unmatched_E_(), matched_V_(), unmatched_V_(), unmatched_F_()
    , matched_U_(), unmatched_U_(), k_(6), debug_(false) {}
template<typename Set>
SBGMatching<Set>::SBGMatching(SBGraph<Set> sbg, bool debug)
  : sbg_(sbg), V_(sbg.V()), Vmap_(sbg.Vmap()), Emap_(sbg.Emap()), k_(6)
    , debug_(debug) {
  set_E(Emap_.dom());

  PW id_vertex(V_);
  smap_ = id_vertex;
  rmap_ = id_vertex;

  omap_ = id_vertex;
  max_V_ = V_.maxElem();

  F_ = sbg.map1().image();
  U_ = sbg.map2().image();
  mapF_ = sbg.map1();
  mapU_ = sbg.map2();

  mapB_ = sbg.map2();
  mapD_ = sbg.map1();

  paths_edges_ = E_;
  matched_E_ = Set();
  unmatched_E_ = E_;

  matched_V_ = Set();
  unmatched_V_ = V_;
  unmatched_F_ = F_;
  matched_U_ = Set();
  unmatched_U_ = U_;
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

member_imp_temp(template<typename Set>, SBGMatching<Set>, unsigned int, k);

member_imp_temp(template<typename Set>, SBGMatching<Set>, bool, debug);

template<typename Set>
Set SBGMatching<Set>::edgesInPaths() const
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
 
  return map_succs.equalImage(mapD());
}

template<typename Set>
Set SBGMatching<Set>::edgesSameRepLR(const PW &rmaprmapd) const
{
  PW rmap_neq_id = rmap().filterMap([](const SBGMap<Set> &sbgmap) {
    return notEqId(sbgmap);
  });
  PW rmapb = rmap_neq_id.composition(mapB());
  PW rmaprmapd_neq_id = rmaprmapd.filterMap([](const SBGMap<Set> &sbgmap) {
    return notEqId(sbgmap);
  }); 
  PW rmaprmapdb = rmaprmapd_neq_id.composition(mapB());
  
  return rmapb.equalImage(rmaprmapdb);
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
MatchInfo<Set> SBGMatching<Set>::calculate(unsigned int k)
{
  if (debug())
    Util::SBG_LOG << "Matching sbg: \n" << sbg() << "\n\n";
  set_k(k);

  auto begin = std::chrono::high_resolution_clock::now();
  shortPath();
  auto end = std::chrono::high_resolution_clock::now();
  auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(
    end - begin
  );

  if (debug())
    Util::SBG_LOG << "shortPath: " << matched_E() << "\n\n";

  bool mr_used = false;
  if (!fullyMatchedU()) {
    begin = std::chrono::high_resolution_clock::now();
    minReachable();
    end = std::chrono::high_resolution_clock::now();

    if (debug())
      Util::SBG_LOG << "minReachable: " << matched_E() << "\n\n";
    mr_used = true;
  }

  Util::SBG_LOG << MatchInfo(matched_E(), fullyMatchedU()) << "\n\n";

  Util::SBG_LOG << "ShortPath exec time: " << duration1.count() << " [μs]\n";
  if (mr_used) {
    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(
      end - begin
    );
    Util::SBG_LOG << "MinReach exec time: " << duration2.count() << " [μs]\n";

    auto total = std::chrono::duration_cast<std::chrono::microseconds>(
      duration1 + duration2
    );
    Util::SBG_LOG << "Total match exec time: " << total.count() << " [μs]\n\n"; 
  }
  else
    Util::SBG_LOG << "Total match exec time: " << duration1.count() << " [μs]\n\n"; 

  return MatchInfo(matched_E(), fullyMatchedU());
}

// -----------------------------------------------------------------------------
// SCC -------------------------------------------------------------------------
// -----------------------------------------------------------------------------

template<typename Set>
SBGSCC<Set>::SBGSCC() : dsbg_(), V_(), Vmap_(), E_(), Emap_(), Ediff_(), mapB_()
  , mapD_(), rmap_(), debug_(false) {}
template<typename Set>
SBGSCC<Set>::SBGSCC(DSBGraph<Set> dsbg, bool debug)
  : dsbg_(dsbg), debug_(debug) {
  V_ = dsbg.V();
  Vmap_ = dsbg.Vmap();
  
  E_ = dsbg.E();
  Emap_ = dsbg.Emap();

  mapB_ = dsbg.mapB();
  mapD_ = dsbg.mapD(); 

  Ediff_ = Set();

  rmap_ = PW(V_);
}

member_imp_temp(template<typename Set>, SBGSCC<Set>, DSBGraph<Set>, dsbg);
member_imp_temp(template<typename Set>, SBGSCC<Set>, Set, V);
member_imp_temp(template<typename Set>, SBGSCC<Set>, PWMap<Set>, Vmap);
member_imp_temp(template<typename Set>, SBGSCC<Set>, Set, E);
member_imp_temp(template<typename Set>, SBGSCC<Set>, PWMap<Set>, Emap);

member_imp_temp(template<typename Set>, SBGSCC<Set>, PWMap<Set>, mapB);
member_imp_temp(template<typename Set>, SBGSCC<Set>, PWMap<Set>, mapD);

member_imp_temp(template<typename Set>, SBGSCC<Set>, Set, Ediff);

member_imp_temp(template<typename Set>, SBGSCC<Set>, PWMap<Set>, rmap);

member_imp_temp(template<typename Set>, SBGSCC<Set>, bool, debug);

template<typename Set>
PWMap<Set> SBGSCC<Set>::sccStep()
{
  PW id_V(V());
 
  DSBGraph<Set> aux_dsbg(
    V(), Vmap()
    , mapB().restrict(E()), mapD().restrict(E()), Emap().restrict(E())
  );
  aux_dsbg.set_subE_map(dsbg().subE_map());
  MinReach min_reach(aux_dsbg, debug());
  PW new_rmap = min_reach.calculate(Set(), V()).reps();
  if (debug())
    Util::SBG_LOG << "SCC new_rmap: " << new_rmap << "\n\n";
  PW rmap_B = new_rmap.composition(mapB());
  PW rmap_D = new_rmap.composition(mapD());
  Set Esame = rmap_B.equalImage(rmap_D); // Edges in the same SCC

  // Leave edges in the same SCC
  set_E(Esame);
  set_Ediff(E().difference(Esame));

  // Swap directions
  PW aux_B = mapB();
  set_mapB(mapD());
  set_mapD(aux_B);

  return new_rmap;
}

template<typename Set>
PWMap<Set> SBGSCC<Set>::calculate()
{
  if (debug())
    Util::SBG_LOG << "SCC dsbg: \n" << dsbg() << "\n\n";

  auto begin = std::chrono::high_resolution_clock::now();
  do {
    sccStep();
    sccStep();
  } while (Ediff() != Set());
  PW rmap = sccStep(); 
  set_rmap(rmap);
  auto end = std::chrono::high_resolution_clock::now();

  auto total = std::chrono::duration_cast<std::chrono::microseconds>(
    end - begin
  );
  Util::SBG_LOG << "Total SCC exec time: " << total.count() << " [μs]\n\n"; 

  if (debug())
    Util::SBG_LOG << "SCC result: " << rmap << "\n\n";

  return rmap;
}

// -----------------------------------------------------------------------------
// Topological sort ------------------------------------------------------------
// -----------------------------------------------------------------------------

template<typename Set>
SBGTopSort<Set>::SBGTopSort()
  : dsbg_(), smap_(), E_(), mapB_(), mapD_(), unordered_(), not_dependent_()
    , visitedV_(), curr_(), debug_(false) {}
template<typename Set>
SBGTopSort<Set>::SBGTopSort(DSBGraph<Set> dsbg, bool debug)
  : dsbg_(dsbg), smap_(), E_(dsbg.E()), mapB_(dsbg.mapB()), mapD_(dsbg.mapD())
    , unordered_(dsbg.V()), not_dependent_(), visitedV_(), curr_()
    , debug_(debug) {
  not_dependent_ = dsbg.V().difference(mapB().image());
  curr_ = not_dependent_.minElem();
}

member_imp_temp(template<typename Set>, SBGTopSort<Set>, DSBGraph<Set>, dsbg);
member_imp_temp(template<typename Set>, SBGTopSort<Set>, PWMap<Set>, smap);
member_imp_temp(template<typename Set>, SBGTopSort<Set>, Set, E);
member_imp_temp(template<typename Set>, SBGTopSort<Set>, PWMap<Set>, mapB);
member_imp_temp(template<typename Set>, SBGTopSort<Set>, PWMap<Set>, mapD);
member_imp_temp(template<typename Set>, SBGTopSort<Set>, Set, unordered);
member_imp_temp(template<typename Set>, SBGTopSort<Set>, Set, not_dependent);
member_imp_temp(template<typename Set>, SBGTopSort<Set>, Set, visitedV);
member_imp_temp(template<typename Set>, SBGTopSort<Set>, Util::MD_NAT, curr);
member_imp_temp(template<typename Set>, SBGTopSort<Set>, bool, debug);

template<typename Set>
Exp SBGTopSort<Set>::calculateExp(Util::MD_NAT n1, Util::MD_NAT n2)
{
  Exp res;

  Util::RATIONAL one(1, 1);
  for (unsigned int j = 0; j < n1.size(); ++j) { 
    Util::RATIONAL x1(n1[j]), x2(n2[j]);
    res.emplaceBack(LExp(1, x1 - x2));
  }

  return res;
}

template<typename Set>
void SBGTopSort<Set>::topSortStep()
{
  Set nd = not_dependent();
  Util::MD_NAT next = nd.minElem();
  Set next_set(next), dom = next_set; 
  Exp exp = calculateExp(curr(), next);
  set_curr(next);

  if (debug()) {
    Util::SBG_LOG << "curr: " << curr() << "\n";
    Util::SBG_LOG << "dom: " << dom << "\n";
    Util::SBG_LOG << "exp: " << exp << "\n\n";
  }

  Set ingoing = mapD().preImage(dom);
  updateStatus(dom, exp, ingoing);
 
  return;
}

template<typename Set>
void SBGTopSort<Set>::updateStatus(Set dom, Exp exp, Set ingoing)
{
  PW Vmap = dsbg().Vmap();

  Set newE = E();
  Set ith_dom = dom, new_unord = unordered(); 
  if (!dom.intersection(visitedV()).isEmpty()) {
    Set dom_sv = Vmap.preImage(Vmap.image(dom));
    ith_dom = dom_sv.difference(smap().dom());

    smap_ref().emplaceBack(SBGMap<Set>(ith_dom, exp));

    // Take out vertices whose succ now also has a succ
    Set no_succ = unordered().difference(smap().dom()); 
    Set whole_ord = smap().dom().difference(smap().preImage(no_succ));
    new_unord = new_unord.difference(whole_ord);
    newE = newE.difference(mapD().preImage(whole_ord));
  }
  else {
    smap_ref().emplaceBack(SBGMap<Set>(dom, exp));
  }
  new_unord = new_unord.difference(dom);
  newE = newE.difference(mapD().preImage(dom));

  Set SV = Vmap.preImage(Vmap.image(ith_dom));
  set_visitedV(visitedV().cup(SV));

  set_E(newE);
  set_mapB(mapB().restrict(E()));
  set_mapD(mapD().restrict(E()));
 
  set_unordered(new_unord);
  set_not_dependent(unordered().difference(mapB().image()));

  Set start = smap().dom().difference(smap().image());
  if (start.cardinal() == 1)
    set_curr(start.minElem());

  if (debug()) {
    Util::SBG_LOG << "curr: " << curr() << "\n";
    Util::SBG_LOG << "smap: " << smap() << "\n";
    Util::SBG_LOG << "E: " << E() << "\n";
    Util::SBG_LOG << "unord: " << unordered() << "\n";
    Util::SBG_LOG << "nd: " << not_dependent() << "\n";
    Util::SBG_LOG << "visitedV: " << visitedV() << "\n\n";
  }

  return;
}

template<typename Set>
PWMap<Set> SBGTopSort<Set>::calculate()
{
  if (debug())
    Util::SBG_LOG << "Topological sort dsbg:\n" << dsbg() << "\n\n";

  auto begin = std::chrono::high_resolution_clock::now();
  Util::MD_NAT aux_curr = curr();
  Set curr_set(curr());
  Exp exp = calculateExp(curr(), curr());
  updateStatus(curr_set, exp, mapD().preImage(curr_set));
  set_curr(aux_curr);
  while (!unordered().isEmpty())
    topSortStep();
  auto end = std::chrono::high_resolution_clock::now();

  auto total = std::chrono::duration_cast<std::chrono::microseconds>(
    end - begin
  );
  Util::SBG_LOG << "Total topological sort exec time: " << total.count() << " [μs]\n\n"; 

  if (debug())
    Util::SBG_LOG << "Topological sort result:\n" << smap().compact() << "\n\n";

  return smap().compact();
}

// Template instantiations -----------------------------------------------------

template BasePWMap connectedComponents<UnordSet>(BaseSBG g);
template CanonPWMap connectedComponents<OrdSet>(CanonSBG g);

template struct PathInfo<UnordSet>;
template struct PathInfo<OrdSet>;

template struct MinReach<UnordSet>;
template struct MinReach<OrdSet>;

template struct MatchInfo<UnordSet>;
template std::ostream &operator<<(
  std::ostream &out, const MatchInfo<UnordSet> &mi
);
template struct MatchInfo<OrdSet>;
template std::ostream &operator<<(
  std::ostream &out, const MatchInfo<OrdSet> &mi
);

template struct SBGMatching<UnordSet>;
template struct SBGMatching<OrdSet>;

template struct SBGSCC<UnordSet>;
template struct SBGSCC<OrdSet>;

template struct SBGTopSort<UnordSet>;
template struct SBGTopSort<OrdSet>;

// Additional operations -------------------------------------------------------

template<typename Set>
DSBGraph<Set> buildSCCFromMatching(const SBGMatching<Set> &match)
{
  auto start = std::chrono::high_resolution_clock::now();
  Set matched_edges = match.matched_E(), unmatched_edges = match.unmatched_E();

  Set V = matched_edges;
  PWMap<Set> auxVmap = match.sbg().subE_map().restrict(matched_edges), Vmap;
  for (const SBGMap<Set> &map : auxVmap) 
    Vmap.emplaceBack(SBGMap<Set>(map.dom().compact(), map.exp()));

  PWMap<Set> matchedF_inv = match.mapF().restrict(matched_edges).inverse();
  PWMap<Set> unmatchedF = match.mapF().restrict(unmatched_edges);
  PWMap<Set> mapB = matchedF_inv.composition(unmatchedF);
  mapB = mapB.compact();
  PWMap<Set> matchedU_inv = match.mapU().restrict(matched_edges).inverse();
  PWMap<Set> unmatchedU = match.mapU().restrict(unmatched_edges);
  PWMap<Set> mapD = matchedU_inv.composition(unmatchedU);
  mapD = mapD.compact();

  unsigned int j = 1, dims = Vmap.nmbrDims();
  PWMap<Set> Emap;
  for (const SBGMap<Set> &map1 : Vmap) {
    Set edges1 = mapB.preImage(map1.dom());
    for (const SBGMap<Set> &map2 : Vmap) {
      Set edges2 = mapD.preImage(map2.dom());
      Set dom = edges1.intersection(edges2);
      Exp exp(Util::MD_NAT(dims, j));

      Emap.emplaceBack(SBGMap<Set>(dom.compact(), exp));
      ++j;
    }
  }

  PWMap<Set> subE_map;
  j = 1;
  for (const SBGMap<Set> &mapb : mapB) {
    for (const SBGMap<Set> &mapd : mapD) {
      Set ith = mapb.dom().intersection(mapd.dom());
      Exp exp(Util::MD_NAT(dims, j));
      subE_map.emplaceBack(SBGMap<Set>(ith.compact(), exp));
      ++j; 
    }
  }

  DSBGraph<Set> res(V, Vmap, mapB, mapD, Emap);
  res.set_subE_map(subE_map);
  auto end = std::chrono::high_resolution_clock::now();
  auto total = std::chrono::duration_cast<std::chrono::microseconds>(
    end - start 
  );
  Util::SBG_LOG << "SBG SCC builder: " << total.count() << " [μs]\n\n"; 

  return res;
}

template BaseDSBG buildSCCFromMatching(const BaseMatch &match);
template CanonDSBG buildSCCFromMatching(const CanonMatch &match);

template<typename Set>
DSBGraph<Set> buildSortFromSCC(
  const SBGSCC<Set> &scc, const PWMap<Set> &rmap
)
{
  DSBGraph dsbg = scc.dsbg();
  Set Ediff = dsbg.E().difference(scc.E());
  PWMap<Set> subE_map = dsbg.subE_map().restrict(Ediff); 
  PWMap<Set> mapB = rmap.composition(dsbg.mapB().restrict(Ediff));
  mapB = mapB.compact();
  PWMap<Set> mapD = rmap.composition(dsbg.mapD().restrict(Ediff));
  mapD = mapD.compact();

  PWMap<Set> aux_rmap = rmap.compact();
  PWMap<Set> reps_rmap = aux_rmap.filterMap([](const SBGMap<Set> &sbgmap) {
    return eqId(sbgmap);
  });
  Set V = reps_rmap.dom();

  for (const SBGMap<Set> &map : subE_map) {
    Set ith_dom = mapB.image(map.dom()).intersection(V);
    V = V.difference(ith_dom);
    V = V.concatenation(ith_dom);
  }
  for (const SBGMap<Set> &map : subE_map) {
    Set ith_dom = mapD.image(map.dom()).intersection(V);
    V = V.difference(ith_dom);
    V = V.concatenation(ith_dom);
  }

  PWMap<Set> Vmap;
  unsigned int j = 1, dims = rmap.nmbrDims();
  for (SetPiece mdi : V) {
    Util::MD_NAT v(dims, j);
    Vmap.emplaceBack(SBGMap<Set>(mdi, Exp(v)));
    ++j;
  }

  j = 1;
  PWMap<Set> Emap;
  for (const SBGMap<Set> &map1 : Vmap) {
    Set edges1 = mapB.preImage(map1.dom());
    for (const SBGMap<Set> &map2 : Vmap) {
      Set edges2 = mapD.preImage(map2.dom());
      Set dom = edges1.intersection(edges2);
      Exp exp(Util::MD_NAT(dims, j));

      Emap.emplaceBack(SBGMap<Set>(dom.compact(), exp));
      ++j;
    }
  }

  DSBGraph<Set> res(V, Vmap, mapB, mapD, Emap);
  res.set_subE_map(subE_map);
  return res;
}

template BaseDSBG buildSortFromSCC(const BaseSCC &scc, const BasePWMap &rmap);
template CanonDSBG buildSortFromSCC(const CanonSCC &scc, const CanonPWMap &rmap);

template<typename Set>
void buildJson(
  const Set &matching, const PWMap<Set> &scc, const PWMap<Set> &order
)
{
  // 1. Parse a JSON string into DOM.
  const char* json = "{\"matching\":\"\",\"scc\":\"\",\"order\":\"\"}";
  rapidjson::Document d;
  d.Parse(json);

  // 2. Modify it by DOM.
  rapidjson::Value &m = d["matching"];
  std::stringstream ss1;
  ss1 << matching;
  m.SetString(ss1.str().c_str(), strlen(ss1.str().c_str()), d.GetAllocator());

  rapidjson::Value &s = d["scc"];
  std::stringstream ss2;
  ss2 << scc;
  s.SetString(ss2.str().c_str(), strlen(ss2.str().c_str()), d.GetAllocator());

  rapidjson::Value &o = d["order"];
  std::stringstream ss3;
  ss3 << order;
  o.SetString(ss3.str().c_str(), strlen(ss3.str().c_str()), d.GetAllocator());

  // 3. Stringify the DOM
  FILE *fp = fopen("output.json", "w");
  char write_buffer[65536];
  rapidjson::FileWriteStream os(fp, write_buffer, sizeof(write_buffer));
  rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
  d.Accept(writer);

  fclose(fp);

  return;
}

template void buildJson
(
  const UnordSet &match, const BasePWMap &scc, const BasePWMap &order
);
template void buildJson
(
  const OrdSet &match, const CanonPWMap &scc, const CanonPWMap &order
);

} // namespace LIB

} // namespace SBG
