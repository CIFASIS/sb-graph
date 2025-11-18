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
#include "algorithms/tearing/tearing_impl.hpp"
#include "algorithms/scc/minadj_mrv.hpp"
#include "algorithms/scc/scc.hpp"
#include "util/logger.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Tearing Algorithm ---------------------------------------------
////////////////////////////////////////////////////////////////////////////////

TearingV1::TearingV1(SCCPtr sccAlgorithm) : dsbg_(), finalDSBG(), tearIOMap_ (PW_FACT.createPWMap()),sccAlgorithm_(std::move(sccAlgorithm)) {}


void TearingV1::init(const DSBG& dsbg)
{
  dsbg_ = dsbg;
  finalDSBG = dsbg;
  return;
}

TearingData TearingV1::calculate(const DSBG& dsbg)
{
  Util::DEBUG_LOG << "MinReachTearing dsbg: \n" << dsbg << "\n\n";

  init(dsbg);
  auto begin = std::chrono::high_resolution_clock::now();

  MD_NAT maxOffset = dsbg_.V().maxElem();
  SCCData result = sccAlgorithm_->calculate(dsbg_);
  PWMap rmap_ = result.rmap();
  PWMap finalMapB = dsbg_.mapB();
  PWMap finalMapD = dsbg_.mapD();
  Set e_notscc = result.Ediff();
  Set e_scc = dsbg_.E().difference(e_notscc);
  while (!e_notscc.isEmpty())  {
    PWMap rmap = result.rmap();
    Set v_tear = rmap.image(rmap.sharedImage());

    PWMap tearIOMap = PW_FACT.createPWMap(v_tear).offsetImage(maxOffset);
    finalDSBG = finalDSBG.addSV(tearIOMap.image());
    maxOffset = finalDSBG.V().maxElem();
    tearIOMap_ = tearIOMap.combine(tearIOMap_);
    rmap = rmap.combine(tearIOMap.firstInv());

    Set e_tear_scc = dsbg_.mapD().restrict(e_scc).preImage(v_tear);
    Set e_tear_notscc = dsbg_.mapB().restrict(e_notscc).preImage(v_tear);
    // PWMap mapB_scc = dsbg_.mapB().restrict(e_scc).composition(rmap); // Necessary?
    PWMap mapD_scc = dsbg_.mapD().restrict(e_tear_scc).composition(tearIOMap);
    PWMap mapB_notscc = dsbg_.mapB().restrict(e_tear_notscc).composition(tearIOMap);
    PWMap mapD_notscc = dsbg_.mapD().restrict(e_notscc).composition(rmap); // Necessary?
    
    finalMapB = mapB_notscc.combine(finalMapB);
    finalMapD = mapD_scc.combine(mapD_notscc).combine(finalMapD);
    dsbg_ = DSBG(finalDSBG.V().compact(), finalDSBG.Vmap().compact()
    , finalMapB.compact(), finalMapD.compact(), finalDSBG.Emap().compact(), finalDSBG.subEmap().compact());
    result = sccAlgorithm_->calculate(dsbg_);
    e_notscc = result.Ediff();
    e_scc = e_scc.difference(e_notscc);
  }

  finalDSBG = dsbg_;

  auto end = std::chrono::high_resolution_clock::now();
  auto total = std::chrono::duration_cast<std::chrono::microseconds>(
    end - begin
  );
  Util::SBG_LOG << "Total MinReachTearing exec time: " << total.count() << " [μs]\n\n"; 

  Util::DEBUG_LOG << "MinReachTearing result: " << rmap_ << "\n\n";

  return TearingData(finalDSBG, rmap_, tearIOMap_);
}

} // namespace LIB

} // namespace SBG
