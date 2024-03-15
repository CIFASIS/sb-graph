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
#include <set>
#include <iostream>
#include <unordered_set>

#include "include/rapidjson/document.h"
#include "include/rapidjson/filewritestream.h"
#include "include/rapidjson/prettywriter.h"
#include "sbg/sbg.hpp"
#include "util/logger.hpp"

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
struct MinReach {
  using PW = PWMap<Set>;
  using PI = PathInfo<Set>;

  member_class(DSBGraph<Set>, dsbg);
  member_class(bool, debug);

  MinReach();
  MinReach(DSBGraph<Set> dsbg, bool debug);

  PI calculate(const Set &starts, const Set &endings) const;

  private:
  PW minReach1(const Set &reach, const PW &smap, const PW &rmap) const;
  PI recursion(unsigned int n, const Set &ER, const Set &not_rv
               , const PW &smap, const PW &rmap) const;
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

  member_class(unsigned int, k); // Depth of shortPath

  member_class(bool, debug);

  SBGMatching();
  SBGMatching(SBGraph<Set> sbg, bool debug);

  MatchInfo<Set> calculate(unsigned int k);

  private:
  Set getManyToOne() const; // Find N:1 connections
  void shortPathDirection(const Set &endings, Direction dir);
  void shortPathStep();
  void shortPath();

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
  using Components = std::set<Set>;

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
  Components transformResult(PW scc);

  private:
  PW sccStep();
};

typedef SBGSCC<UnordSet> BaseSCC;
typedef SBGSCC<OrdSet> CanonSCC;

// Topological sort ------------------------------------------------------------

// Save the order in which vertices are taken out in a recursion 
template<typename Set>
struct OrderInfo {
  member_class(SetPiece, vs);
  member_class(Util::MD_NAT, first);
  member_class(Util::MD_NAT, step);
  member_class(Util::MD_NAT, last);

  OrderInfo();
  OrderInfo(
    SetPiece vs, Util::MD_NAT first, Util::MD_NAT step, Util::MD_NAT last
  );
  
  bool operator==(const OrderInfo &other) const; 
  bool operator!=(const OrderInfo &other) const; 
};
template<typename Set>
std::ostream &operator<<(std::ostream &out, const OrderInfo<Set> &oi);

typedef OrderInfo<UnordSet> BaseOI;
typedef OrderInfo<OrdSet> CanonOI;

template<typename Set>
struct VertexOrder {
  member_class(std::vector<Set>, container); 

  VertexOrder();

  void emplaceBack(const Set &s);

  bool operator==(const VertexOrder &other) const;
  bool operator!=(const VertexOrder &other) const;
};
template<typename Set>
std::ostream &operator<<(std::ostream &out, const VertexOrder<Set> &vo);

typedef VertexOrder<UnordSet> BaseVO;
typedef VertexOrder<OrdSet> CanonVO;

template<typename Set>
struct SBGTopSort {
  using PW = PWMap<Set>;

  //*** SBG info, constant
  member_class(DSBGraph<Set>, dsbg);

  //-----------------------------
  member_class(PW, mapB);
  member_class(PW, mapD);

  member_class(Set, disordered);

  // Auxiliary map where recursive paths are grouped as pertaining to the same
  // subset-edge
  member_class(PW, rec_map);

  member_class(bool, debug);

  SBGTopSort();
  SBGTopSort(DSBGraph<Set> dsbg, bool debug);

  VertexOrder<Set> calculate(); 

  private:
  void findRecursions(); // Find all possible recursions
  Set topSortStep();
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
  const Set &matching, std::set<Set> scc, const VertexOrder<Set> &order
);

} // namespace LIB

}  // namespace SBG

#endif
