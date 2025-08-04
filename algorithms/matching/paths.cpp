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

#include "algorithms/matching/paths.hpp"
#include "util/logger.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Path Finder -----------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

template<class PathsImpl>
Paths<PathsImpl>::Paths(const PWMapAF &fact) : fact_(fact) {}

template<class PathsImpl>
PWMap Paths<PathsImpl>::calculate(const DSBG &dsbg, const Set &endings)
{
  return static_cast<PathsImpl*>(this)->impl(dsbg, endings);
}

////////////////////////////////////////////////////////////////////////////////
// Path Finder BFS Implementation ----------------------------------------------
////////////////////////////////////////////////////////////////////////////////

BFSPaths::BFSPaths(const PWMapAF &fact) : Paths<BFSPaths>(fact) {}

PWMap BFSPaths::impl(const DSBG &dsbg, const Set &endings)
{
  Set dsbgV = dsbg.V();
  PWMap dsbgB = dsbg.mapB();
  PWMap dsbgD = dsbg.mapD();
  PWMap subEmap = dsbg.subEmap();

  // Unmatched vertices in forward direction
  PWMap res = fact_.createPWMap(endings);

  // A record of allowed edges to keep out cycle edges
  Set allowed_edges = dsbg.E();
  // Ingoing edges to vertices that reach unmatched_D
  Set ingoing = dsbgD.preImage(endings); 
  // A record of visited set-edges
  Set visitedE = fact_.createSet();
  do {
    // Calculate successor for ith vertices
    PWMap ingB = dsbgB.restrict(ingoing), ingD = dsbgD.restrict(ingoing);
    PWMap ith_smap = ingB.minAdjMap(ingD);

    Util::DEBUG_LOG << "ith_smap: " << ith_smap << "\n";

    // Edges that lead to a successor
    Set Eith = dsbgD.equalImage(ith_smap.composition(dsbgB));
    // Visited set-edges
    Set Erec = visitedE.intersection(subEmap.image(Eith)); 
    // Handle recursion
    if (!Erec.isEmpty()) {
      Set Eplus = subEmap.preImage(Erec);
      PWMap rec_smap = dsbgB.restrict(Eplus).minAdjMap(dsbgD.restrict(Eplus)); 
      Util::DEBUG_LOG << "rec_smap: " << rec_smap << "\n";
      ith_smap = ith_smap.combine(rec_smap);
    }
    res = ith_smap.combine(res);
 
    // Take out other outgoing edges to avoid cycles
    allowed_edges = allowed_edges.difference(dsbgB.preImage(res.dom()));
    dsbgD = dsbgD.restrict(allowed_edges);
    dsbgB = dsbgB.restrict(allowed_edges);

    // Edges that reach vertices with a successor
    ingoing = dsbgD.preImage(res.dom()).intersection(allowed_edges);

    visitedE = visitedE.cup(subEmap.image(Eith));

    Util::DEBUG_LOG << "Eith: " << Eith << "\n";
    Util::DEBUG_LOG << "Erec: " << Erec << "\n";
    Util::DEBUG_LOG << "visitedE: " << visitedE << "\n";
    Util::DEBUG_LOG << "res: " << res << "\n\n";
  } while (!ingoing.isEmpty());

  return res;
}

template class Paths<BFSPaths>;

} // namespace LIB

} // namespace SBG
