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

////////////////////////////////////////////////////////////////////////////////
// Connected components --------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

PWMap connectedComponents(SBG g)
{
  const PWMapAF &fact_ = g.fact();

  if (!g.V().isEmpty()) {
    PWMap rmap = fact_.createPWMap(g.V()), old_rmap = fact_.createPWMap();

    if (g.E().isEmpty())
      return rmap;

    do {
      old_rmap = rmap;

      PWMap ermap1 = rmap.composition(g.map1());
      PWMap ermap2 = rmap.composition(g.map2());

      PWMap rmap1 = ermap1.minAdjMap(ermap2);
      PWMap rmap2 = ermap2.minAdjMap(ermap1);
      rmap1 = rmap1.combine(rmap);
      rmap2 = rmap2.combine(rmap);

      PWMap aux_rmap = rmap1.minMap(rmap2);
      rmap = rmap.minMap(aux_rmap);

      if (!(rmap == old_rmap)) {
        rmap = aux_rmap;
        rmap = rmap.mapInf();
      }
    } while (rmap != old_rmap); 

    return rmap.compact();
  }

  return fact_.createPWMap();
}

////////////////////////////////////////////////////////////////////////////////
// Matching --------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

// Minimum reachable -----------------------------------------------------------

PWMap SBGMatching::directedOffset(const PWMap &dir_map) const
{
  if (debug())
    Util::SBG_LOG << "directedOffset unmatched_V: " << unmatched_V() << "\n\n";
  Set unmatched_side = dir_map.image(paths_edges());
  unmatched_side = unmatched_side.intersection(unmatched_V());
  PWMap res = omap().restrict(unmatched_side);
  res = res.offsetImage(max_V());

  return res.combine(omap()).compact();
}

DSBG SBGMatching::offsetGraph(const PWMap &dir_omap) const
{
  Set _V = dir_omap.image();
  PWMap _Vmap = Vmap().offsetDom(dir_omap);

  PWMap _mapB = dir_omap.composition(mapB())
            , _mapD = dir_omap.composition(mapD());
  _mapB = _mapB.restrict(paths_edges());
  _mapD = _mapD.restrict(paths_edges());

  return DSBG(
    fact_
    , _V.compact()
    , _Vmap.compact() 
    , _mapB.compact()
    , _mapD.compact()
    , Emap().restrict(paths_edges()).compact()
    , subEmap().restrict(paths_edges()).compact()
  );
}

void SBGMatching::selectSucc(DSBG dsbg)
{
  Set dsbgV = dsbg.V();
  PWMap dsbgB = dsbg.mapB(), dsbgD = dsbg.mapD(), dsbg_subE = dsbg.subEmap();
  if (debug())
    Util::SBG_LOG << "\nsucc dsbg:\n" << dsbg << "\n\n";

  Set M = matched_E(), NM = unmatched_E();
  PWMap subEmap = fact_.createPWMap();
  unsigned int j = 1, dims = dsbgV.arity();
  for (const Map &SE : dsbg_subE) {
    Exp expM(MD_NAT(dims, j));
    Map SEM = fact_.createMap(M.intersection(SE.dom()), expM);
    ++j;
    Exp expU(MD_NAT(dims, j));
    Map SEU = fact_.createMap(NM.intersection(SE.dom()), expU);
    ++j;

    subEmap.emplaceBack(SEM);
    subEmap.emplaceBack(SEU);
  }
  if (debug())
    Util::SBG_LOG << "subEmap:\n" << subEmap << "\n\n";

  // Unmatched vertices in forward direction
  Set unmatched_D = dsbgD.image().intersection(unmatched_V());
  PWMap res = fact_.createPWMap(unmatched_D);

  // A record of allowed edges to keep out cycle edges
  Set allowed_edges = dsbg.E();
  // Ingoing edges to vertices that reach unmatched_D
  Set ingoing = dsbgD.preImage(unmatched_D); 
  // A record of visited set-edges
  Set visitedE = fact_.createSet();
  do {
    // Calculate successor for ith vertices
    PWMap ingB = dsbgB.restrict(ingoing), ingD = dsbgD.restrict(ingoing);
    PWMap ith_smap = ingB.minAdjMap(ingD);
    if (debug())
      Util::SBG_LOG << "ith_smap: " << ith_smap << "\n";

    // Edges that lead to a successor
    Set Eith = dsbgD.equalImage(ith_smap.composition(dsbgB));
    // Visited set-edges
    Set Erec = visitedE.intersection(subEmap.image(Eith)); 
    // Handle recursion
    if (!Erec.isEmpty()) {
      Set Eplus = subEmap.preImage(Erec);
      PWMap rec_smap = dsbgB.restrict(Eplus).minAdjMap(dsbgD.restrict(Eplus)); 
      ith_smap = ith_smap.combine(rec_smap);
    }
    res = ith_smap.combine(res);
 
    // Take out other outgoing edges to avoid cycles
    allowed_edges = allowed_edges.difference(dsbgB.preImage(res.dom()));
    dsbgD = dsbgD.restrict(allowed_edges);
    dsbgB = dsbgB.restrict(allowed_edges);

    // Edges that reach vertices with a successor
    ingoing = dsbgD.preImage(res.dom()).intersection(allowed_edges);

    visitedE = visitedE.cup(subEmap.image(Eith));
    if (debug()) {
      Util::SBG_LOG << "Eith: " << Eith << "\n";
      Util::SBG_LOG << "Erec: " << Erec << "\n";
      Util::SBG_LOG << "visitedE: " << visitedE << "\n";
      Util::SBG_LOG << "res: " << res << "\n\n";
    }
  } while (!ingoing.isEmpty());

  smap_ = res;
  rmap_ = res.mapInf();
}

void SBGMatching::directedMinReach(const PWMap &dir_map)
{
  PWMap dir_omap = directedOffset(dir_map);
  if (debug())
    Util::SBG_LOG << "dir_omap: " << dir_omap << "\n";
  DSBG dsbg = offsetGraph(dir_omap);

  selectSucc(dsbg);

  PWMap aux_omap = dir_omap.combine(omap()), to_normal = aux_omap.inverse();
  PWMap succs = smap().composition(aux_omap);
  smap_ = to_normal.composition(succs);
  PWMap reps = rmap().composition(aux_omap);
  rmap_ = to_normal.composition(reps);

  if (debug()) {
    Util::SBG_LOG << "minReach smap: " << smap() << "\n";
    Util::SBG_LOG << "minReach rmap: " << rmap() << "\n\n";
  }
}

void SBGMatching::minReachableStep()
{
  // *** Forward direction
  directedMinReach(mapU());
  PWMap rmapd = rmap();
  Set reach_unmatched = rmap().preImage(unmatched_F());
  Set pe = edgesInPaths();
  paths_edges_ = pe;

  // *** Backward direction
  PWMap auxB = mapB();
  mapB_ = mapD();
  mapD_ = auxB;

  directedMinReach(mapF());
  // Vertices that reach unmatched left and right vertices 
  reach_unmatched = reach_unmatched.intersection(rmap().preImage(unmatched_U()));
  rmap_ = rmap().restrict(reach_unmatched);
  smap_ = smap().restrict(reach_unmatched);

  const PWMap &rmaprmapd = rmap().composition(rmapd);
  Set edgesb = edgesSameRepLR(rmaprmapd);
  pe = pe.intersection(edgesInPaths().intersection(edgesb));
  paths_edges_ = pe;

  // *** Initial direction
  mapD_ = mapB();
  mapB_ = auxB;

  // *** Update structures to reflect new matched edges
  updatePaths(); 

  // *** Offset matched vertices
  updateOffset();

  return;
}

void SBGMatching::minReachable()
{
  do {
    paths_edges_ = E();
    minReachableStep();
    if (debug()) {
      Util::SBG_LOG << "minimum reachable step smap: " << smap() << "\n";
      Util::SBG_LOG << "minimum reachable matched_E: " << matched_E() << "\n\n";
    }
  } while (!fullyMatchedU() && !paths_edges().isEmpty());

  return;
}

// Matching --------------------------------------------------------------------

bool eqId(const Map &sbgmap) { return sbgmap.isId(); }

bool notEqId(const Map &sbgmap) { return !(sbgmap.isId()); }

MatchInfo::MatchInfo(Set matched_edges, bool fully_matchedU) 
  : matched_edges_(matched_edges), fully_matchedU_(fully_matchedU) {}

member_imp(MatchInfo, Set, matched_edges);
member_imp(MatchInfo, bool, fully_matchedU);

std::ostream &operator<<(std::ostream &out, const MatchInfo &m_info)
{
  out << m_info.matched_edges();
  if (m_info.fully_matchedU())
    out << " [FULLY MATCHED]";

  else 
    out << " [UNMATCHED]";

  return out;
}

SBGMatching::SBGMatching(const SBG &sbg, bool debug)
  : fact_(sbg.fact()), sbg_(sbg), V_(sbg.V()), Vmap_(sbg.Vmap())
    , E_(fact_.createSet()), Emap_(sbg.Emap()), subEmap_(sbg.subEmap())
    , smap_(fact_.createPWMap()), rmap_(fact_.createPWMap())
    , omap_(fact_.createPWMap()), F_(fact_.createSet()), U_(fact_.createSet())
    , mapF_(fact_.createPWMap()), mapU_(fact_.createPWMap())
    , mapB_(fact_.createPWMap()), mapD_(fact_.createPWMap())
    , paths_edges_(fact_.createSet()), matched_E_(fact_.createSet())
    , unmatched_E_(fact_.createSet()), matched_V_(fact_.createSet())
    , unmatched_V_(fact_.createSet()), unmatched_F_(fact_.createSet())
    , matched_U_(fact_.createSet()), unmatched_U_(fact_.createSet())
    , cycle_edges_(fact_.createSet()), debug_(debug) {
  E_ = Emap_.dom();

  PWMap id_vertex = fact_.createPWMap(V_);
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
  matched_E_ = fact_.createSet();
  unmatched_E_ = E_;

  matched_V_ = fact_.createSet();
  unmatched_V_ = V_;
  unmatched_F_ = F_;
  matched_U_ = fact_.createSet();
  unmatched_U_ = U_;
}

member_imp(SBGMatching, SBG, sbg);
member_imp(SBGMatching, Set, V);
member_imp(SBGMatching, PWMap, Vmap);
member_imp(SBGMatching, Set, E);
member_imp(SBGMatching, PWMap, Emap);
member_imp(SBGMatching, PWMap, subEmap);

member_imp(SBGMatching, PWMap, smap);
member_imp(SBGMatching, PWMap, rmap);

member_imp(SBGMatching, PWMap, omap);
member_imp(SBGMatching, MD_NAT, max_V);

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

member_imp(SBGMatching, bool, debug);

Set SBGMatching::edgesInPaths() const
{
  // Vertices that are successor of other vertices in a path
  Set succs = smap().filterMap([](const Map &sbgmap) {
    return notEqId(sbgmap);
  }).image();
  // Edges whose endings are successors 
  Set ending_edges = mapD().preImage(succs);
  // Map from a 'successor' edge to its start
  PWMap auxB = mapB().restrict(ending_edges);
  // Map from edge to the successor of its start
  PWMap map_succs = smap().composition(auxB);
 
  return map_succs.equalImage(mapD());
}

Set SBGMatching::edgesSameRepLR(const PWMap &rmaprmapd) const
{
  PWMap rmap_neq_id = rmap().filterMap([](const Map &sbgmap) {
    return notEqId(sbgmap);
  });
  PWMap rmapb = rmap_neq_id.composition(mapB());
  PWMap rmaprmapd_neq_id = rmaprmapd.filterMap([](const Map &sbgmap) {
    return notEqId(sbgmap);
  }); 
  PWMap rmaprmapdb = rmaprmapd_neq_id.composition(mapB());
  
  return rmapb.equalImage(rmaprmapdb);
}

bool SBGMatching::fullyMatchedU() const
{
  return U().difference(matched_U()).isEmpty();
}

void SBGMatching::updatePaths()
{
  // *** Revert match and unmatched edges in augmenting paths
  PWMap paths_mapB = mapB().restrict(paths_edges())
     , paths_mapD = mapD().restrict(paths_edges());
  mapB_ = paths_mapD.combine(mapB());
  mapD_ = paths_mapB.combine(mapD());

  // *** Update matched edges
  matched_E_ = mapD().preImage(U());
  unmatched_E_ = mapD().preImage(F());

  // *** Update matched vertices
  Set matchedB = mapB().image(matched_E())
      , matchedD = mapD().image(matched_E());
  matched_V_= matchedB.cup(matchedD);
  matched_U_= U().intersection(matched_V());

  unmatched_V_ = V().difference(matched_V());
  unmatched_F_ = unmatched_V().intersection(F());
  unmatched_U_ = unmatched_V().intersection(U());

  return;
}

void SBGMatching::updateOffset()
{
  PWMap aux = omap().restrict(matched_V());
  aux = aux.offsetImage(max_V());
  aux = aux.offsetImage(max_V());
  omap_ = aux.combine(omap());

  return;
}

MatchInfo SBGMatching::calculate()
{
  if (debug())
    Util::SBG_LOG << "Matching sbg: \n" << sbg() << "\n\n";

  auto begin = std::chrono::high_resolution_clock::now();
  minReachable();
  auto end = std::chrono::high_resolution_clock::now();

  if (debug())
    Util::SBG_LOG << "minReachable: " << matched_E().compact() << "\n\n";

  Util::SBG_LOG << MatchInfo(matched_E().compact(), fullyMatchedU()) << "\n\n";

  auto total = std::chrono::duration_cast<std::chrono::microseconds>(
    end - begin
  );
  Util::SBG_LOG << "Total match exec time: " << total.count() << " [μs]\n";

  return MatchInfo(matched_E().compact(), fullyMatchedU());
}

const PWMapAF &SBGMatching::fact() const { return fact_; }

////////////////////////////////////////////////////////////////////////////////
// SCC -------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

SBGSCC::SBGSCC(const DSBG &dsbg, bool debug)
  : fact_(dsbg.fact()), dsbg_(dsbg), V_(fact_.createSet())
    , Vmap_(fact_.createPWMap()), Emap_(fact_.createPWMap())
    , subEmap_(fact_.createPWMap()), E_(fact_.createSet())
    , Ediff_(fact_.createSet()), mapB_(fact_.createPWMap())
    , mapD_(fact_.createPWMap()), rmap_(fact_.createPWMap())
    , debug_(debug) {
  DSBG dg = dsbg;
  dsbg_ = dg;

  V_ = dg.V();
  Vmap_ = dg.Vmap();
  
  E_ = dg.E();
  Emap_ = dg.Emap();
  subEmap_ = dg.subEmap();

  mapB_ = dg.mapB();
  mapD_ = dg.mapD(); 

  rmap_ = fact_.createPWMap(V_);
}

member_imp(SBGSCC, DSBG, dsbg);
member_imp(SBGSCC, Set, V);
member_imp(SBGSCC, PWMap, Vmap);
member_imp(SBGSCC, Set, E);
member_imp(SBGSCC, PWMap, Emap);
member_imp(SBGSCC, PWMap, subEmap);

member_imp(SBGSCC, PWMap, mapB);
member_imp(SBGSCC, PWMap, mapD);

member_imp(SBGSCC, Set, Ediff);

member_imp(SBGSCC, PWMap, rmap);

member_imp(SBGSCC, bool, debug);

PWMap SBGSCC::sccMinReach(const DSBG &dg) const
{
  if (debug())
    Util::SBG_LOG << "Min reach graph:\n" << dg << "\n\n";

  Set V = dg.V(), E = dg.E();
  PWMap mapB = dg.mapB(), mapD = dg.mapD(), subEmap = dg.subEmap();
  if (!V.isEmpty()) {
    unsigned int copies = V.arity();
    PWMap rmap = fact_.createPWMap(V), old_rmap = fact_.createPWMap();

    if (E.isEmpty())
      return rmap;

    Set Vc = fact_.createSet();
    do {
      old_rmap = rmap;

      PWMap ermapD = rmap.composition(mapD);

      PWMap new_rmap = mapB.minAdjMap(ermapD);
      rmap = rmap.minMap(new_rmap).combine(rmap);
      if (debug())
        Util::SBG_LOG << "rmap before rec: " << rmap << "\n\n";

      Set positive = fact_.createSet(SetPiece(copies, Interval(1, 1, Inf)));
      PWMap rec_rmap = fact_.createPWMap();
      Vc = V.difference(old_rmap.equalImage(rmap));
      if (!Vc.isEmpty()) {
        for (const Map &subv : dg.Vmap()) {
          Set vs = subv.dom();
          if (!vs.intersection(Vc).isEmpty()) {
            // If the mrv is in the same SV, the algorithm would detect a false
            // recursion, i.e. if we have a cycle 1 -> 2 -> ... -> 10 -> 1,
            // where SV = [1:10], then it detects a recursion when mrv(10)
            // becomes "1" (false recursion). So we take self mrvs out.
            auto other_rep = rmap.filterMap([](const Map &sbgmap) {
              return notEqId(sbgmap);
            }).dom();
            // Vertices in the set-vertex that share its rep with other vertex
            // in the set-vertex
            Set VR = rmap.restrict(vs.intersection(other_rep)).sharedImage();
            // There is a recursive vertex that changed its rep in the last step
            // (to avoid computing again an already found recursion)
            if (!VR.intersection(Vc).isEmpty()) {
              // Vertices that reach the shared representative
              Set repV = rmap.preImage(rmap.image(VR));
              Set end = VR.difference(Vc);

              // Edges with both endings in VR (path to a minimum rep)
              Set ERB = mapB.preImage(repV), ERD = mapD.preImage(repV);
              Set ER = ERB.intersection(ERD);
              if (!end.isEmpty() && !ER.isEmpty()) {
                // Distance map
                PWMap dmap = fact_.createPWMap();
                Set ith = end;
                NAT dist = 0;
                // Calculate distance for vertices in same_rep that reach reps
                for (; dmap.dom().intersection(Vc.intersection(VR)).isEmpty();) {
                  Set dom = ith.difference(dmap.dom());
                  Exp exp(MD_NAT(copies, dist));
                  dmap.emplaceBack(fact_.createMap(dom, exp));
                  // Update ith to vertices that have outgoing edges entering ith
                  ith = mapB.image(mapD.preImage(ith));
                  ++dist;
                }
                PWMap dmapB = dmap.composition(mapB), dmapD = dmap.composition(mapD);
                // Get edges where the end is closer to the rep than the beginning
                Set not_cycle_edges = (dmapB - dmapD).preImage(positive);
                ER = ER.intersection(not_cycle_edges);

                // Extend to subset-edge
                Set ER_plus = subEmap.preImage(subEmap.image(ER));
                // Calculate a succesor
                PWMap auxB = mapB.restrict(ER_plus), auxD = mapD.restrict(ER_plus);
                PWMap smap_plus = rmap.restrict(VR);
                smap_plus = smap_plus.combine(auxB.minAdjMap(auxD));

                // Update rmap for recursion, and leave the rest unchanged
                rec_rmap = smap_plus.combine(rec_rmap).compact();

                if (debug()) {
                  Util::SBG_LOG << "VR: " << VR << "\n";
                  Util::SBG_LOG << "repV: " << repV << "\n";
                  Util::SBG_LOG << "ER: " << ER << "\n";
                  Util::SBG_LOG << "dmap: " << dmap << "\n";
                  Util::SBG_LOG << "not_cycle_edges: " << not_cycle_edges << "\n";
                  Util::SBG_LOG << "ER_plus: " << ER_plus << "\n";
                  Util::SBG_LOG << "smap_plus: " << smap_plus << "\n";
                  Util::SBG_LOG << "rec_rmap: " << rec_rmap << "\n\n";
                }
              }
            }
          }
        }
        PWMap rmap_plus = rec_rmap.combine(rmap);
        rmap_plus = rmap_plus.mapInf();
        rmap = rmap.minMap(rmap_plus).compact();

        if (debug())
          Util::SBG_LOG << "rmap after rec: " << rmap << "\n\n";
      }
    } while (!Vc.isEmpty()); 

    return rmap;
  }

  return fact_.createPWMap();
}

PWMap SBGSCC::sccStep()
{
  PWMap id_V = fact_.createPWMap(V());

  DSBG aux_dsbg(
    fact_, V(), Vmap()
    , mapB().restrict(E()), mapD().restrict(E()), Emap().restrict(E()).compact()
    , subEmap().restrict(E())
  );
  PWMap new_rmap = sccMinReach(aux_dsbg);
  if (debug())
    Util::SBG_LOG << "SCC new_rmap: " << new_rmap << "\n";

  PWMap rmap_B = new_rmap.composition(mapB());
  PWMap rmap_D = new_rmap.composition(mapD());
  Set Esame = rmap_B.equalImage(rmap_D); // Edges in the same SCC
  
  // Leave edges in the same SCC
  Ediff_ = E().difference(Esame);
  E_ = Esame;
  if (debug())
    Util::SBG_LOG << "SCC erased edges: " << Ediff() << "\n\n";

  // Swap directions
  PWMap aux_B = mapB();
  mapB_ = mapD();
  mapD_ = aux_B;

  return new_rmap;
}

PWMap SBGSCC::calculate()
{
  if (debug())
    Util::SBG_LOG << "SCC dsbg: \n" << dsbg() << "\n\n";

  auto begin = std::chrono::high_resolution_clock::now();
  PWMap rmap = sccStep();
  do {
    rmap = sccStep();
  } while (Ediff() != fact_.createSet());
  rmap_ = rmap.compact();
  auto end = std::chrono::high_resolution_clock::now();

  auto total = std::chrono::duration_cast<std::chrono::microseconds>(
    end - begin
  );
  Util::SBG_LOG << "Total SCC exec time: " << total.count() << " [μs]\n\n"; 

  if (debug())
    Util::SBG_LOG << "SCC result: " << rmap.compact() << "\n\n";

  return rmap.compact();
}

const PWMapAF &SBGSCC::fact() const { return fact_; }

////////////////////////////////////////////////////////////////////////////////
// Tearing ---------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

SBGTearing::SBGTearing(const DSBG &dsbg, bool debug)
    : fact_(dsbg.fact()),
      dsbg_(dsbg),
      V_(fact_.createSet()),
      Vmap_(fact_.createPWMap()),
      Emap_(fact_.createPWMap()),
      subEmap_(fact_.createPWMap()),
      E_(fact_.createSet()),
      Ediff_(fact_.createSet()),
      mapB_(fact_.createPWMap()),
      mapD_(fact_.createPWMap()),
      rmap_(fact_.createPWMap()),
      debug_(debug)
{
  DSBG dg = dsbg;
  dsbg_ = dg;

  V_ = dg.V();
  Vmap_ = dg.Vmap();

  E_ = dg.E();
  Emap_ = dg.Emap();
  subEmap_ = dg.subEmap();

  mapB_ = dg.mapB();
  mapD_ = dg.mapD();

  rmap_ = fact_.createPWMap(V_);
}

member_imp(SBGTearing, DSBG, dsbg);
member_imp(SBGTearing, Set, V);
member_imp(SBGTearing, PWMap, Vmap);
member_imp(SBGTearing, Set, E);
member_imp(SBGTearing, PWMap, Emap);
member_imp(SBGTearing, PWMap, subEmap);

member_imp(SBGTearing, PWMap, mapB);
member_imp(SBGTearing, PWMap, mapD);

member_imp(SBGTearing, Set, Ediff);

member_imp(SBGTearing, PWMap, rmap);

member_imp(SBGTearing, bool, debug);

PWMap SBGTearing::sccMinReach(const DSBG &dg) const
{
  if (debug()) Util::SBG_LOG << "Min reach graph:\n" << dg << "\n\n";

  Set V = dg.V(), E = dg.E();
  PWMap mapB = dg.mapB(), mapD = dg.mapD(), subEmap = dg.subEmap();
  if (!V.isEmpty()) {
    unsigned int copies = V.arity();
    PWMap rmap = fact_.createPWMap(V), old_rmap = fact_.createPWMap();

    if (E.isEmpty()) return rmap;

    Set Vc = fact_.createSet();
    do {
      old_rmap = rmap;

      PWMap ermapD = rmap.composition(mapD);

      PWMap new_rmap = mapB.minAdjMap(ermapD);
      rmap = rmap.minMap(new_rmap).combine(rmap);
      if (debug()) Util::SBG_LOG << "rmap before rec: " << rmap << "\n\n";

      Set positive = fact_.createSet(SetPiece(copies, Interval(1, 1, Inf)));
      PWMap rec_rmap = fact_.createPWMap();
      Vc = V.difference(old_rmap.equalImage(rmap));
      if (!Vc.isEmpty()) {
        for (const Map &subv : dg.Vmap()) {
          Set vs = subv.dom();
          if (!vs.intersection(Vc).isEmpty()) {
            // If the mrv is in the same SV, the algorithm would detect a false
            // recursion, i.e. if we have a cycle 1 -> 2 -> ... -> 10 -> 1,
            // where SV = [1:10], then it detects a recursion when mrv(10)
            // becomes "1" (false recursion). So we take self mrvs out.
            auto other_rep = rmap.filterMap([](const Map &sbgmap) { return notEqId(sbgmap); }).dom();
            // Vertices in the set-vertex that share its rep with other vertex
            // in the set-vertex
            Set VR = rmap.restrict(vs.intersection(other_rep)).sharedImage();
            // There is a recursive vertex that changed its rep in the last step
            // (to avoid computing again an already found recursion)
            if (!VR.intersection(Vc).isEmpty()) {
              // Vertices that reach the shared representative
              Set repV = rmap.preImage(rmap.image(VR));
              Set end = VR.difference(Vc);

              // Edges with both endings in VR (path to a minimum rep)
              Set ERB = mapB.preImage(repV), ERD = mapD.preImage(repV);
              Set ER = ERB.intersection(ERD);
              if (!end.isEmpty() && !ER.isEmpty()) {
                // Distance map
                PWMap dmap = fact_.createPWMap();
                Set ith = end;
                NAT dist = 0;
                // Calculate distance for vertices in same_rep that reach reps
                for (; dmap.dom().intersection(Vc.intersection(VR)).isEmpty();) {
                  Set dom = ith.difference(dmap.dom());
                  Exp exp(MD_NAT(copies, dist));
                  dmap.emplaceBack(fact_.createMap(dom, exp));
                  // Update ith to vertices that have outgoing edges entering ith
                  ith = mapB.image(mapD.preImage(ith));
                  ++dist;
                }
                PWMap dmapB = dmap.composition(mapB), dmapD = dmap.composition(mapD);
                // Get edges where the end is closer to the rep than the beginning
                Set not_cycle_edges = (dmapB - dmapD).preImage(positive);
                ER = ER.intersection(not_cycle_edges);

                // Extend to subset-edge
                Set ER_plus = subEmap.preImage(subEmap.image(ER));
                // Calculate a succesor
                PWMap auxB = mapB.restrict(ER_plus), auxD = mapD.restrict(ER_plus);
                PWMap smap_plus = rmap.restrict(VR);
                smap_plus = smap_plus.combine(auxB.minAdjMap(auxD));

                // Update rmap for recursion, and leave the rest unchanged
                rec_rmap = smap_plus.combine(rec_rmap).compact();

                if (debug()) {
                  Util::SBG_LOG << "VR: " << VR << "\n";
                  Util::SBG_LOG << "repV: " << repV << "\n";
                  Util::SBG_LOG << "ER: " << ER << "\n";
                  Util::SBG_LOG << "dmap: " << dmap << "\n";
                  Util::SBG_LOG << "not_cycle_edges: " << not_cycle_edges << "\n";
                  Util::SBG_LOG << "ER_plus: " << ER_plus << "\n";
                  Util::SBG_LOG << "smap_plus: " << smap_plus << "\n";
                  Util::SBG_LOG << "rec_rmap: " << rec_rmap << "\n\n";
                }
              }
            }
          }
        }
        PWMap rmap_plus = rec_rmap.combine(rmap);
        rmap_plus = rmap_plus.mapInf();
        rmap = rmap.minMap(rmap_plus).compact();

        if (debug()) Util::SBG_LOG << "rmap after rec: " << rmap << "\n\n";
      }
    } while (!Vc.isEmpty());

    return rmap;
  }

  return fact_.createPWMap();
}

PWMap SBGTearing::sccStep()
{
  PWMap id_V = fact_.createPWMap(V());

  DSBG aux_dsbg(fact_, V(), Vmap(), mapB().restrict(E()), mapD().restrict(E()), Emap().restrict(E()).compact(), subEmap().restrict(E()));
  PWMap new_rmap = sccMinReach(aux_dsbg);
  if (debug()) Util::SBG_LOG << "SCC new_rmap: " << new_rmap << "\n";

  PWMap rmap_B = new_rmap.composition(mapB());
  PWMap rmap_D = new_rmap.composition(mapD());
  Set Esame = rmap_B.equalImage(rmap_D);  // Edges in the same SCC

  // Leave edges in the same SCC
  Ediff_ = E().difference(Esame);
  E_ = Esame;
  if (debug()) Util::SBG_LOG << "SCC erased edges: " << Ediff() << "\n\n";

  // Swap directions
  PWMap aux_B = mapB();
  mapB_ = mapD();
  mapD_ = aux_B;

  return new_rmap;
}

PWMap SBGTearing::calculate()
{
  if (debug()) Util::SBG_LOG << "SCC dsbg: \n" << dsbg() << "\n\n";

  auto begin = std::chrono::high_resolution_clock::now();
  PWMap rmap = sccStep();
  Set vrem = fact_.createSet();
  do {
    do {
      rmap = sccStep();
    } while (Ediff() != fact_.createSet());

    if (E() != fact_.createSet()) {
      Set erem_b = mapB().preImage(rmap.image());
      Set erem_d = mapD().preImage(rmap.image());
      vrem = vrem.cup(mapB().image(erem_b)).cup(mapD().image(erem_d));
      std::cout <<  erem_b.cup(erem_d) << std::endl;
      E_ = E().difference(erem_b.cup(erem_d));
    }
    rmap_ = rmap.compact();
  } while (E() != fact_.createSet());
  auto end = std::chrono::high_resolution_clock::now();

  auto total = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
  Util::SBG_LOG << "Total SCC exec time: " << total.count() << " [μs]\n\n";

  if (debug()) {
    Util::SBG_LOG << "Tearing result: " << rmap.compact() << "\n\n";
    E().print(std::cout);
  }

  return rmap.compact();
}

const PWMapAF &SBGTearing::fact() const { return fact_; }

////////////////////////////////////////////////////////////////////////////////
// Topological sort ------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

SBGTopSort::SBGTopSort(const DSBG &dsbg, bool debug) 
  : fact_(dsbg.fact()), dsbg_(dsbg), debug_(debug) {}

member_imp(SBGTopSort, DSBG, dsbg);
member_imp(SBGTopSort, bool, debug);

Exp SBGTopSort::calculateExp(const MD_NAT &from, const MD_NAT & to)
{
  Exp res;

  RATIONAL one(1, 1);
  for (unsigned int j = 0; j < from.arity(); ++j) { 
    RATIONAL r_from(from[j]), r_to(to[j]);
    res.emplaceBack(LExp(1, r_to - r_from));
  }

  return res;
}

PWMap SBGTopSort::calculate()
{
  if (debug())
    Util::SBG_LOG << "Topological sort dsbg:\n" << dsbg() << "\n\n";

  auto begin = std::chrono::high_resolution_clock::now();
  PWMap mapB = dsbg().mapB(), mapD = dsbg().mapD(), Vmap = dsbg().Vmap();
  PWMap smap = fact_.createPWMap();
  Set U = dsbg().V(), Nd = U.difference(mapB.image());
  if (!Nd.isEmpty()) {
    MD_NAT vsucc = Nd.minElem();
    Set SV = fact_.createSet(), E = dsbg().E();
    do {
      Set vsucc_set = fact_.createSet(SetPiece(vsucc));
      Set Nd_vsucc = Nd.intersection(Vmap.preImage(Vmap.image(vsucc_set)));
      MD_NAT v = Nd.minElem();
      if (!Nd_vsucc.isEmpty())
        v = Nd_vsucc.minElem();
      Set d = fact_.createSet(v);
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
      smap.emplaceBack(fact_.createMap(d, e));
      
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
  }
  auto end = std::chrono::high_resolution_clock::now();

  auto total = std::chrono::duration_cast<std::chrono::microseconds>(
    end - begin
  );
  Util::SBG_LOG << "Total topological sort exec time: " << total.count() << " [μs]\n\n"; 

  if (debug())
    Util::SBG_LOG << "Topological sort result:\n" << smap.compact() << "\n\n";

  return smap.compact();
}

const PWMapAF &SBGTopSort::fact() const { return fact_; }

////////////////////////////////////////////////////////////////////////////////
// Cut-set algorithm -----------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

SBGCutSet::SBGCutSet(const DSBG &dsbg, bool debug) 
  : fact_(dsbg.fact()), dsbg_(dsbg), debug_(debug) {}

member_imp(SBGCutSet, DSBG, dsbg);
member_imp(SBGCutSet, bool, debug);

PWMap SBGCutSet::getDegMap(const DSBG &dsbg)
{
  Set V = dsbg.V();
  PWMap mapB = dsbg.mapB(), mapD = dsbg.mapD();

  auto dims = V.arity();
  MD_NAT zero(dims, 0), one(dims, 1);
  PWMap dmap = fact_.createPWMap(fact_.createMap(V, Exp(zero))); 

  for (const Map &SE : dsbg.subEmap()) {
    Set dom = SE.dom();
    MD_NAT n(dims, dom.cardinal());
    Set vsB = mapB.image(dom), vsD = mapD.image(dom);
    if (vsB.cardinal() == 1) {
      PWMap ith = dmap.restrict(vsD).offsetImage(n);
      dmap = ith.combine(dmap);
      ith = dmap.restrict(vsB).offsetImage(one);
      dmap = ith.combine(dmap);
    }
    else if (vsD.cardinal() == 1) {
      PWMap ith = dmap.restrict(vsB).offsetImage(n);
      dmap = ith.combine(dmap);
      ith = dmap.restrict(vsD).offsetImage(one);
      dmap = ith.combine(dmap);
    }
    else {
      PWMap ith = dmap.restrict(vsB.cup(vsD)).offsetImage(one);
      dmap = ith.combine(dmap);
    }
  }

  return dmap;
}

Set SBGCutSet::calculate()
{
  auto begin = std::chrono::high_resolution_clock::now();
  DSBG dg = dsbg();
  PWMap Vmap = dg.Vmap(), mapB = dg.mapB(), mapD = dg.mapD();
  PWMap rmap = SBGSCC(dg, debug()).calculate();
  Set newD = fact_.createSet(), oldD = newD, visitedV = newD, V = dg.V();
  if (debug())
    Util::SBG_LOG << "initial dg vertex cut set:\n" << dg << "\n";

  PWMap dmap = getDegMap(dg);
  // Degree map
  if (debug())
    Util::SBG_LOG << "initial dmap: " << dmap << "\n\n";

  while (rmap.dom() != rmap.image()) {
    oldD = newD;
 
    MD_NAT aux = dmap.image().maxElem();
    MD_NAT vmax = dmap.preImage(fact_.createSet(aux)).minElem();
    Set vmaxSV = Vmap.image(fact_.createSet(vmax));
    newD = newD.cup(fact_.createSet(vmax));

    if (!visitedV.intersection(vmaxSV).isEmpty()) {
      MD_NAT vmaxD = newD.intersection(Vmap.preImage(vmaxSV)).minElem();

      SetPiece mdi;
      for (unsigned int j = 0; j < vmax.arity(); ++j) {
        if (vmax[j] < vmaxD[j]) {
          NAT st = vmaxD[j] - vmax[j];
          NAT beg = vmax[j] % st;
          mdi.emplaceBack(Interval(beg, st, vmax[j]));
        }

        else
          mdi.emplaceBack(Interval(vmaxD[j], vmax[j] - vmaxD[j], Inf));    
      }
      if (debug()) {
        Util::SBG_LOG << "vmax: " << vmax << "\n"; 
        Util::SBG_LOG << "vmaxD: " << vmaxD << "\n"; 
        Util::SBG_LOG << "mdi: " << mdi << "\n\n";
      }
      Set mdi_set = fact_.createSet(mdi);
      newD = newD.cup(mdi_set.intersection(Vmap.preImage(vmaxSV)));
    }

    // Update graph info erasing newD vertices
    visitedV = visitedV.cup(vmaxSV);
    dg = dg.eraseVertices(newD);
    Vmap = dg.Vmap();
    mapB = dg.mapB(); 
    mapD = dg.mapD(); 

    // Update degree map
    dmap = getDegMap(dg);

    // Resulting SCC from induced graph
    rmap = SBGSCC(dg, debug()).calculate();

    if (debug()) {
      Util::SBG_LOG << "oldD: " << oldD << "\n";
      Util::SBG_LOG << "newD: " << newD << "\n";
      Util::SBG_LOG << "resulting graph:\n" << dg << "\n";
      Util::SBG_LOG << "new degree map: " << dmap << "\n";
      Util::SBG_LOG << "new rmap: " << rmap << "\n\n";
    }
  }
  auto end = std::chrono::high_resolution_clock::now();

  auto total = std::chrono::duration_cast<std::chrono::microseconds>(
    end - begin
  );
  Util::SBG_LOG << "Total vertex cut set exec time: " << total.count() << " [μs]\n\n"; 

  return newD.compact();
}

////////////////////////////////////////////////////////////////////////////////
// Additional operations -------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

DSBG buildSCCFromMatching(const SBGMatching &match)
{
  const PWMapAF &fact = match.fact();

  auto start = std::chrono::high_resolution_clock::now();
  Set matched_edges = match.matched_E(), unmatched_edges = match.unmatched_E();

  Set V = matched_edges.compact();
  PWMap auxVmap = match.sbg().subEmap().restrict(matched_edges);
  PWMap Vmap = fact.createPWMap();
  for (const Map &map : auxVmap) 
    Vmap.emplaceBack(fact.createMap(map.dom().compact(), map.exp()));

  PWMap matchedF_inv = match.mapF().restrict(matched_edges).inverse();
  PWMap unmatchedF = match.mapF().restrict(unmatched_edges);
  PWMap mapB = matchedF_inv.composition(unmatchedF);
  mapB = mapB.compact();
  PWMap matchedU_inv = match.mapU().restrict(matched_edges).inverse();
  PWMap unmatchedU = match.mapU().restrict(unmatched_edges);
  PWMap mapD = matchedU_inv.composition(unmatchedU);
  mapD = mapD.compact();

  PWMap Emap = match.Emap().restrict(unmatched_edges);
  PWMap subEmap = match.subEmap().restrict(unmatched_edges);

  DSBG res(fact, V, Vmap, mapB, mapD, Emap, subEmap);
  auto end = std::chrono::high_resolution_clock::now();
  auto total = std::chrono::duration_cast<std::chrono::microseconds>(
    end - start 
  );
  Util::SBG_LOG << "SBG SCC builder: " << total.count() << " [μs]\n\n"; 

  return res;
}

DSBG buildSortFromSCC(
  const SBGSCC &scc, const PWMap &rmap
)
{
  DSBG dsbg = scc.dsbg();
  Set Ediff = dsbg.E().difference(scc.E());
  PWMap mapB = rmap.composition(dsbg.mapB().restrict(Ediff));
  mapB = mapB.compact();
  PWMap mapD = rmap.composition(dsbg.mapD().restrict(Ediff));
  mapD = mapD.compact();

  PWMap aux_rmap = rmap.compact();
  PWMap reps_rmap = aux_rmap.filterMap([](const Map &sbgmap) {
    return eqId(sbgmap);
  });
  Set V = reps_rmap.dom();

  PWMap Vmap = dsbg.Vmap().restrict(V);

  PWMap Emap = dsbg.Emap().restrict(Ediff);
  PWMap subEmap = dsbg.subEmap().restrict(Ediff);

  DSBG res(dsbg.fact(), V, Vmap, mapB, mapD, Emap, subEmap);
  return res;
}

////////////////////////////////////////////////////////////////////////////////
// JSON output -----------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

rapidjson::Value setJson(const Set &s
  , rapidjson::Document::AllocatorType &alloc)
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

rapidjson::Value mapJson(
  const PWMap &pw, rapidjson::Document::AllocatorType &alloc
)
{
  rapidjson::Value res(rapidjson::kArrayType);

  for (const Map &map : pw) {
    rapidjson::Value ith(rapidjson::kObjectType);

    ith.AddMember("dom", setJson(map.dom(), alloc), alloc);
    ith.AddMember("exp", expJson(map.exp(), alloc), alloc);

    res.PushBack(ith, alloc);
  }

  return res;
}

void buildJson(const Set &matching, const PWMap &scc, const PWMap &order)
{
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

} // namespace LIB

} // namespace SBG
