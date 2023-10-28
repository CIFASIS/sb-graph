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

#include "sbg/sbg.hpp"

namespace SBG {

namespace LIB {

// Connected components --------------------------------------------------------

template<typename Set>
PWMap<Set> connectedComponents(SBGraph<Set> g);

// Minimum reachable -----------------------------------------------------------

template<typename Set>
struct PathInfo {
  member_class(PWMap<Set>, succs);
  member_class(PWMap<Set>, reps);

  PathInfo();
  PathInfo(PWMap<Set> succs, PWMap<Set> reps);
};

template<typename Set>
PWMap<Set> minReach1(Set V, Set E, PWMap<Set> mapB, PWMap<Set> mapD, PWMap<Set> smap, PWMap<Set> rmap);


template<typename Set>
struct MinReach {
  member_class(DSBGraph<Set>, dsbg);

  MinReach();
  MinReach(DSBGraph<Set> dsbg);

  PathInfo<Set> recursion(unsigned int n, Set ER, Set not_rv, PWMap<Set> smap, PWMap<Set> rmap);
  PathInfo<Set> calculate(Set unmatched_V);
};

typedef MinReach<UnordSet> BaseMR;
typedef MinReach<OrdSet> CanonMR;

// Matching --------------------------------------------------------------------

enum Direction { forward, backward };

template<typename Set>
struct MatchInfo {
  member_class(Set, matched_edges);
  member_class(bool, fully_matchedU);

  MatchInfo();
  MatchInfo(Set matched_edges, bool fully_matchedU);
};
template<typename Set>
std::ostream &operator<<(std::ostream &out, const MatchInfo<Set> &m_info);

template<typename Set>
struct SBGMatching {
  //*** SBG info, constant
  member_class(SBGraph<Set>, sbg);

  member_class(Set, V);
  member_class(PWMap<Set>, Vmap);

  member_class(Set, E);
  member_class(PWMap<Set>, Emap);
  //-----------------------------

  member_class(PWMap<Set>, smap); // Successors map
  member_class(PWMap<Set>, rmap); // Representatives map

  member_class(PWMap<Set>, omap); // Offset map
  member_class(Util::MD_NAT, max_V); // Current maximum value

  member_class(Set, F); // Left vertices, constant
  member_class(Set, U); // Right vertices, constant
  member_class(PWMap<Set>, mapF); // Left map, constant
  member_class(PWMap<Set>, mapU); // Forward map, constant

  member_class(PWMap<Set>, mapB); // Backward map, mutable
  member_class(PWMap<Set>, mapD); // Forward map, mutable

  member_class(Set, paths_edges); // Available edges in each step to find paths, mutable
  member_class(Set, matched_E); // Matched edges, mutable
  member_class(Set, unmatched_E); // Unmatched edges, mutable

  member_class(Set, matched_V); // All matched vertices, mutable
  member_class(Set, unmatched_V); // All matched vertices, mutable
  member_class(Set, unmatched_F); // Left unmatched vertices, mutable
  member_class(Set, matched_U); // Right matched vertices, mutable
  member_class(Set, unmatched_U); // Right unmatched vertices, mutable

  SBGMatching();
  SBGMatching(SBGraph<Set> sbg);

  Set getManyToOne(); // Find N:1 connections
  void shortPathDirection(Set endings, Direction dir); 
  void shortPathStep();
  void shortPath();

  PWMap<Set> directedOffset(PWMap<Set> dir_map);
  DSBGraph<Set> offsetGraph(PWMap<Set> dir_omap);
  void directedMinReach(PWMap<Set> dir_map);
  void minReachableStep();
  void minReachable();  

  Set getAllowedEdges(); // Calculate edges used by paths
  bool fullyMatchedU();
  void offsetVertices();
  void updatePaths();
  void updateOffset();
  MatchInfo<Set> calculate();
};

typedef SBGMatching<UnordSet> BaseMatch;
typedef SBGMatching<OrdSet> CanonMatch;

} // namespace LIB

}  // namespace SBG

#endif
