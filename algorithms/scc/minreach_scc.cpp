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

#include "algorithms/scc/decreasing_edges_mrv.hpp"
#include "algorithms/scc/minreach_scc.hpp"
#include "algorithms/scc/minadj_mrv.hpp"
#include "util/logger.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Minimum Reachable SCC Algorithm ---------------------------------------------
////////////////////////////////////////////////////////////////////////////////

MinReachSCC::MinReachSCC() : dsbg_(), E_(SET_FACT.createSet()) {}

void MinReachSCC::swapEdgesDirection(const Set& E)
{
  PWMap mapB = dsbg_.mapB();
  PWMap mapD = dsbg_.mapD();

  PWMap temp_mapB = mapB.restrict(E);
  mapB = mapD.restrict(E);
  mapD = temp_mapB.restrict(E);

  PWMap Emap = dsbg_.Emap().restrict(E);
  PWMap subEmap = dsbg_.subEmap().restrict(E);

  dsbg_ = DSBG(dsbg_.V().compact(), dsbg_.Vmap().compact()
    , mapB.compact(), mapD.compact(), Emap.compact(), subEmap.compact());

  return;
}

void MinReachSCC::init(const DSBG& dsbg)
{
  dsbg_ = dsbg;
  E_ = dsbg.E();

  return;
}

SCCData MinReachSCC::calculate(const DSBG& dsbg)
{
  Util::DEBUG_LOG << "MinReachSCC dsbg: \n" << dsbg << "\n\n";

  init(dsbg);

  auto begin = std::chrono::high_resolution_clock::now();
  PWMap rmap = PW_FACT.createPWMap();
  Set Ediff = SET_FACT.createSet();
  Set oldE = dsbg.E();
  Set deleted_edges = SET_FACT.createSet();
  do {
    oldE = dsbg_.E();
    rmap = sccStep();
    Ediff = oldE.difference(dsbg_.E());
    deleted_edges = deleted_edges.disjointCup(Ediff);
  } while (Ediff != SET_FACT.createSet());
  rmap = rmap.compact();
  auto end = std::chrono::high_resolution_clock::now();

  auto total = std::chrono::duration_cast<std::chrono::microseconds>(
    end - begin
  );
  Util::SBG_LOG << "Total MinReachSCC exec time: " << total.count() << " [μs]\n\n"; 

  Util::DEBUG_LOG << "MinReachSCC result: " << rmap << "\n\n";

  return SCCData(dsbg, rmap, deleted_edges);
}

////////////////////////////////////////////////////////////////////////////////
// First version ---------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

MinReachSCCV1::MinReachSCCV1() {}

PWMap MinReachSCCV1::sccStep()
{
  // Calculate MRV
  MinAdjMRV mrv;
  PWMap new_rmap = mrv.calculate(dsbg_);
  Util::DEBUG_LOG << "MinReachSCC new_rmap: " << new_rmap << "\n";

  // Leave edges in the same SCC
  PWMap rmapB = new_rmap.composition(dsbg_.mapB());
  PWMap rmapD = new_rmap.composition(dsbg_.mapD());
  Set Esame = rmapB.equalImage(rmapD);
  E_ = Esame;
  Util::DEBUG_LOG << "MinReachSCCV1 erased edges: "
    << dsbg_.E().difference(E_) << "\n\n";

  // Swap directions
  swapEdgesDirection(E_);

  return new_rmap;
}

////////////////////////////////////////////////////////////////////////////////
// Second version --------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

MinReachSCCV2::MinReachSCCV2() {}

PWMap MinReachSCCV2::sccStep()
{
  // Calculate MRV
  LtEdgesMRV mrv;
  PWMap new_rmap = mrv.calculate(dsbg_);
  Util::DEBUG_LOG << "MinReachSCC new_rmap: " << new_rmap << "\n";

  // Leave edges in the same SCC
  PWMap rmapB = new_rmap.composition(dsbg_.mapB());
  PWMap rmapD = new_rmap.composition(dsbg_.mapD());
  Set Esame = rmapB.equalImage(rmapD);
  E_ = Esame;
  Util::DEBUG_LOG << "MinReachSCCV1 erased edges: "
    << dsbg_.E().difference(E_) << "\n\n";

  // Swap directions
  swapEdgesDirection(E_);

  return new_rmap;
}

} // namespace LIB

} // namespace SBG
