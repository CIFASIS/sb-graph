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

#include "algorithms/matching/bfs_paths.hpp"
#include "sbg/pwmap_fact.hpp"
#include "sbg/set_fact.hpp"
#include "util/logger.hpp"

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Path Finder BFS Implementation ----------------------------------------------
////////////////////////////////////////////////////////////////////////////////

BFSPaths::BFSPaths() {}

PWMap BFSPaths::calculate(const DirectedSBG& dsbg, const Set& endings)
{
  Set V = dsbg.V();
  PWMap mapB = dsbg.mapB();
  PWMap mapD = dsbg.mapD();
  PWMap Emap = dsbg.Emap();

  // Successor map to unmatched vertices
  PWMap result = PWMAP_FACT.createPWMap(endings);

  // A record of allowed edges to keep out cycle edges
  Set allowed_edges = dsbg.E();
  // Ingoing edges to vertices that reach endings
  Set ingoing = mapD.preImage(endings); 
  // A record of visited set-edges
  Set visitedE = SET_FACT.createSet();
  do {
    // Calculate successor for ith vertices
    PWMap ingoingB = mapB.restrict(ingoing);
    PWMap ingoingD = mapD.restrict(ingoing);
    PWMap ith_smap = ingoingB.minAdj(ingoingD);

    Util::DEBUG_LOG << "ith_smap: " << ith_smap << "\n";

    // Edges that lead to a successor
    Set Eith = mapD.equalImage(ith_smap.composition(mapB));
    // Visited set-edges
    Set Erec = visitedE.intersection(Emap.image(Eith)); 
    // Handle recursion
    if (!Erec.isEmpty()) {
      Set Eplus = Emap.preImage(Erec);
      PWMap rec_smap = mapB.restrict(Eplus).minAdj(mapD.restrict(Eplus)); 
      Util::DEBUG_LOG << "rec_smap: " << rec_smap << "\n";
      ith_smap = ith_smap.combine(rec_smap);
    }
    result = ith_smap.combine(result);

    // Take out other outgoing edges to avoid cycles
    allowed_edges = allowed_edges.difference(mapB.preImage(result.domain()));
    mapD = mapD.restrict(allowed_edges);
    mapB = mapB.restrict(allowed_edges);

    // Edges that reach vertices with a successor
    ingoing = mapD.preImage(result.domain()).intersection(allowed_edges);

    visitedE = visitedE.cup(Emap.image(Eith));

    Util::DEBUG_LOG << "Eith: " << Eith << "\n";
    Util::DEBUG_LOG << "Erec: " << Erec << "\n";
    Util::DEBUG_LOG << "visitedE: " << visitedE << "\n";
    Util::DEBUG_LOG << "result: " << result << "\n\n";
  } while (!ingoing.isEmpty());

  return result;
}

} // namespace detail

} // namespace LIB

} // namespace SBG
