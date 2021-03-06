/*****************************************************************************

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
#include <boost/graph/subgraph.hpp>
#include <sbg/sbg_algorithms.hpp>
#include <sbg/util/logger.hpp>

#include <chrono>
#include <time.h>

/*-----------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------*/
// SB-Graphs algorithms
/*-----------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------*/

// Connected components ---------------------------------------------------------------------------

PWLMap connectedComponents(SBGraph g)
{
  PWLMap res;

  VertexIt vi_start, vi_end;
  boost::tie(vi_start, vi_end) = vertices(g);
  EdgeIt ei_start, ei_end;
  boost::tie(ei_start, ei_end) = edges(g);

  if (vi_start != vi_end /*&& ei_start != ei_end*/) {
    Set vss;
    while (vi_start != vi_end) {
      Set aux = (g[*vi_start]).range();
      vss = vss.cup(aux);

      ++vi_start;
    }

    PWLMap auxres(vss);
    res = auxres;

    if (ei_start == ei_end) return res;

    PWLMap emap1 = (g[*ei_start]).map_f();
    PWLMap emap2 = (g[*ei_start]).map_u();
    ++ei_start;

    while (ei_start != ei_end) {
      emap1 = (g[*ei_start]).map_f_ref().combine(emap1);
      emap2 = (g[*ei_start]).map_u_ref().combine(emap2);

      ++ei_start;
    }

    PWLMap oldres;

    while (!oldres.equivalentPW(res)) {
      PWLMap ermap1 = res.compPW(emap1);
      PWLMap ermap2 = res.compPW(emap2);

      PWLMap rmap1 = ermap1.minAdjMap(ermap2);
      PWLMap rmap2 = ermap2.minAdjMap(ermap1);
      rmap1 = rmap1.combine(res);
      rmap2 = rmap2.combine(res);

      PWLMap newRes = rmap1.minMap(rmap2);
      oldres = res;
      res = newRes.minMap(res);

      if (!oldres.equivalentPW(res)) {
        res = newRes;
        res = res.mapInf(1);
      }
    }
  }

  return res;
}

// Minimum reachable ------------------------------------------------------------------------------

bool eqId(Set dom, LMap lm)
{
  ORD_REALS::iterator itg = lm.gain_ref().begin();
  ORD_REALS::iterator ito = lm.offset_ref().begin();

  for (int i = 0; i < lm.ndim(); i++)
    if (*itg != 1 || *ito != 0) return false;

  return true;
}

bool notEqId(Set dom, LMap lm)
{
  ORD_REALS::iterator itg = lm.gain_ref().begin();
  ORD_REALS::iterator ito = lm.offset_ref().begin();

  for (int i = 0; i < lm.ndim(); i++)
    if (*itg != 1 || *ito != 0) return true;

  return false;
}

// Use result of newMap if offMap(newMap(v)) < offMap(currentMap(v))
// (In the minReachable context, is used to select a new successor
//  only if the new successor has a "<" representative than the old
//  successor)
PWLMap updateMap(Set V, PWLMap currentMap, PWLMap newMap, PWLMap offMap)
{
  Interval i(0, 1, 0);
  MultiInterval mi;
  for (int j = 0; j < currentMap.ndim(); j++) mi.addInter(i);
  Set zero = createSet(mi);

  PWLMap offCurrent = offMap.compPW(currentMap);
  PWLMap offNew = offMap.compPW(newMap);

  PWLMap deltaMap = offCurrent.diffMap(offNew);
  Set notUpdateVerts = deltaMap.preImage(zero);
  Set updateVerts = V.diff(notUpdateVerts);

  PWLMap resMap = newMap.restrictMap(updateVerts);
  resMap = resMap.combine(currentMap.restrictMap(notUpdateVerts));

  return resMap;
}

// Solve recursion in constant time. For recursive paths
std::pair<PWLMap, PWLMap> recursion(int n, Set ER, Set V, Set E, PWLMap Emap, PWLMap map_D, PWLMap map_B, PWLMap currentSmap,
                                    PWLMap currentSEmap, PWLMap currentRmap, PWLMap m_map)
{
  // *** Initialization
  LOG << "ER: " << ER << "\n\n";

  PWLMap Vid(V);

  PWLMap m_mapInv = m_map.minInv(m_map.image());

  PWLMap newSmap;
  PWLMap newRmap;  // Minimum reachable map

  Interval i(0, 1, 0);
  MultiInterval mi;
  for (int j = 0; j < map_D.ndim(); j++) mi.addInter(i);
  Set zero = createSet(mi);

  // *** Traverse graph

  PWLMap semapNth = currentSEmap;
  Set Erec = ER;
  PWLMap originalSmap = currentSmap;
  PWLMap originalSEmap = currentSEmap;
  for (int i = 0; i < n; i++) {
    Erec = Erec.cup(semapNth.image(ER));
    semapNth = semapNth.compPW(originalSEmap);
  }

  Set ERplus = Emap.preImage(Emap.image(Erec));  // Edges that share sub-set-edges with those in Erec
  ERplus = ERplus.cap(E);

  PWLMap tildeD = map_D.restrictMap(ERplus);
  PWLMap tildeB = map_B.restrictMap(ERplus);
  PWLMap smapPlus = tildeD.compPW(tildeB.minInv(tildeB.image()));  // Successor map through ERplus

  Set finalsRec = map_B.image(semapNth.image(ER));
  PWLMap finalsRecMap(finalsRec);
  smapPlus = finalsRecMap.combine(smapPlus);  // Preserve the successors of the final vertices

  PWLMap rmapPlus = currentRmap.compPW(smapPlus.mapInf(n));  // Minimum reachable through ERplus
  newSmap = smapPlus.combine(currentSmap);
  newRmap = currentRmap.minMap(rmapPlus);  // New minimum reachable map
  newRmap = newRmap.combine(currentRmap);

  return std::pair<PWLMap, PWLMap>(newSmap, newRmap);
}

// Find min reachable in 1 step
PWLMap minReach1(Set V, PWLMap map_D, PWLMap map_B, PWLMap currentSmap, PWLMap currentRmap)
{
  PWLMap finalSmap;

  Interval i(0, 1, 0);
  MultiInterval mi;
  for (int j = 0; j < map_D.ndim(); j++) mi.addInter(i);
  Set zero = createSet(mi);

  PWLMap Vid(V);

  PWLMap idCurrentSmap = currentSmap.filterMap(eqId);
  Set idVerts = idCurrentSmap.diffMap(currentRmap).preImage(zero);
  Set usableVerts = idVerts.cup(currentSmap.filterMap(notEqId).wholeDom());

  PWLMap adjSmap = map_B.minAdjMap(map_D, currentRmap);  // Get minimum adjacent vertex with minimum representative
  PWLMap newSmap = adjSmap.minMap(currentSmap.restrictMap(usableVerts), currentRmap);  // Old successor vs new successor
  newSmap = updateMap(V, currentSmap, newSmap, currentRmap);                           // Old succesor vs new successor
  newSmap = newSmap.combine(adjSmap);
  newSmap = newSmap.combine(currentSmap);

  return newSmap;
}

// Minimum reachable in several steps
std::tuple<PWLMap, PWLMap, PWLMap> minReachable(int nmax, Set V, Set E, PWLMap Vmap, PWLMap Emap, PWLMap map_D, PWLMap map_B, PWLMap m_map)
{
  // *** Initialization

  PWLMap newSmap;  // Successors map
  PWLMap newRmap;  // Minimum reachable map

  Interval i(0, 1, 0);
  MultiInterval mi;
  for (int j = 0; j < map_D.ndim(); j++) mi.addInter(i);
  Set zero = createSet(mi);

  map_D = map_D.restrictMap(E);
  map_B = map_B.restrictMap(E);

  PWLMap oldSmap;
  PWLMap oldSEmap;

  PWLMap Vid(V);
  newRmap = Vid;
  newSmap = Vid;

  PWLMap Eid(E);
  PWLMap newSEmap = Eid;

  PWLMap m_mapInv = m_map.minInv(m_map.image());

  Set Vc;  // Vertices that changed its successor
  Set Ec;  // Edges that changed its successor

  // *** Traverse graph
  do {
    oldSmap = newSmap;
    newSmap = minReach1(V, map_D, map_B, newSmap, newRmap);  // Get successor
    newRmap = newSmap.mapInf(1);                             // Get representative

    PWLMap deltaSmap = newSmap.diffMap(oldSmap);
    Vc = V.diff(deltaSmap.preImage(zero));  // Vertices that changed its successor

    if (!Vc.empty()) {
      oldSEmap = newSEmap;
      Set Esucc = newSmap.compPW(map_B).diffMap(map_D).preImage(zero);  // Edges that connect vertices with successors
      newSEmap = (map_B.restrictMap(Esucc).minInv(newSmap.image())).compPW(map_D.restrictMap(Esucc));
      newSEmap = newSEmap.combine(oldSEmap);

      PWLMap deltaSEmap = newSEmap.diffMap(oldSEmap);
      Ec = E.diff(deltaSEmap.preImage(zero));

      int n = 1;
      Set ER;  // Recursive edges that changed its successor
      PWLMap se2map = newSEmap.compPW(newSEmap);
      PWLMap semapNth = se2map;

      Set visitedEdges = newSEmap.wholeDom().cup(newSEmap.image());
      Set oldSubsetEdges, visitedSubsetEdges = Emap.image(visitedEdges);
      do {
        PWLMap deltaEmap = Emap.compPW(semapNth.filterMap(notEqId)).diffMap(Emap);
        ER = deltaEmap.preImage(zero);
        semapNth = semapNth.compPW(newSEmap);

        visitedEdges = newSEmap.image(visitedEdges);
        oldSubsetEdges = visitedSubsetEdges;
        visitedSubsetEdges = Emap.image(visitedEdges);
        n++;
      } while (ER.empty() && oldSubsetEdges.cap(visitedSubsetEdges).empty());

      // *** Handle recursion
      ER = ER.cap(Ec);
      if (!ER.empty()) {
        ER = createSet(*(ER.asets().begin()));

        Set oldSubsetEdge = Emap.image(ER);
        Set succSubsetEdge = Emap.image(newSEmap.image(ER));

        if (oldSubsetEdge.cap(succSubsetEdge).empty()) {
          std::pair<PWLMap, PWLMap> p = recursion(n, ER, V, E, Emap, map_D, map_B, newSmap, newSEmap, newRmap, m_map);
          newRmap = std::get<1>(p);
        }
      }
    }
  } while (!Vc.empty());

  return std::tuple<PWLMap, PWLMap, PWLMap>(newSmap, newSEmap, newRmap);
}

// Matching ---------------------------------------------------------------------------------------

// Initialization
MatchingStruct::MatchingStruct(SBGraph garg)
{
  g = garg;

  BOOST_FOREACH (SetEdgeDesc ei, edges(g)) {
    PWLMap fmap = (g[ei]).map_f();
    PWLMap umap = (g[ei]).map_u();

    mapF = mapF.concat(fmap);
    mapU = mapU.concat(umap);
  }

  mapD = mapF;
  mapB = mapU;

  allEdges = mapF.wholeDom();
  Ed = allEdges;

  Set emptySet;
  allVertices = emptySet;
  F = mapF.image(allEdges);
  U = mapU.image(allEdges);
  initU = U;
  nmax = num_vertices(g);
  int dims = F.ndim();

  PWLMap vmap;
  BOOST_FOREACH (SetVertexDesc vdi, vertices(g)) {
    SetVertex vi = g[vdi];
    allVertices.addAtomSets(vi.range_ref().asets());

    LMap lm;
    for (int i = 0; i < dims; i++) lm.addGO(0, vi.id());
    vmap.addSetLM(vi.range(), lm);
  }
  Vmap = vmap;
  maxV = allVertices.maxElem();

  int eCount = 1;
  PWLMap emap;
  BOOST_FOREACH (SetEdgeDesc edi, edges(g)) {
    SetEdge ei = g[edi];

    BOOST_FOREACH (Set sf, ei.map_f_ref().dom()) {
      BOOST_FOREACH (Set su, ei.map_u_ref().dom()) {
        Set uf = sf.cap(su);

        if (!uf.empty()) {
          LMap lm;
          for (int i = 0; i < dims; i++) lm.addGO(0, eCount);
          emap.addSetLM(uf, lm);

          ++eCount;
        }
      }
    }
  }
  Emap = emap;

  matchedV = emptySet;
  unmatchedV = emptySet;
  matchedE = emptySet;

  PWLMap idMap(allVertices);
  mmap = idMap;

  PWLMap emptyMap;
  smap = emptyMap;
  semap = emptyMap;
  rmap = emptyMap;

  // Auxiliary maps
  VSide = emptySet;
  mmapSide = emptyMap;
  mmapSideInv = emptyMap;
  VmapSide = emptyMap;
  mapDSide = emptyMap;
  mapBSide = emptyMap;
}

// Find sub-set edges with a 1:N connection in some dimension
Set MatchingStruct::getManyToOne()
{
  Set res;

  PWLMap auxD = mapD.atomize();

  OrdCT<Set>::iterator itdom = auxD.dom_ref().begin();
  OrdCT<LMap>::iterator itlmap = auxD.lmap_ref().begin();

  while (itdom != auxD.dom_ref().end()) {
    BOOST_FOREACH (MultiInterval as, (*itdom).asets()) {
      Set sas = createSet(as).cap(Ed);
      PWLMap auxmap;
      auxmap.addSetLM(sas, *itlmap);
      Set oneVertex = auxmap.image();
      Set NVertices = mapB.image(sas);
      if (NVertices.card() > oneVertex.card()) {  // N:1 connection in some dimension
        Set unmatchedNVertices = NVertices.diff(matchedV);
        Set unmatchedOut = mapB.preImage(unmatchedNVertices).cap(sas);
        Set imageOneVertex = mapD.image(unmatchedOut);

        if (unmatchedOut.card() > imageOneVertex.card()) res = res.cup(unmatchedOut);
      }
    }

    ++itdom;
    ++itlmap;
  }

  return res;
}

void MatchingStruct::offsetMaps(PWLMap sideMap)
{
  // Offset unmatched vertices on the opposite side of the search
  Set unmatchedSide = sideMap.image(Ed);
  unmatchedSide = unmatchedSide.cap(unmatchedV);
  PWLMap offsetUS = mmap.restrictMap(unmatchedSide);
  offsetUS = offsetUS.offsetImageMap(maxV);

  mmapSide = offsetUS.combine(mmap);
  VSide = mmapSide.image(allVertices);
  mmapSideInv = mmapSide.minInv(mmapSide.image());

  // Offset image of Vmap, mapD and mapB according to mmapSide
  VmapSide = Vmap.offsetDomMap(mmapSide);
  mapDSide = mmapSide.compPW(mapD);
  mapBSide = mmapSide.compPW(mapB);
}

void MatchingStruct::shortPathsLeft(Set D, Set E)
{
  PWLMap idMap(allVertices);
  PWLMap map_D = mapD.restrictMap(E);
  PWLMap map_B = mapB.restrictMap(E);

  smap = idMap;
  rmap = idMap;

  Set C = D;  // Vertices that reach D
  int k = 0;

  Set P;

  Set allowedEdges = E;

  do {
    map_D = map_D.restrictMap(allowedEdges);
    map_B = map_B.restrictMap(allowedEdges);

    P = map_B.image(map_D.preImage(C)).diff(C);
    Set EP = map_D.preImage(C).cap(map_B.preImage(P));
    PWLMap mapBP = map_B.restrictMap(EP);
    PWLMap Vmin = mapBP.minInv(P);
    PWLMap smapP = map_D.compPW(Vmin);
    smap = smapP.combine(smap);
    PWLMap rmapP = rmap.compPW(smapP);
    rmap = rmapP.combine(rmap);

    C = C.cup(P);
    Set usedSubsetEdges = Emap.compPW(Vmin).image(P);
    allowedEdges = allowedEdges.diff(Emap.preImage(usedSubsetEdges));
    k++;
  } while (!P.empty());
}

void MatchingStruct::shortPathsRight(Set D, Set E)
{
  PWLMap idMap(allVertices);
  PWLMap map_D = mapD.restrictMap(E);
  PWLMap map_B = mapB.restrictMap(E);

  smap = idMap;
  rmap = idMap;

  Set C = D;  // Vertices that reach D
  int k = 0;

  Set P;

  do {
    P = map_B.image(map_D.preImage(C)).diff(C);
    Set EP = map_D.preImage(C).cap(map_B.preImage(P));
    PWLMap mapBP = map_B.restrictMap(EP);
    PWLMap Vmin = mapBP.minInv(P);
    PWLMap smapP = map_D.compPW(Vmin);
    smap = smapP.combine(smap);
    PWLMap rmapP = rmap.compPW(smapP);
    rmap = rmapP.combine(rmap);

    C = C.cup(P);
    k++;
  } while (!P.empty());
}

void MatchingStruct::directedMinReach(PWLMap sideMap)
{
  offsetMaps(sideMap);
  std::tuple<PWLMap, PWLMap, PWLMap> t = minReachable(nmax, VSide, Ed, VmapSide, Emap, mapDSide, mapBSide, mmapSide);
  PWLMap directedSmap = std::get<0>(t);
  smap = mmapSideInv.compPW(directedSmap.compPW(mmapSide));
  semap = std::get<1>(t);
  PWLMap directedRmap = std::get<2>(t);
  rmap = mmapSideInv.compPW(directedRmap.compPW(mmapSide));

  LOG << "smapmin: " << smap << "\n\n";
  LOG << "rmapmin: " << rmap << "\n\n";
}

void MatchingStruct::SBGMatchingShortStep(Set E)
{
  Interval i(0, 1, 0);
  MultiInterval mi;
  for (int j = 0; j < mapF.ndim(); j++) mi.addInter(i);
  Set zero = createSet(mi);

  Ed = E;
  unmatchedV = allVertices.diff(matchedV);

  // *** Forward direction

  shortPathsLeft(unmatchedV.cap(F), Ed);
  PWLMap rmapl = rmap;

  // Leave edges in paths that reach unmatched left vertices
  PWLMap auxB = mapB.restrictMap(mapD.preImage(smap.filterMap(notEqId).image()));
  Ed = smap.compPW(auxB).diffMap(mapD).preImage(zero);  // Edges that connect vertices with successors

  // *** Backward direction

  PWLMap auxMapD = mapD;
  mapD = mapB;
  mapB = auxMapD;
  shortPathsRight(unmatchedV.cap(U), Ed);

  // Leave edges in paths that reach unmatched left and right vertices
  auxB = mapB.restrictMap(mapD.preImage(smap.filterMap(notEqId).image()));
  PWLMap qmap = rmap.compPW(auxB).diffMap(rmap.compPW(rmapl.compPW(auxB)));
  Set edgesInPaths = qmap.preImage(zero);  // Edges in augmenting paths that connect vertices with successors
  Ed = Ed.cap(edgesInPaths);

  mapB = mapD;
  mapD = auxMapD;

  // *** Revert matched and unmatched edges in augmenting paths
  PWLMap mapDEd = mapD.restrictMap(Ed);
  PWLMap mapBEd = mapB.restrictMap(Ed);
  mapD = mapBEd.combine(mapD);
  mapB = mapDEd.combine(mapB);

  // *** Update matched edges and vertices
  matchedE = mapD.preImage(U);
  matchedV = mapD.image(matchedE);
  matchedV = matchedV.cup(mapB.image(matchedE));

  unmatchedV = allVertices.diff(matchedV);
  PWLMap idUnmatched(unmatchedV);

  debugStep();
}

void MatchingStruct::SBGMatchingMinStep(Set E)
{
  Interval i(0, 1, 0);
  MultiInterval mi;
  for (int j = 0; j < mapF.ndim(); j++) mi.addInter(i);
  Set zero = createSet(mi);

  Ed = E;
  unmatchedV = allVertices.diff(matchedV);

  // *** Forward direction

  directedMinReach(mapU);
  PWLMap rmapl = rmap.restrictMap(rmap.preImage(unmatchedV));  // Rmap for vertices that reach unmatched left vertices

  // Leave edges in paths that reach unmatched left vertices
  PWLMap auxB = mapB.restrictMap(mapD.preImage(smap.filterMap(notEqId).image()));
  Set edgesInPaths = smap.compPW(auxB).diffMap(mapD).preImage(zero);  // Edges that connect vertices with successors
  Ed = edgesInPaths;

  // *** Backward direction

  PWLMap auxMapD = mapD;
  mapD = mapB;
  mapB = auxMapD;
  directedMinReach(mapF);

  // Leave edges in paths that reach unmatched left and right vertices
  Set rightU = rmap.preImage(unmatchedV);  // Vertices that reach unmatched right vertices
  rmap = rmap.restrictMap(rightU);
  smap = smap.restrictMap(rightU);

  auxB = mapB.restrictMap(mapD.preImage(smap.filterMap(notEqId).image()));
  PWLMap qmap = rmap.compPW(auxB).diffMap(rmap.compPW(rmapl.compPW(auxB)));
  edgesInPaths = qmap.preImage(zero);  // Edges in augmenting paths that connect vertices with successors
  Ed = Ed.cap(edgesInPaths);

  mapB = mapD;
  mapD = auxMapD;

  // *** Revert matched and unmatched edges in augmenting paths
  PWLMap mapDEd = mapD.restrictMap(Ed);
  PWLMap mapBEd = mapB.restrictMap(Ed);
  mapD = mapBEd.combine(mapD);
  mapB = mapDEd.combine(mapB);

  // *** Update matched edges and vertices
  matchedE = mapD.preImage(U);
  matchedV = mapD.image(matchedE);
  matchedV = matchedV.cup(mapB.image(matchedE));

  // *** Update mmap for matched vertices
  PWLMap auxM = mmap.restrictMap(matchedV);
  auxM = auxM.offsetImageMap(maxV);
  auxM = auxM.offsetImageMap(maxV);
  mmap = auxM.combine(mmap);

  unmatchedV = allVertices.diff(matchedV);
  PWLMap idUnmatched(unmatchedV);
  mmap = idUnmatched.combine(mmap);

  debugStep();
}

void MatchingStruct::SBGMatchingShort()
{
  Set diffMatched, allowedEdges = allEdges.diff(getManyToOne());

  do {
    SBGMatchingShortStep(allowedEdges);
    diffMatched = U.diff(matchedV);
  } while (!diffMatched.empty() && !Ed.empty());

  LOG << "matchedshort: " << matchedE << "\n\n";

  if (diffMatched.empty()) return;

  // Update mmap for matched vertices
  PWLMap auxM = mmap.restrictMap(matchedV);
  auxM = auxM.offsetImageMap(maxV);
  auxM = auxM.offsetImageMap(maxV);
  mmap = auxM.combine(mmap);

  unmatchedV = allVertices.diff(matchedV);
  PWLMap idUnmatched(unmatchedV);
  mmap = idUnmatched.combine(mmap);

  // Didn't find any path, use minreach
  SBGMatchingMin();
}

void MatchingStruct::SBGMatchingMin()
{
  Set diffMatched;

  do {
    SBGMatchingMinStep(allEdges);
    diffMatched = U.diff(matchedV);
  } while (!diffMatched.empty() && !Ed.empty());

  LOG << "matchedmin: " << matchedE << "\n\n";

  return;
}

std::pair<Set, bool> MatchingStruct::SBGMatching()
{
  debugInit();

  SBGMatchingShort();

  matchedE = mapD.preImage(U);
  return std::pair<Set, bool>(matchedE, U.diff(matchedV).empty());
}

void MatchingStruct::debugInit()
{
  LOG << "\n\n";
  BOOST_FOREACH (SetVertexDesc vi, vertices(g)) {
    SetVertex v = g[vi];
    Set vs = v.range();

    Set e1 = mapF.preImage(vs);
    Set e2 = mapU.preImage(vs);

    LOG << "-------\n";
    LOG << v << " | " << e1 << " | " << e2 << "\n";
  }
  LOG << "-------\n\n";

  LOG << "Vmap: " << Vmap << "\n\n";
  LOG << "Emap: " << Emap << "\n\n";

  LOG << "*******************************\n\n";
}

void MatchingStruct::debugStep()
{
  bool diffMatched = U.diff(matchedV).empty();
  LOG << "matchedE: " << matchedE << "\n";
  LOG << "matched all unknowns: " << diffMatched << "\n\n";

  BOOST_FOREACH (MultiInterval as1, matchedE.asets()) {
    BOOST_FOREACH (MultiInterval as2, matchedE.asets()) {
      if (as1 != as2) {
        Set s1 = createSet(as1);
        Set s2 = createSet(as2);

        Set inter1 = mapF.image(s1).cap(mapF.image(s2));
        Set inter2 = mapU.image(s1).cap(mapU.image(s2));

        if (!inter1.empty() || !inter2.empty()) LOG << "adj: " << as1 << " | " << as2 << "\n\n";
      }
    }
  }
}
