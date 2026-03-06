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

#include "algorithms/scc/decreasing_edges_mrv.hpp"
#include "algorithms/scc/minreach_scc.hpp"
#include "algorithms/scc/minadj_mrv.hpp"
#include "sbg/pwmap_fact.hpp"
#include "sbg/set_fact.hpp"
#include "util/logger.hpp"
#include "util/time_profiler.hpp"

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Minimum Reachable SCC Algorithm ---------------------------------------------
////////////////////////////////////////////////////////////////////////////////

MinReachSCC::MinReachSCC() : _dsbg(), _E(SET_FACT.createSet()) {}

void MinReachSCC::swapEdgesDirection(const Set& E)
{
  PWMap mapB = _dsbg.mapB();
  PWMap mapD = _dsbg.mapD();

  PWMap temp_mapB = mapB.restrict(E);
  mapB = mapD.restrict(E);
  mapD = temp_mapB.restrict(E);

  PWMap Emap = _dsbg.Emap().restrict(E);

  _dsbg = DirectedSBG{_dsbg.V(), _dsbg.Vmap(), mapB, mapD, Emap};
}

void MinReachSCC::init(const DirectedSBG& dsbg)
{
  _dsbg = dsbg;
  _E = dsbg.E();
}

SCCData MinReachSCC::calculate(const DirectedSBG& dsbg)
{
  Util::DEBUG_LOG << "MinReachSCC dsbg: \n" << dsbg << "\n\n";

  Util::Internal::TimeProfiler profiler{"Total MinReachSCC exec time: "};
  init(dsbg);

  PWMap rmap = PWMAP_FACT.createPWMap();
  Set Ediff = SET_FACT.createSet();
  Set oldE = dsbg.E();
  Set deleted_edges = SET_FACT.createSet();
  do {
    oldE = _dsbg.E();
    rmap = sccStep();
    Ediff = oldE.difference(_dsbg.E());
    deleted_edges = std::move(deleted_edges).disjointCup(Ediff);
  } while (Ediff != SET_FACT.createSet());
  rmap.compact();

  Util::DEBUG_LOG << "MinReachSCC result: " << rmap << "\n\n";

  return SCCData{dsbg, rmap, deleted_edges};
}

////////////////////////////////////////////////////////////////////////////////
// First version ---------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

MinReachSCCV1::MinReachSCCV1() {}

PWMap MinReachSCCV1::sccStep()
{
  // Calculate MRV
  MinAdjMRV mrv;
  PWMap new_rmap = mrv.calculate(_dsbg);
  Util::DEBUG_LOG << "MinReachSCCV1 new_rmap: " << new_rmap << "\n";

  // Leave edges in the same SCC
  PWMap rmapB = new_rmap.composition(_dsbg.mapB());
  PWMap rmapD = new_rmap.composition(_dsbg.mapD());
  Set Esame = rmapB.equalImage(rmapD);
  _E = Esame;
  Util::DEBUG_LOG << "MinReachSCCV1 erased edges: "
    << _dsbg.E().difference(_E) << "\n\n";

  // Swap directions
  swapEdgesDirection(_E);

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
  PWMap new_rmap = mrv.calculate(_dsbg);
  Util::DEBUG_LOG << "MinReachSCCV2 new_rmap: " << new_rmap << "\n";

  // Leave edges in the same SCC
  PWMap rmapB = new_rmap.composition(_dsbg.mapB());
  PWMap rmapD = new_rmap.composition(_dsbg.mapD());
  Set Esame = rmapB.equalImage(rmapD);
  _E = Esame;
  Util::DEBUG_LOG << "MinReachSCCV2 erased edges: "
    << _dsbg.E().difference(_E) << "\n\n";

  // Swap directions
  swapEdgesDirection(_E);

  return new_rmap;
}

} // namespace detail

} // namespace LIB

} // namespace SBG
