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
#pragma once

#include <iostream>

#include <boost/foreach.hpp>
#include <boost/unordered_set.hpp>

#include <sbg/sbg.hpp>

using namespace SBG;

// Connected components of undirected SBGraphs ---------------------------------------------------

PWLMap connectedComponents(SBGraph g);

// Minimum reachable -----------------------------------------------------------------------------

std::pair<PWLMap, PWLMap> recursion(int n, Set ER, Set V, Set E, PWLMap Emap, PWLMap map_D, PWLMap map_B, PWLMap currentSmap,
                                    PWLMap currentSEmap, PWLMap currentRmap, PWLMap m_map);
PWLMap minReach1(Set V, PWLMap map_D, PWLMap map_B, PWLMap currentSmap, PWLMap currentRmap);
std::tuple<PWLMap, PWLMap, PWLMap> minReachable(int nmax, Set V, Set E, PWLMap Vmap, PWLMap Emap, PWLMap map_D, PWLMap map_B, PWLMap m_map);

// Matching of undirected SBGraphs ---------------------------------------------------------------

struct MatchingStruct {
  MatchingStruct(SBGraph g);

  member_class(SBGraph, g);
  member_class(Set, F);
  member_class(Set, U);
  member_class(Set, initU);
  member_class(PWLMap, Vmap);  // Map from vertices to set-vertices
  member_class(PWLMap, Emap);  // Map from edges to set-edges

  member_class(Set, allEdges);
  member_class(Set, Ed);  // Allowed edges in each step
  member_class(Set, allVertices);
  member_class(int, nmax);         // Number of set-vertices of the SBG
  member_class(OrdCT<INT>, maxV);  // Value of maximum vertex of the SBG

  member_class(PWLMap, mapF);  // "Left" maps
  member_class(PWLMap, mapU);  // "Right" maps
  member_class(PWLMap, mapD);  // Forward direction
  member_class(PWLMap, mapB);  // Backward direction

  member_class(Set, matchedV);
  member_class(Set, unmatchedV);
  member_class(Set, matchedE);

  member_class(PWLMap, smap);   // Successors map
  member_class(PWLMap, semap);  // Edge's successors map
  member_class(PWLMap, rmap);   // Representatives map
  member_class(PWLMap, mmap);   // Offset map

  // Auxiliary maps and sets, that are used to offset vertices for the search
  member_class(Set, VSide);
  member_class(PWLMap, mmapSide);
  member_class(PWLMap, mmapSideInv);
  member_class(PWLMap, VmapSide);
  member_class(PWLMap, mapDSide);
  member_class(PWLMap, mapBSide);

  Set getManyToOne();
  void offsetMaps(PWLMap sideMap);
  void shortPathsLeft(Set D, Set E);
  void shortPathsRight(Set D, Set E);
  void directedMinReach(PWLMap sideMap);
  void SBGMatchingShortStep(Set E);
  void SBGMatchingMinStep(Set E);
  void SBGMatchingShort();
  void SBGMatchingMin();
  std::pair<Set, bool> SBGMatching();

  private:
  void debugInit();
  void debugStep();
};

// SCC of directed SBGraphs ----------------------------------------------------------------------

struct SCCStruct {
  SCCStruct(DSBGraph g);

  void SCCStep();
  PWLMap SBGSCC();

  member_class(DSBGraph, g);
  member_class(Set, V);
  member_class(PWLMap, Vmap);  // Map from vertices to set-vertices
  member_class(Set, E);
  member_class(PWLMap, Emap);  // Map from edges to set-edges

  member_class(Set, Ediff);  // Edges between different SCC 

  member_class(PWLMap, mapD);  // Forward direction
  member_class(PWLMap, mapB);  // Backward direction

  member_class(PWLMap, rmap);   // Representatives map

  private:
  void debugInit();
  void debugStep();
};
