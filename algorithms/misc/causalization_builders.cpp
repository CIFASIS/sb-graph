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

#include <chrono>

#include "algorithms/misc/causalization_builders.hpp"
#include "util/logger.hpp"

namespace MISC {

using namespace SBG::LIB;

DSBG buildSCCFromMatching(const BFSMatching &match)
{
  const PWMapAF &fact = match.fact();

  auto start = std::chrono::high_resolution_clock::now();
  Set matched_edges = match.matched_E(), unmatched_edges = match.unmatched_E();

  Set V = matched_edges.compact();
  PWMap auxVmap = match.sbg().subEmap().restrict(matched_edges);
  PWMap Vmap = fact.createPWMap();
  for (const Map &map : auxVmap) 
    Vmap.emplaceBack(fact.createMap(map.dom().compact(), map.exp()));

  PWMap matchedF_inv = match.mapF().restrict(matched_edges).inverse();
  PWMap unmatchedF = match.mapF().restrict(unmatched_edges);
  PWMap mapB = matchedF_inv.composition(unmatchedF);
  mapB = mapB.compact();
  PWMap matchedU_inv = match.mapU().restrict(matched_edges).inverse();
  PWMap unmatchedU = match.mapU().restrict(unmatched_edges);
  PWMap mapD = matchedU_inv.composition(unmatchedU);
  mapD = mapD.compact();

  PWMap Emap = match.Emap().restrict(unmatched_edges);
  PWMap subEmap = match.subEmap().restrict(unmatched_edges);

  DSBG res(fact, V, Vmap, mapB, mapD, Emap, subEmap);
  auto end = std::chrono::high_resolution_clock::now();
  auto total = std::chrono::duration_cast<std::chrono::microseconds>(
    end - start 
  );
  SBG::Util::SBG_LOG << "SBG SCC builder: " << total.count() << " [μs]\n\n"; 

  return res;
}

DSBG buildSortFromSCC(const SCC &scc, const PWMap &rmap)
{
  auto start = std::chrono::high_resolution_clock::now();
  DSBG dsbg = scc.dsbg();
  Set Ediff = dsbg.E().difference(scc.E());
  PWMap mapB = rmap.composition(dsbg.mapB().restrict(Ediff));
  mapB = mapB.compact();
  PWMap mapD = rmap.composition(dsbg.mapD().restrict(Ediff));
  mapD = mapD.compact();

  PWMap aux_rmap = rmap.compact();
  PWMap reps_rmap = aux_rmap.restrict(aux_rmap.fixedPoints());
  Set V = reps_rmap.dom();

  PWMap Vmap = dsbg.Vmap().restrict(V);

  PWMap Emap = dsbg.Emap().restrict(Ediff);
  PWMap subEmap = dsbg.subEmap().restrict(Ediff);

  DSBG res(dsbg.fact(), V, Vmap, mapB, mapD, Emap, subEmap);
  auto end = std::chrono::high_resolution_clock::now();
  auto total = std::chrono::duration_cast<std::chrono::microseconds>(
    end - start 
  );
  SBG::Util::SBG_LOG << "SBG Topological Sort builder: " << total.count()
    << " [μs]\n\n"; 

  return res;
}

} // namespace MISC
