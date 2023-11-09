/** @file sbg_algorithms.hpp

 @brief <b>SBG algorithms implementation</b>

 <hr>

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

#ifndef SBG_SBGALGO_HPP
#define SBG_SBGALGO_HPP

#include<chrono>

#include "sbg/sbg.hpp"

namespace SBG {

namespace LIB {

// Connected components --------------------------------------------------------

PWMap connectedComponents(SBGraph g);

// Minimum reachable -----------------------------------------------------------

struct PathInfo {
  member_class(PWMap, succs);
  member_class(PWMap, reps);

  PathInfo();
  PathInfo(PWMap succs, PWMap reps);
};

PWMap minReach1(Set V, Set E, PWMap mapB, PWMap mapD, PWMap smap, PWMap rmap);


struct MinReach {
  member_class(DSBGraph, dsbg);

  MinReach();
  MinReach(DSBGraph dsbg);

  PathInfo recursion(unsigned int n, Set ER, Set not_rv, PWMap smap, PWMap rmap);
  PathInfo calculate(Set unmatched_V);
};

// Matching --------------------------------------------------------------------

enum Direction { forward, backward };

struct MatchInfo {
  member_class(Set, matched_edges);
  member_class(bool, fully_matchedU);

  MatchInfo();
  MatchInfo(Set matched_edges, bool fully_matchedU);
};
std::ostream &operator<<(std::ostream &out, const MatchInfo &m_info);

struct SBGMatching {
  //*** SBG info, constant
  member_class(SBGraph, sbg);

  member_class(Set, V);
  member_class(PWMap, Vmap);

  member_class(Set, E);
  member_class(PWMap, Emap);
  //-----------------------------

  member_class(PWMap, smap); // Successors map
  member_class(PWMap, rmap); // Representatives map

  member_class(PWMap, omap); // Offset map
  member_class(Util::MD_NAT, max_V); // Current maximum value

  member_class(Set, F); // Left vertices, constant
  member_class(Set, U); // Right vertices, constant
  member_class(PWMap, mapF); // Left map, constant
  member_class(PWMap, mapU); // Forward map, constant

  member_class(PWMap, mapB); // Backward map, mutable
  member_class(PWMap, mapD); // Forward map, mutable

  member_class(Set, paths_edges); // Available edges in each step to find paths, mutable
  member_class(Set, matched_E); // Matched edges, mutable
  member_class(Set, unmatched_E); // Unmatched edges, mutable

  member_class(Set, matched_V); // All matched vertices, mutable
  member_class(Set, unmatched_V); // All matched vertices, mutable
  member_class(Set, unmatched_F); // Left unmatched vertices, mutable
  member_class(Set, matched_U); // Right matched vertices, mutable
  member_class(Set, unmatched_U); // Right unmatched vertices, mutable

  SBGMatching();
  SBGMatching(SBGraph sbg);

  Set getManyToOne(); // Find N:1 connections
  void shortPathDirection(Set endings, Direction dir); 
  void shortPathStep();
  void shortPath();

  PWMap directedOffset(PWMap dir_map);
  DSBGraph offsetGraph(PWMap dir_omap);
  void directedMinReach(PWMap dir_map);
  void minReachableStep();
  void minReachable();  

  Set getAllowedEdges(); // Calculate edges used by paths
  bool fullyMatchedU();
  void offsetVertices();
  void updatePaths();
  void updateOffset();
  MatchInfo calculate();
};

} // namespace LIB

}  // namespace SBG

#endif
