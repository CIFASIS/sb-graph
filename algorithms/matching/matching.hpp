/** @file matching.hpp

 @brief <b>Matching SBG implementation</b>

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

#ifndef SBG_MATCH_HPP
#define SBG_MATCH_HPP

#include "sbg/sbg.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Matching --------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

enum Direction { forward, backward };

struct MatchInfo {
  member_class(Set, matched_edges);
  member_class(bool, fully_matchedU);

  MatchInfo(Set matched_edges, bool fully_matchedU);
};
std::ostream &operator<<(std::ostream &out, const MatchInfo &m_info);

struct BFSMatching {
  private:
  const PWMapAF &fact_;

  //*** SBG info, constant
  member_class(SBG, sbg);

  member_class(Set, V);
  member_class(PWMap, Vmap);

  member_class(Set, E);
  member_class(PWMap, Emap);
  member_class(PWMap, subEmap);

  //-----------------------------
  member_class(PWMap, smap); // Successors map
  member_class(PWMap, rmap); // Representatives map

  member_class(PWMap, omap); // Offset map
  member_class(MD_NAT, max_V); // Current maximum value

  member_class(Set, F); // Left vertices, constant
  member_class(Set, U); // Right vertices, constant
  member_class(PWMap, mapF); // Left map, constant
  member_class(PWMap, mapU); // Right map, constant

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

  member_class(Set, cycle_edges);

  member_class(bool, debug);

  public:
  BFSMatching(const SBG &sbg, bool debug);

  MatchInfo calculate();

  const PWMapAF &fact() const;

  private:
  void selectSucc(DSBG dsbg);

  PWMap directedOffset(const PWMap &dir_map) const;
  DSBG offsetGraph(const PWMap &dir_omap) const;
  void directedMinReach(const PWMap &dir_map);
  void minReachableStep();
  void minReachable();  

  Set edgesInPaths() const; // Calculate edges used by paths
  // Several vertices can share the same left representant "vl", through paths
  // p1, p2, ..., pk. When vl chooses its right representant "vr" through path
  // pj all the other paths should be discarded. This function discards all
  // the edges in paths p1, ..., pj-1, pj+1, ..., pk.
  Set edgesSameRepLR(const PWMap &rmapd) const; 
  bool fullyMatchedU() const;
  void offsetVertices();
  void updatePaths();
  void updateOffset();
};

} // namespace LIB

} // namespace SBG

#endif