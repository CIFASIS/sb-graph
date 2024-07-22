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

#include <chrono>
#include <list>
#include <map>
#include <set>
#include <iostream>
#include <unordered_set>

#include "rapidjson/document.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"
#include "sbg/sbg.hpp"
#include "util/logger.hpp"

namespace SBG {

namespace LIB {

// Connected components --------------------------------------------------------

template<typename Set>
PWMap<Set> connectedComponents(SBGraph<Set> g);

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
  using Map = SBGMap<Set>;
  using PW = PWMap<Set>;

  //*** SBG info, constant
  member_class(SBGraph<Set>, sbg);

  member_class(Set, V);
  member_class(PW, Vmap);

  member_class(Set, E);
  member_class(PW, Emap);

  //-----------------------------
  member_class(PW, smap); // Successors map
  member_class(PW, rmap); // Representatives map

  member_class(PW, omap); // Offset map
  member_class(Util::MD_NAT, max_V); // Current maximum value

  member_class(Set, F); // Left vertices, constant
  member_class(Set, U); // Right vertices, constant
  member_class(PW, mapF); // Left map, constant
  member_class(PW, mapU); // Right map, constant

  member_class(PW, mapB); // Backward map, mutable
  member_class(PW, mapD); // Forward map, mutable

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

  SBGMatching();
  SBGMatching(SBGraph<Set> sbg, bool debug);

  MatchInfo<Set> calculate();

  private:
  void selectSucc(DSBGraph<Set> dsbg);

  PW directedOffset(const PW &dir_map) const;
  DSBGraph<Set> offsetGraph(const PW &dir_omap) const;
  void directedMinReach(const PW &dir_map);
  void minReachableStep();
  void minReachable();  

  Set edgesInPaths() const; // Calculate edges used by paths
  // Several vertices can share the same left representant "vl", through paths
  // p1, p2, ..., pk. When vl chooses its right representant "vr" through path
  // pj all the other paths should be discarded. This function discards all
  // the edges in paths p1, ..., pj-1, pj+1, ..., pk.
  Set edgesSameRepLR(const PW &rmapd) const; 
  bool fullyMatchedU() const;
  void offsetVertices();
  void updatePaths();
  void updateOffset();
};

typedef SBGMatching<UnordSet> BaseMatch;
typedef SBGMatching<OrdSet> CanonMatch;

// SCC -------------------------------------------------------------------------

template<typename Set>
struct SBGSCC {
  using Map = SBGMap<Set>;
  using PW = PWMap<Set>;

  //*** SBG info, constant
  member_class(DSBGraph<Set>, dsbg);

  member_class(Set, V);
  member_class(PW, Vmap);

  member_class(PW, Emap);

  //-----------------------------
  member_class(Set, E); // Edges in the same SCC in each step
  member_class(Set, Ediff); // Edges between different SCC in each step

  member_class(PW, mapB);
  member_class(PW, mapD);
 
  member_class(PW, rmap);

  member_class(bool, debug);

  SBGSCC();
  SBGSCC(DSBGraph<Set> dsbg, bool debug);

  PW calculate();

  private:
  PW sccMinReach(DSBGraph<Set> dg) const;
  PW sccStep();
};

typedef SBGSCC<UnordSet> BaseSCC;
typedef SBGSCC<OrdSet> CanonSCC;

// Topological sort ------------------------------------------------------------

template<typename Set>
struct SBGTopSort {
  using Map = SBGMap<Set>;
  using PW = PWMap<Set>;

  //*** SBG info, constant
  member_class(DSBGraph<Set>, dsbg);

  //-----------------------------
  member_class(bool, debug);

  SBGTopSort();
  SBGTopSort(DSBGraph<Set> dsbg, bool debug);

  PW calculate(); 

  private:
  Exp calculateExp(Util::MD_NAT from, Util::MD_NAT to);
};

typedef SBGTopSort<UnordSet> BaseTopSort;
typedef SBGTopSort<OrdSet> CanonTopSort;

// Additional operations ------------------------------------------------------

template<typename Set>
DSBGraph<Set> buildSCCFromMatching(const SBGMatching<Set> &match);

template<typename Set>
DSBGraph<Set> buildSortFromSCC(const SBGSCC<Set> &scc, const PWMap<Set> &rmap);

template<typename Set>
void buildJson(
  const Set &matching, const PWMap<Set> &scc, const PWMap<Set> &order
);

} // namespace LIB

}  // namespace SBG

#endif
