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

#include <sbg/sbg.h>

using namespace SBG;

// Connected components of undirected SBGraphs ---------------------------------------------------

PWLMap connectedComponents(SBGraph g);

// Minimum reachable -----------------------------------------------------------------------------

std::pair<PWLMap, PWLMap> recursion(int n, Set ER, Set V, Set E, PWLMap Emap, PWLMap map_D, PWLMap map_B, PWLMap currentSmap,
                                    PWLMap currentSEmap, PWLMap currentRmap);
PWLMap minReach1(Set V, PWLMap map_D, PWLMap map_B, PWLMap currentSmap, PWLMap currentRmap);
std::pair<PWLMap, PWLMap> minReachable(int nmax, Set V, Set E, PWLMap Vmap, PWLMap Emap, PWLMap map_D, PWLMap map_B);

// Matching of undirected SBGraphs ---------------------------------------------------------------

struct MatchingStruct {
  MatchingStruct(SBGraph g);

  void directedMinReach(PWLMap sideMap);
  Set SBGComponentMatching();
  std::pair<Set, bool> SBGMatching();

  private:
  SBGraph g;
  Set F;
  Set U;
  PWLMap Vmap;  // Map from vertices to set-vertices
  PWLMap Emap;  // Map from edges to set-edges

  Set allEdges;
  Set Ed;  // Allowed edges in each step
  Set allVertices;
  int nmax;
  OrdCT<INT> maxV;

  PWLMap mapF;  // "Left" maps
  PWLMap mapU;  // "Right" maps
  PWLMap mapD;  // Forward direction
  PWLMap mapB;  // Backward direction

  Set matchedV;
  Set unmatchedV;
  Set matchedE;

  PWLMap smap;  // Successors map
  PWLMap rmap;  // Representatives map
  PWLMap mmap;  // Offset map

  void debugInit();
  void debugStep();
};
