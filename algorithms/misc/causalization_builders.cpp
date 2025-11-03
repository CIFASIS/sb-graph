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
#include "sbg/af_pwmap.hpp"
#include "util/logger.hpp"

namespace MISC {

SBG::LIB::DSBG buildSCCFromMatching(const SBG::LIB::MatchData &data)
{
  auto start = std::chrono::high_resolution_clock::now();

  const SBG::LIB::SBG &sbg = data.sbg();
  SBG::LIB::Set M = data.M();
  SBG::LIB::Set free_edges = sbg.E().difference(M);

  SBG::LIB::Set V = M.compact();
  SBG::LIB::PWMap auxVmap = data.sbg().subEmap().restrict(M);
  SBG::LIB::PWMap Vmap = SBG::LIB::PW_FACT.createPWMap();
  for (const SBG::LIB::Map &map : auxVmap) 
    Vmap.emplaceBack(SBG::LIB::Map(map.dom().compact()
      , map.exp()));

  SBG::LIB::PWMap mapF = sbg.map1();
  SBG::LIB::PWMap mapU = sbg.map2();

  SBG::LIB::PWMap matchedF_inv = mapF.restrict(M).inverse();
  SBG::LIB::PWMap unmatchedF = mapF.restrict(free_edges);
  SBG::LIB::PWMap mapB = matchedF_inv.composition(unmatchedF);
  mapB = mapB.compact();
  SBG::LIB::PWMap matchedU_inv = mapU.restrict(M).inverse();
  SBG::LIB::PWMap unmatchedU = mapU.restrict(free_edges);
  SBG::LIB::PWMap mapD = matchedU_inv.composition(unmatchedU);
  mapD = mapD.compact();

  SBG::LIB::PWMap Emap = sbg.Emap().restrict(free_edges);
  SBG::LIB::PWMap subEmap = sbg.subEmap().restrict(free_edges);

  SBG::LIB::DSBG res(V, Vmap, mapB, mapD, Emap, subEmap);
  auto end = std::chrono::high_resolution_clock::now();
  auto total = std::chrono::duration_cast<std::chrono::microseconds>(
    end - start 
  );
  SBG::Util::SBG_LOG << "SBG SCC builder: " << total.count() << " [μs]\n\n"; 

  return res;
}

SBG::LIB::DSBG buildSortFromSCC(const SBG::LIB::SCCData &data)
{
  auto start = std::chrono::high_resolution_clock::now();

  const SBG::LIB::DSBG &dsbg = data.dsbg();
  SBG::LIB::PWMap rmap = data.rmap();
  SBG::LIB::Set Ediff = data.Ediff();

  SBG::LIB::PWMap mapB = rmap.composition(dsbg.mapB().restrict(Ediff));
  mapB = mapB.compact();
  SBG::LIB::PWMap mapD = rmap.composition(dsbg.mapD().restrict(Ediff));
  mapD = mapD.compact();

  SBG::LIB::PWMap aux_rmap = rmap.compact();
  SBG::LIB::PWMap reps_rmap = aux_rmap.restrict(aux_rmap.fixedPoints());
  SBG::LIB::Set V = reps_rmap.dom();

  SBG::LIB::PWMap Vmap = dsbg.Vmap().restrict(V);

  SBG::LIB::PWMap Emap = dsbg.Emap().restrict(Ediff);
  SBG::LIB::PWMap subEmap = dsbg.subEmap().restrict(Ediff);

  SBG::LIB::DSBG res(V, Vmap, mapB, mapD, Emap, subEmap);
  auto end = std::chrono::high_resolution_clock::now();
  auto total = std::chrono::duration_cast<std::chrono::microseconds>(
    end - start 
  );
  SBG::Util::SBG_LOG << "SBG Topological Sort builder: " << total.count()
    << " [μs]\n\n"; 

  return res;
}

} // namespace MISC
