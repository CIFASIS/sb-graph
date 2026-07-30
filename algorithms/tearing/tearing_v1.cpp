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
#include "algorithms/tearing/tearing_v1.hpp"
#include "algorithms/tearing/tearing_data.hpp"
#include "algorithms/scc/minadj_mrv.hpp"
#include "algorithms/scc/scc.hpp"
#include "util/logger.hpp"
#include <iostream>

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Tearing Algorithm ---------------------------------------------
////////////////////////////////////////////////////////////////////////////////

TearingV1::TearingV1() : _dsbg(), _finalDSBG(), _tearIOMap (PWMap{}) {}


void TearingV1::init(const DirectedSBG& dsbg)
{
  _dsbg = dsbg;
  _finalDSBG = dsbg;
  _finalMapB = dsbg.mapB();
  _finalMapD = dsbg.mapD();
  return;
}

Set TearingV1::getTearingSV(const PWMap rmap)  {
  return rmap.image(rmap.domain().difference(rmap.fixedPoints()));
}

TearingData TearingV1::calculate(const DirectedSBG& dsbg)
{
  Util::DEBUG_LOG << "TearingV1 dsbg: \n" << dsbg << "\n\n";

  init(dsbg);
  LIB::SCC sccAlgorithm;
  auto begin = std::chrono::high_resolution_clock::now();

  MD_NAT maxOffset = _dsbg.V().maxElem();
  SCCData result = sccAlgorithm.calculate(_dsbg);
  PWMap _rmap = result.rmap();
  Set e_notscc = result.Ediff();
  Set e_scc = _dsbg.E().difference(e_notscc);
  Set v_tear = getTearingSV(_rmap);

  while (!v_tear.isEmpty())  {
    PWMap rmap = result.rmap();
    PWMap tearIOMap = PWMap(v_tear).offsetImage(maxOffset);
    _finalDSBG.addSetVertex(tearIOMap.image());
    maxOffset = _finalDSBG.V().maxElem();
    _tearIOMap = tearIOMap.combine(_tearIOMap);
    rmap = rmap.combine(tearIOMap.inverse());
    _rmap = _rmap.combine(rmap);

    Set e_tear_scc = _dsbg.mapD().restrict(e_scc).preImage(v_tear);
    Set e_tear_notscc = _dsbg.mapB().restrict(e_notscc).preImage(v_tear);
    PWMap mapD_scc = tearIOMap.composition(_dsbg.mapD().restrict(e_tear_scc));
    PWMap mapB_notscc = tearIOMap.composition(_dsbg.mapB().restrict(e_tear_notscc));

    _finalMapB = mapB_notscc.combine(_finalMapB);
    _finalMapD = mapD_scc.combine(_finalMapD);
    _finalMapB.compact();
    _finalMapD.compact();

    _dsbg = DirectedSBG{_finalDSBG.V(), _finalDSBG.Vmap(), _finalMapB, _finalMapD, _finalDSBG.Emap()};

    result = sccAlgorithm.calculate(_dsbg);
    e_notscc = result.Ediff();
    rmap = result.rmap();
    e_scc = e_scc.difference(e_notscc);
    v_tear = getTearingSV(rmap);
  }

  _finalDSBG = _dsbg;
  _rmap.compact();
  _tearIOMap.compact();
  
  auto end = std::chrono::high_resolution_clock::now();
  auto total = std::chrono::duration_cast<std::chrono::microseconds>(
    end - begin
  );
  Util::SBG_LOG << "Total TearingV1 exec time: " << total.count() << " [μs]\n\n"; 

  Util::DEBUG_LOG << "TearingV1 result: " << _rmap << "\n\n";

  return TearingData(_finalDSBG, _rmap, _tearIOMap);
}

} // namespace LIB

} // namespace SBG
