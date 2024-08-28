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
    PWMap<Set> rmap(g.V()), old_rmap;

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
// Matching --------------------------------------------------------------------
// -----------------------------------------------------------------------------

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

  return res.combine(omap()).compact();
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

  return DSBGraph<Set>(
    _V.compact()
   , _Vmap.compact() 
   , _mapB.compact()
   , _mapD.compact()
   , Emap().restrict(paths_edges()).compact());
}

template<typename Set>
void SBGMatching<Set>::selectSucc(DSBGraph<Set> dsbg)
{
  Set dsbgV = dsbg.V();
  PW dsbgB = dsbg.mapB(), dsbgD = dsbg.mapD(), dsbg_Vmap = dsbg.Vmap();
  if (debug())
    Util::SBG_LOG << dsbg << "\n\n";

  // Unmatched vertices in forward direction
  Set unmatched_D = dsbgD.image().intersection(unmatched_V());
  PW res(unmatched_D);

  // A record of allowed edges to keep out cycle edges
  Set allowed_edges = dsbg.E();
  Set ith_end = unmatched_D; // Ends of paths to mininmum reachable
  Set ingoing = dsbgD.preImage(ith_end); // Ingoing edges to ith_end
  // A record of visited set-vertices
  Set visitedV = dsbg_Vmap.preImage(dsbg_Vmap.image(ith_end));
  do {
    // Calculate successor for ith vertices
    PW ingB = dsbgB.restrict(ingoing), ingD = dsbgD.restrict(ingoing);
    PW ith_smap = ingB.minAdjMap(ingD);
    if (debug())
      Util::SBG_LOG << "ith_smap: " << ith_smap << "\n";

    // Adjacent vertices that reach ith_end
    Set ith_start = ith_smap.dom();
    if (debug()) {
     Util::SBG_LOG << "ith_start: " << ith_start << "\n";
     Util::SBG_LOG << "ith_end: " << ith_end << "\n";
    }

    // Vertices in a recursion
    Set rec = visitedV.intersection(ith_smap.dom()); 
    // Handle recursion
    if (!rec.isEmpty()) {
      for (const Map &ith : dsbg_Vmap) {
        PW rule = ith_smap.restrict(ith.dom());
        if (!rule.isEmpty()) {
          Exp exp = rule.begin()->exp();
          PW aux_res (Map(ith.dom().difference(res.dom()), exp));
          res = aux_res.combine(res);
        }
      }
    }
    res = ith_smap.restrict(ith_start.difference(rec)).combine(res);
 
    // Take out other outgoing edges to avoid cycles
    allowed_edges = allowed_edges.difference(dsbgB.preImage(res.dom()));
    dsbgD = dsbgD.restrict(allowed_edges);
    dsbgB = dsbgB.restrict(allowed_edges);

    // Vertices without a predecessor
    ith_end = res.dom();
    ingoing = dsbgD.preImage(ith_end).intersection(allowed_edges);

    visitedV = visitedV.cup(dsbg_Vmap.preImage(dsbg_Vmap.image(ith_start)));
    if (debug()) {
      Util::SBG_LOG << "ith_end 2: " << ith_end << "\n";
      Util::SBG_LOG << "res: " << res << "\n\n";
    }
  } while (!ingoing.isEmpty());

  set_smap(res);
  set_rmap(res.mapInf());
}

template<typename Set>
void SBGMatching<Set>::directedMinReach(const PW &dir_map)
{
  PW dir_omap = directedOffset(dir_map);
  if (debug())
    Util::SBG_LOG << "dir_omap: " << dir_omap << "\n";
  DSBGraph<Set> dsbg = offsetGraph(dir_omap);
  dsbg.set_subE_map(sbg().subE_map().restrict(paths_edges()));
  Set V = dsbg.V();
  PW subE_map = sbg().subE_map().restrict(paths_edges());
  PW auxB = dsbg.mapB(), auxD = dsbg.mapD();
  for (const SBGMap<Set> &map : subE_map) {
    Set ith_dom = auxB.image(map.dom()).intersection(V);
    V = V.difference(ith_dom);
    V = V.concatenation(ith_dom);
  }
  for (const SBGMap<Set> &map : subE_map) {
    Set ith_dom = auxD.image(map.dom()).intersection(V);
    V = V.difference(ith_dom);
    V = V.concatenation(ith_dom);
  }
  dsbg.set_subE_map(subE_map);

  PWMap<Set> Vmap;
  unsigned int j = 1, dims = dir_omap.arity();
  for (SetPiece mdi : V) {
    Util::MD_NAT v(dims, j);
    Vmap.emplaceBack(SBGMap<Set>(mdi, Exp(v)));
    ++j;
  }
  dsbg.set_Vmap(Vmap);

  selectSucc(dsbg);

  PW aux_omap = dir_omap.combine(omap()), to_normal = aux_omap.inverse();
  PW succs = smap().composition(aux_omap);
  set_smap(to_normal.composition(succs));
  PW reps = rmap().composition(aux_omap);
  set_rmap(to_normal.composition(reps));

  if (debug()) {
    Util::SBG_LOG << "minReach smap: " << smap() << "\n";
    Util::SBG_LOG << "minReach rmap: " << rmap() << "\n\n";
  }
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
bool eqId(const SBGMap<Set> &sbgmap) { return sbgmap.isId(); }

template<typename Set>
bool notEqId(const SBGMap<Set> &sbgmap) { return !(sbgmap.isId()); }

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
    , matched_U_(), unmatched_U_(), debug_(false) {}
template<typename Set>
SBGMatching<Set>::SBGMatching(SBGraph<Set> sbg, bool debug)
  : sbg_(sbg), V_(sbg.V()), Vmap_(sbg.Vmap()), Emap_(sbg.Emap())
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
MatchInfo<Set> SBGMatching<Set>::calculate()
{
  if (debug())
    Util::SBG_LOG << "Matching sbg: \n" << sbg() << "\n\n";

  auto begin = std::chrono::high_resolution_clock::now();
  minReachable();
  auto end = std::chrono::high_resolution_clock::now();

  if (debug())
    Util::SBG_LOG << "minReachable: " << matched_E() << "\n\n";

  Util::SBG_LOG << MatchInfo(matched_E().compact(), fullyMatchedU()) << "\n\n";

  auto total = std::chrono::duration_cast<std::chrono::microseconds>(
    end - begin
  );
  Util::SBG_LOG << "Total match exec time: " << total.count() << " [μs]\n";

  return MatchInfo(matched_E().compact(), fullyMatchedU());
}

// -----------------------------------------------------------------------------
// SCC -------------------------------------------------------------------------
// -----------------------------------------------------------------------------

template<typename Set>
SBGSCC<Set>::SBGSCC() : dsbg_(), V_(), Vmap_(), E_(), Emap_(), Ediff_(), mapB_()
  , mapD_(), rmap_(), debug_(false) {}
template<typename Set>
SBGSCC<Set>::SBGSCC(DSBGraph<Set> dsbg, bool debug)
  : dsbg_(dsbg), debug_(debug), Ediff_() {
  DSBGraph<Set> dg = partitionSE(dsbg);
  dsbg_ = dg;

  V_ = dg.V();
  Vmap_ = dg.Vmap();
  
  E_ = dg.E();
  Emap_ = dg.Emap();

  mapB_ = dg.mapB();
  mapD_ = dg.mapD(); 

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
PWMap<Set> SBGSCC<Set>::sccMinReach(DSBGraph<Set> dg) const
{
  if (debug())
    Util::SBG_LOG << "Min reach graph:\n" << dg << "\n\n";

  Set V = dg.V(), E = dg.E();
  PW mapB = dg.mapB(), mapD = dg.mapD(), subE_map = dg.subE_map();
  if (!V.isEmpty()) {
    unsigned int copies = V.arity();
    PW rmap(V), old_rmap;

    if (E.isEmpty())
      return rmap;

    do {
      old_rmap = rmap;

      PW ermapD = rmap.composition(mapD);

      PW new_rmap = mapB.minAdjMap(ermapD);
      rmap = rmap.minMap(new_rmap).combine(rmap);

      Set positive(SetPiece(copies, Interval(1, 1, Util::Inf)));
      PW rec_rmap;
      Set Vc = V.difference(old_rmap.equalImage(rmap));
      for (const Map &subv : dg.Vmap()) {
        Set vs = subv.dom();
        if (!vs.isEmpty()) {
          // Vertices in the set-vertex that share its rep with other vertex
          // in the set-vertex
          Set VR = rmap.restrict(vs).sharedImage();
          // There is a recursive vertex that changed its rep in the last step
          // (to avoid computing again an already found recursion)
          if (!VR.intersection(Vc).isEmpty()) {
            // Edges with both endings in VR (path to a minimum rep)
            Set ERB = mapB.preImage(VR), ERD = mapD.preImage(VR);
            Set ER = ERB.intersection(ERD);

            // Distance map
            PW dmap;
            Set ith = rmap.image(VR), dom_VR;
            //Set visited, dom_vs;
            Util::NAT dist = 0;
            // Calculate distance for vertices in same_rep that reach reps
            for (; dg.Vmap().restrict(dom_VR).sharedImage().isEmpty();) {
              Exp exp(Util::MD_NAT(copies, dist));
              Set dom = ith.difference(dmap.dom());
              dmap.emplaceBack(Map(ith.difference(dmap.dom()), exp));
              dom_VR = dmap.dom().intersection(VR);
              // Update ith to vertices that have outgoing edges entering ith
              ith = mapB.image(mapD.preImage(ith));
              ++dist;
            }
            dmap = dmap.restrict(VR);
            PW dmapB = dmap.composition(mapB), dmapD = dmap.composition(mapD);
            // Get edges where the end is closer to the rep that the beginning
            Set not_cycle_edges = (dmapB - dmapD).preImage(positive);
            ER = ER.intersection(not_cycle_edges);

            // Extend to subset-edge
            Set ER_plus = subE_map.preImage(subE_map.image(ER));
            PW auxB = mapB.restrict(ER_plus), auxD = mapD.restrict(ER_plus);
            // Calculate a succesor
            Set same_SV = dg.Vmap().preImage(dg.Vmap().image(VR));
            PW rmap_plus = auxB.minAdjMap(auxD).restrict(same_SV);

            // Update rmap for recursion, and leave the rest unchanged
            rec_rmap = rmap_plus.combine(rec_rmap).compact();

            if (debug()) {
              Util::SBG_LOG << "VR: " << VR << "\n";
              Util::SBG_LOG << "ER: " << ER << "\n";
              Util::SBG_LOG << "dmap: " << dmap << "\n";
              Util::SBG_LOG << "not_cycle_edges: " << not_cycle_edges << "\n";
              Util::SBG_LOG << "ER_plus: " << ER_plus << "\n";
              Util::SBG_LOG << "rmap_plus: " << rmap_plus << "\n";
            }
          }
        }
      }
      rmap = rec_rmap.combine(rmap).compact();
      if (debug())
        Util::SBG_LOG << "rmap rec: " << rmap << "\n\n";

      if (rmap != old_rmap)
        rmap = rmap.mapInf();
    } while (rmap != old_rmap); 

    return rmap;
  }

  return PWMap<Set>();
}

template<typename Set>
PWMap<Set> SBGSCC<Set>::sccStep()
{
  PW id_V(V());

  DSBGraph<Set> aux_dsbg(
    V(), Vmap()
    , mapB().restrict(E()), mapD().restrict(E()), Emap().restrict(E())
  );
  PW new_rmap = sccMinReach(aux_dsbg);
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
  PW rmap = sccStep().compact(); 
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
SBGTopSort<Set>::SBGTopSort() : dsbg_(), debug_(false) {}
template<typename Set>
SBGTopSort<Set>::SBGTopSort(DSBGraph<Set> dsbg, bool debug) 
  : dsbg_(partitionSE(dsbg)), debug_(debug) {}

member_imp_temp(template<typename Set>, SBGTopSort<Set>, DSBGraph<Set>, dsbg);
member_imp_temp(template<typename Set>, SBGTopSort<Set>, bool, debug);

template<typename Set>
Exp SBGTopSort<Set>::calculateExp(Util::MD_NAT from, Util::MD_NAT to)
{
  Exp res;

  Util::RATIONAL one(1, 1);
  for (unsigned int j = 0; j < from.arity(); ++j) { 
    Util::RATIONAL r_from(from[j]), r_to(to[j]);
    res.emplaceBack(LExp(1, r_to - r_from));
  }

  return res;
}

template<typename Set>
PWMap<Set> SBGTopSort<Set>::calculate()
{
  if (debug())
    Util::SBG_LOG << "Topological sort dsbg:\n" << dsbg() << "\n\n";

  auto begin = std::chrono::high_resolution_clock::now();
  PW mapB = dsbg().mapB(), mapD = dsbg().mapD(), Vmap = dsbg().Vmap(), smap;
  Set U = dsbg().V(), Nd = U.difference(mapB.image());
  Util::MD_NAT vsucc = Nd.minElem();
  Set SV, E = dsbg().E();
  do {
    Set Nd_vsucc = Nd.intersection(Vmap.preImage(Vmap.image(vsucc)));
    Util::MD_NAT v = Nd.minElem();
    if (!Nd_vsucc.isEmpty())
      v = Nd_vsucc.minElem();
    Set d(v);
    Exp e = calculateExp(v, vsucc);
    vsucc = v;

    Set SVd = Vmap.image(d);
    bool cond = SVd.intersection(SV).isEmpty();
    if (!cond) {
      Set dvs = Vmap.preImage(SVd);
      for (const Map &map : smap.restrict(dvs)) {
        if (e == map.exp()) {
          d = dvs.difference(smap.dom());
          break;
        }
      }
    }
    smap.emplaceBack(Map(d, e));
    
    Set Nsucc = U.difference(smap.dom());
    Set S = smap.dom().difference(smap.preImage(Nsucc));

    E = E.difference(mapD.preImage(S));
    mapB = mapB.restrict(E);
    mapD = mapD.restrict(E);

    U = U.difference(S);
    Nd = U.difference(mapB.image());
    SV = SV.cup(Vmap.image(d));
    if (S == smap.dom()) {
      Set start = smap.dom().difference(smap.image());
      if (!start.isEmpty())
        vsucc = start.minElem(); 
    }

    if (debug()) {
      Util::SBG_LOG << "S: " << S << "\n";
      Util::SBG_LOG << "U: " << U << "\n";
      Util::SBG_LOG << "E: " << E << "\n";
      Util::SBG_LOG << "Nd: " << Nd << "\n";
      Util::SBG_LOG << "smap: " << smap << "\n\n";
    }
  } while (!U.isEmpty());
  auto end = std::chrono::high_resolution_clock::now();

  auto total = std::chrono::duration_cast<std::chrono::microseconds>(
    end - begin
  );
  Util::SBG_LOG << "Total topological sort exec time: " << total.count() << " [μs]\n\n"; 

  if (debug())
    Util::SBG_LOG << "Topological sort result:\n" << smap.compact() << "\n\n";

  return smap.compact();
}

// Cut-set algorithm -----------------------------------------------------------

template<typename Set>
SBGCutSet<Set>::SBGCutSet() : dsbg_(), debug_(false) {}
template<typename Set>
SBGCutSet<Set>::SBGCutSet(DSBGraph<Set> dsbg, bool debug) 
  : dsbg_(dsbg), debug_(debug) {}

member_imp_temp(template<typename Set>, SBGCutSet<Set>, DSBGraph<Set>, dsbg);
member_imp_temp(template<typename Set>, SBGCutSet<Set>, bool, debug);

template<typename Set>
Set SBGCutSet<Set>::calculate()
{
  DSBGraph<Set> dg = dsbg();
  PW Vmap = dg.Vmap(), mapB = dg.mapB(), mapD = dg.mapD();
  PW rmap = SBGSCC(dg, debug()).calculate();
  Set newD, oldD, visitedV;

  // Degree map
  Util::MD_NAT zero(rmap.arity(), 0), one(rmap.arity(), 1);
  PW dmap(Map(dg.V(), Exp(zero))); 
  for (const Map &SE : dg.subE_map()) {
    Set vs = mapB.image(SE.dom()).cup(mapD.image(SE.dom()));
    PW ith = dmap.restrict(vs).offsetImage(one);
    dmap = ith.combine(dmap);
  }
  if (debug())
    Util::SBG_LOG << "initial dmap: " << dmap << "\n\n";

  while (dg.V() != rmap.image()) {
    oldD = newD;
 
    Util::MD_NAT aux = dmap.image().maxElem();
    Util::MD_NAT vmax = dmap.preImage(Set(aux)).minElem();
    Set vmaxSV = Vmap.image(Set(vmax));
    newD = newD.cup(Set(vmax));

    if (!visitedV.intersection(vmaxSV).isEmpty()) {
      Util::MD_NAT vmaxD = newD.intersection(Vmap.preImage(vmaxSV)).minElem();

      SetPiece mdi;
      for (unsigned int j = 0; j < vmax.arity(); ++j) {
        if (vmax[j] < vmaxD[j]) {
          Util::NAT st = vmaxD[j] - vmax[j];
          Util::NAT beg = vmax[j] % st;
          mdi.emplaceBack(Interval(beg, st, vmax[j]));
        }

        else
          mdi.emplaceBack(Interval(vmaxD[j], vmax[j] - vmaxD[j], Util::Inf));    
      }
      newD = Set(mdi).intersection(Vmap.preImage(vmaxSV));
    }

    // Update graph info erasing newD vertices
    visitedV = visitedV.cup(vmaxSV);
    dg = dg.eraseVertices(newD);
    Vmap = dg.Vmap();
    mapB = dg.mapB(); 
    mapD = dg.mapD(); 

    // Update degree map
    Set deltaD = newD.difference(oldD);
    PW ith_dmap = dmap.restrict(deltaD).offsetImage(one);
    dmap = ith_dmap.combine(dmap);

    // Resulting SCC from induced graph
    rmap = SBGSCC(dg, debug()).calculate();

    if (debug()) {
      Util::SBG_LOG << "resulting graph:\n" << dg << "\n";
      Util::SBG_LOG << "new degree map: " << dmap << "\n";
      Util::SBG_LOG << "new rmap: " << rmap << "\n\n";
    }
  }

  return newD;
}

// Template instantiations -----------------------------------------------------

template BasePWMap connectedComponents<UnordSet>(BaseSBG g);
template CanonPWMap connectedComponents<OrdSet>(CanonSBG g);

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

template struct SBGCutSet<UnordSet>;
template struct SBGCutSet<OrdSet>;

// Additional operations -------------------------------------------------------

template<typename Set>
DSBGraph<Set> partitionSE(const DSBGraph<Set> &dg)
{
  Set V = dg.V();
  unsigned int dims = V.arity(), j = 1;
  PWMap mapB = dg.mapB(), mapD = dg.mapD(), Emap = dg.Emap();

  std::vector<Set> vs;
  for (const SBGMap<Set> &map : dg.Vmap())
    vs.emplace_back(map.dom());

  Set univ(SetPiece(dims, Interval(0, 1, Util::Inf)));
  for (const SBGMap<Set> &map : dg.subE_map()) {
    Set ith_edge = mapB.image(map.dom());
    Set not_ith_edge = univ.difference(ith_edge);
    std::vector<Set> aux_vs;
    aux_vs.reserve(2*vs.size());
    for (const Set &ith_vs : vs) {
      Set s1 = ith_edge.intersection(ith_vs);
      Set s2 = not_ith_edge.intersection(ith_vs);

      if (!s1.isEmpty())
        aux_vs.emplace_back(s1);

      if (!s2.isEmpty())
        aux_vs.emplace_back(s2);
    }
    vs = aux_vs;
  }

  for (const SBGMap<Set> &map : dg.subE_map()) {
    Set ith_edge = mapD.image(map.dom());
    Set not_ith_edge = univ.difference(ith_edge);
    std::vector<Set> aux_vs;
    aux_vs.reserve(2*vs.size());
    for (const Set &ith_vs : vs) {
      Set s1 = ith_edge.intersection(ith_vs);
      Set s2 = not_ith_edge.intersection(ith_vs);

      if (!s1.isEmpty())
        aux_vs.emplace_back(s1);

      if (!s2.isEmpty())
        aux_vs.emplace_back(s2);
    }
    vs = aux_vs;
  }

  PWMap<Set> Vmap;
  for (const Set &dom : vs) {
    Util::MD_NAT v(dims, j);
    Vmap.emplaceBack(SBGMap<Set>(dom, Exp(v)));
    ++j;
  }

  return DSBGraph(V, Vmap, mapB, mapD, Emap);
}

template<typename Set>
DSBGraph<Set> buildSCCFromMatching(const SBGMatching<Set> &match)
{
  auto start = std::chrono::high_resolution_clock::now();
  Set matched_edges = match.matched_E(), unmatched_edges = match.unmatched_E();

  Set V = matched_edges.compact();
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

  unsigned int j = 1, dims = Vmap.arity();
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
  PWMap<Set> mapB = rmap.composition(dsbg.mapB().restrict(Ediff));
  mapB = mapB.compact();
  PWMap<Set> mapD = rmap.composition(dsbg.mapD().restrict(Ediff));
  mapD = mapD.compact();

  PWMap<Set> aux_rmap = rmap.compact();
  PWMap<Set> reps_rmap = aux_rmap.filterMap([](const SBGMap<Set> &sbgmap) {
    return eqId(sbgmap);
  });
  Set V = reps_rmap.dom();

  PWMap<Set> Vmap = dsbg.Vmap().restrict(V);

  PWMap<Set> Emap = dsbg.Emap().restrict(Ediff);

  DSBGraph<Set> res(V, Vmap, mapB, mapD, Emap);
  return res;
}

template BaseDSBG buildSortFromSCC(const BaseSCC &scc, const BasePWMap &rmap);
template CanonDSBG buildSortFromSCC(const CanonSCC &scc, const CanonPWMap &rmap);

template<typename Set>
rapidjson::Value setJson(const Set &s, rapidjson::Document::AllocatorType &alloc)
{
  rapidjson::Value res(rapidjson::kArrayType);

  for (const SetPiece &mdi : s) {
    rapidjson::Value inter_array(rapidjson::kArrayType);
    for (const Interval &i : mdi) {
      rapidjson::Value inter(rapidjson::kArrayType);

      rapidjson::Value beg;
      beg.SetInt(i.begin());
      inter.PushBack(beg, alloc);
      rapidjson::Value st;
      st.SetInt(i.step());
      inter.PushBack(st, alloc);
      rapidjson::Value end;
      end.SetInt(i.end());
      inter.PushBack(end, alloc);

      inter_array.PushBack(inter, alloc);
    }
    rapidjson::Value mdi_obj(rapidjson::kObjectType);
    mdi_obj.AddMember("interval", inter_array, alloc);
    res.PushBack(mdi_obj, alloc);
  }

  return res;
}

rapidjson::Value expJson(Exp exp, rapidjson::Document::AllocatorType &alloc)
{
  rapidjson::Value res(rapidjson::kArrayType);

  for (const LExp &le : exp) {
    rapidjson::Value le_array(rapidjson::kArrayType);

    std::stringstream ssm;
    ssm << le.slope();
    rapidjson::Value m;
    m.SetString(ssm.str().c_str(), strlen(ssm.str().c_str()), alloc);
    le_array.PushBack(m, alloc);

    std::stringstream ssh;
    ssh << le.offset();
    rapidjson::Value h;
    h.SetString(ssh.str().c_str(), strlen(ssh.str().c_str()), alloc);
    le_array.PushBack(h, alloc);

    res.PushBack(le_array, alloc);
  }

  return res;
}

template<typename Set>
rapidjson::Value mapJson(
  const PWMap<Set> &pw, rapidjson::Document::AllocatorType &alloc
)
{
  rapidjson::Value res(rapidjson::kArrayType);

  for (const SBGMap<Set> &map : pw) {
    rapidjson::Value ith(rapidjson::kObjectType);

    ith.AddMember("dom", setJson(map.dom(), alloc), alloc);
    ith.AddMember("exp", expJson(map.exp(), alloc), alloc);

    res.PushBack(ith, alloc);
  }

  return res;
}

template<typename Set>
void buildJson(
  const Set &matching, const PWMap<Set> &scc, const PWMap<Set> &order
)
{
  //const char* json = "{\"matching\":\"\",\"scc\":\"\",\"order\":\"\"}";
  rapidjson::Document d;
  d.SetObject();
  rapidjson::Document::AllocatorType& alloc = d.GetAllocator();

  // Create matching information
  rapidjson::Value edges = setJson(matching, alloc);
  d.AddMember("matching", edges, alloc);

  // Create SCC information
  rapidjson::Value scc_rmap = mapJson(scc, alloc);
  d.AddMember("scc", scc_rmap, alloc);

  // Create sort information
  rapidjson::Value order_rmap = mapJson(order, alloc);
  d.AddMember("sort", order_rmap, alloc);

  FILE *fp = fopen("output.json", "w");
  char write_buffer[65536];
  rapidjson::FileWriteStream os(fp, write_buffer, sizeof(write_buffer));
  rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
  rapidjson::PrettyFormatOptions opt = rapidjson::kFormatSingleLineArray;
  writer.SetFormatOptions(opt);
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
