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

#include <chrono>

#include "algorithms/matching/matching.hpp"
#include "util/logger.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Matching --------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

// Minimum reachable -----------------------------------------------------------

PWMap BFSMatching::directedOffset(const PWMap &dir_map) const
{
  if (debug())
    Util::SBG_LOG << "directedOffset unmatched_V: " << unmatched_V() << "\n\n";
  Set unmatched_side = dir_map.image(paths_edges());
  unmatched_side = unmatched_side.intersection(unmatched_V());
  PWMap res = omap().restrict(unmatched_side);
  res = res.offsetImage(max_V());

  return res.combine(omap()).compact();
}

DSBG BFSMatching::offsetGraph(const PWMap &dir_omap) const
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

void BFSMatching::selectSucc(DSBG dsbg)
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

void BFSMatching::directedMinReach(const PWMap &dir_map)
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

void BFSMatching::minReachableStep()
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

void BFSMatching::minReachable()
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

BFSMatching::BFSMatching(const SBG &sbg, bool debug)
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

member_imp(BFSMatching, SBG, sbg);
member_imp(BFSMatching, Set, V);
member_imp(BFSMatching, PWMap, Vmap);
member_imp(BFSMatching, Set, E);
member_imp(BFSMatching, PWMap, Emap);
member_imp(BFSMatching, PWMap, subEmap);

member_imp(BFSMatching, PWMap, smap);
member_imp(BFSMatching, PWMap, rmap);

member_imp(BFSMatching, PWMap, omap);
member_imp(BFSMatching, MD_NAT, max_V);

member_imp(BFSMatching, Set, F);
member_imp(BFSMatching, Set, U);
member_imp(BFSMatching, PWMap, mapF);
member_imp(BFSMatching, PWMap, mapU);

member_imp(BFSMatching, PWMap, mapB);
member_imp(BFSMatching, PWMap, mapD);

member_imp(BFSMatching, Set, paths_edges);
member_imp(BFSMatching, Set, matched_E);
member_imp(BFSMatching, Set, unmatched_E);

member_imp(BFSMatching, Set, matched_V);
member_imp(BFSMatching, Set, unmatched_V);
member_imp(BFSMatching, Set, unmatched_F);
member_imp(BFSMatching, Set, matched_U);
member_imp(BFSMatching, Set, unmatched_U);

member_imp(BFSMatching, bool, debug);

Set BFSMatching::edgesInPaths() const
{
  // Vertices that are successors of other vertices in a path
  Set not_fixed = smap().dom().difference(smap().fixedPoints());
  Set succs = smap().restrict(not_fixed).image();
  // Edges whose endings are successors 
  Set ending_edges = mapD().preImage(succs);
  // Map from a 'successor' edge to its start
  PWMap auxB = mapB().restrict(ending_edges);
  // Map from edge to the successor of its start
  PWMap map_succs = smap().composition(auxB);
 
  return map_succs.equalImage(mapD());
}

Set BFSMatching::edgesSameRepLR(const PWMap &rmaprmapd) const
{
  Set not_fixed = rmap().dom().difference(rmap().fixedPoints());
  PWMap rmap_neq_id = rmap().restrict(not_fixed);
  PWMap rmapb = rmap_neq_id.composition(mapB());

  not_fixed = rmaprmapd.dom().difference(rmaprmapd.fixedPoints());
  PWMap rmaprmapd_neq_id = rmaprmapd.restrict(not_fixed);
  PWMap rmaprmapdb = rmaprmapd_neq_id.composition(mapB());
  
  return rmapb.equalImage(rmaprmapdb);
}

bool BFSMatching::fullyMatchedU() const
{
  return U().difference(matched_U()).isEmpty();
}

void BFSMatching::updatePaths()
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

void BFSMatching::updateOffset()
{
  PWMap aux = omap().restrict(matched_V());
  aux = aux.offsetImage(max_V());
  aux = aux.offsetImage(max_V());
  omap_ = aux.combine(omap());

  return;
}

MatchInfo BFSMatching::calculate()
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
    end - begin);
  Util::SBG_LOG << "Total match exec time: " << total.count() << " [μs]\n";

  return MatchInfo(matched_E().compact(), fullyMatchedU());
}

const PWMapAF &BFSMatching::fact() const { return fact_; }

} // namespace LIB

} // namespace SBG
