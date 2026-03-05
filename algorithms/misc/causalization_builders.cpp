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

#include "algorithms/misc/causalization_builders.hpp"
#include "sbg/bipartite_sbg.hpp"
#include "sbg/set.hpp"
#include "sbg/pw_map.hpp"
#include "sbg/pwmap_fact.hpp"
#include "sbg/map.hpp"
#include "util/time_profiler.hpp"

namespace misc {

SBG::LIB::DirectedSBG buildSCCFromMatching(const SBG::LIB::MatchData& data)
{
  SBG::Util::Internal::TimeProfiler profiler{"SBG SCC builder: "};

  const SBG::LIB::BipartiteSBG& bsbg = data.bsbg();
  SBG::LIB::Set M = data.M();
  SBG::LIB::Set free_edges = bsbg.E().difference(M);

  M.compact();
  SBG::LIB::Set V = M;
  SBG::LIB::PWMap auxVmap = data.bsbg().Emap().restrict(M);
  SBG::LIB::PWMap Vmap = SBG::LIB::PWMAP_FACT.createPWMap();
  for (const SBG::LIB::Map& m : auxVmap) { 
    SBG::LIB::Set domain = m.domain();
    domain.compact();
    Vmap.pushBack(SBG::LIB::Map(domain, m.law()));
  }

  SBG::LIB::PWMap map1 = bsbg.map1();
  SBG::LIB::PWMap map2 = bsbg.map2();

  SBG::LIB::Set X = bsbg.X();
  SBG::LIB::PWMap map1_toX = map1.restrict(map1.preImage(X));
  SBG::LIB::PWMap map2_toX = map2.restrict(map2.preImage(X));
  SBG::LIB::PWMap mapF = map1_toX.concatenation(map2_toX);

  SBG::LIB::Set Y = bsbg.Y();
  SBG::LIB::PWMap map1_toY = map1.restrict(map1.preImage(Y));
  SBG::LIB::PWMap map2_toY = map2.restrict(map2.preImage(Y));
  SBG::LIB::PWMap mapU = map1_toY.concatenation(map2_toY);

  SBG::LIB::PWMap matchedF_inv = mapF.restrict(M).inverse();
  SBG::LIB::PWMap unmatchedF = mapF.restrict(free_edges);
  SBG::LIB::PWMap mapB = matchedF_inv.composition(unmatchedF);
  mapB.compact();
  SBG::LIB::PWMap matchedU_inv = mapU.restrict(M).inverse();
  SBG::LIB::PWMap unmatchedU = mapU.restrict(free_edges);
  SBG::LIB::PWMap mapD = matchedU_inv.composition(unmatchedU);
  mapD.compact();

  SBG::LIB::PWMap Emap = bsbg.Emap().restrict(free_edges);

  return SBG::LIB::DirectedSBG{V, Vmap, mapB, mapD, Emap};
}

SBG::LIB::DirectedSBG buildSortFromSCC(const SBG::LIB::SCCData& data)
{
  SBG::Util::Internal::TimeProfiler profiler{"SBG Topological Sort builder: "}; 

  const SBG::LIB::DirectedSBG& dsbg = data.dsbg();
  SBG::LIB::PWMap rmap = data.rmap();
  SBG::LIB::Set Ediff = data.Ediff();

  SBG::LIB::PWMap mapB = rmap.composition(dsbg.mapB().restrict(Ediff));
  mapB.compact();
  SBG::LIB::PWMap mapD = rmap.composition(dsbg.mapD().restrict(Ediff));
  mapD.compact();

  rmap.compact();
  SBG::LIB::PWMap aux_rmap = rmap;
  SBG::LIB::PWMap reps_rmap = aux_rmap.restrict(aux_rmap.fixedPoints());
  SBG::LIB::Set V = reps_rmap.domain();

  SBG::LIB::PWMap Vmap = dsbg.Vmap().restrict(V);

  SBG::LIB::PWMap Emap = dsbg.Emap().restrict(Ediff);

  return SBG::LIB::DirectedSBG {V, Vmap, mapB, mapD, Emap};
}

} // namespace misc
