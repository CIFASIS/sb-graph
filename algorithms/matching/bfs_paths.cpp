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

Set BFSPaths::calculate(const DirectedSBG& dsbg, const Set& endings)
{
  Set V = dsbg.V();
  PWMap mapB = dsbg.mapB();
  PWMap auxB = mapB;
  PWMap mapD = dsbg.mapD();
  PWMap auxD = mapD;
  PWMap Emap = dsbg.Emap();

  // Successor map to unmatched vertices
  PWMap smap = PWMAP_FACT.createPWMap(endings);

  // A record of allowed edges to keep out cycle edges
  Set E = dsbg.E();
  // Ingoing edges to vertices that reach endings
  Set ingoing = mapD.preImage(endings); 
  // A record of visited set-edges
  Set visitedSE = SET_FACT.createSet();
  do {
    // Calculate successor for ith vertices
    PWMap ingoingB = auxB.restrict(ingoing);
    PWMap ingoingD = auxD.restrict(ingoing);
    PWMap ith_smap = ingoingB.minAdj(ingoingD);

    Util::DEBUG_LOG << "ith_smap: " << ith_smap << "\n";

    // Edges that lead to a successor
    Set Ei = auxD.equalImage(ith_smap.composition(auxB));
    // Visited set-edges
    Set repeatedSE = visitedSE.intersection(Emap.image(Ei)); 
    if (!repeatedSE.isEmpty()) {
      // Propose candidate successors for repetitive paths
      Set Eplus = Emap.preImage(repeatedSE);
      PWMap smap_plus = auxB.restrict(Eplus).minAdj(auxD.restrict(Eplus)); 
      Util::DEBUG_LOG << "smap_plus: " << smap_plus << "\n";
      ith_smap = ith_smap.combine(smap_plus);
      visitedSE = visitedSE.difference(repeatedSE);
    } else {
      visitedSE = visitedSE.disjointCup(Emap.image(Ei));
    }
    smap = ith_smap.combine(smap);

    // Take out other outgoing edges to avoid cycles
    E = E.difference(auxB.preImage(smap.domain()));
    auxD = auxD.restrict(E);
    auxB = auxB.restrict(E);

    // Edges that reach vertices with a successor
    ingoing = auxD.preImage(smap.domain()).intersection(E);

    Util::DEBUG_LOG << "Ei: " << Ei << "\n";
    Util::DEBUG_LOG << "repeatedSE: " << repeatedSE << "\n";
    Util::DEBUG_LOG << "visitedSE: " << visitedSE << "\n";
    Util::DEBUG_LOG << "smap: " << smap << "\n\n";
  } while (!ingoing.isEmpty());

  // Keep edges that reach vertices in U
  Set P = smap.composition(mapB).equalImage(mapD);
  PWMap rmap = smap.mapInf();
  Set reach_unmatched = rmap.preImage(endings);
  P = P.intersection(mapD.preImage(reach_unmatched));

  Util::DEBUG_LOG << "BFS Paths P: " << P << "\n\n";

  return P;
}

} // namespace detail

} // namespace LIB

} // namespace SBG
