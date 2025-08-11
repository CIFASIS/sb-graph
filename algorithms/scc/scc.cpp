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

#include "algorithms/scc/mrv.hpp"
#include "algorithms/scc/scc.hpp"
#include "util/logger.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Auxiliary structures --------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

SCCData::SCCData(DSBG dsbg, PWMap rmap, Set Ediff)
  : dsbg_(dsbg), rmap_(rmap), Ediff_(Ediff) {}

const DSBG& SCCData::dsbg() const { return dsbg_; }
const PWMap& SCCData::rmap() const { return rmap_; }
const Set& SCCData::Ediff() const { return Ediff_; }

////////////////////////////////////////////////////////////////////////////////
// SCC Algorithm Delegate Constructors -----------------------------------------
////////////////////////////////////////////////////////////////////////////////

SCCDelegate::SCCDelegate(const PWMapAF& fact) : fact_(std::move(fact)) {}

////////////////////////////////////////////////////////////////////////////////
// Minimum Reachable SCC Algorithm ---------------------------------------------
////////////////////////////////////////////////////////////////////////////////

MinReachSCC::MinReachSCC(const PWMapAF& fact)
  : SCCDelegate(fact), dsbg_(DSBG(fact)), E_(fact_.createSet()) {}

void MinReachSCC::swapEdgesDirection(const Set& E)
{
  PWMap mapB = dsbg_.mapB();
  PWMap mapD = dsbg_.mapD();

  PWMap temp_mapB = mapB.restrict(E);
  mapB = mapD.restrict(E);
  mapD = temp_mapB.restrict(E);

  PWMap Emap = dsbg_.Emap().restrict(E);
  PWMap subEmap = dsbg_.subEmap().restrict(E);

  dsbg_ = DSBG(fact_, dsbg_.V().compact(), dsbg_.Vmap().compact()
    , mapB.compact(), mapD.compact(), Emap.compact(), subEmap.compact());

  return;
}

PWMap MinReachSCC::sccStep()
{
  // Calculate MRV
  MinAdjMRV mrv(fact_);
  PWMap new_rmap = mrv.calculate(dsbg_);
  Util::DEBUG_LOG << "MinReachSCC new_rmap: " << new_rmap << "\n";

  // Leave edges in the same SCC
  PWMap rmapB = new_rmap.composition(dsbg_.mapB());
  PWMap rmapD = new_rmap.composition(dsbg_.mapD());
  Set Esame = rmapB.equalImage(rmapD);
  E_ = Esame;
  Util::DEBUG_LOG << "MinReachSCC erased edges: "
    << dsbg_.E().difference(E_) << "\n\n";

  // Swap directions
  swapEdgesDirection(E_);

  return new_rmap;
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
  PWMap rmap = fact_.createPWMap();
  Set Ediff = fact_.createSet();
  Set oldE = dsbg.E();
  do {
    oldE = dsbg_.E();
    rmap = sccStep();
    Ediff = oldE.difference(dsbg_.E());
  } while (Ediff != fact_.createSet());
  rmap = rmap.compact();
  auto end = std::chrono::high_resolution_clock::now();

  auto total = std::chrono::duration_cast<std::chrono::microseconds>(
    end - begin
  );
  Util::SBG_LOG << "Total MinReachSCC exec time: " << total.count() << " [μs]\n\n"; 

  Util::DEBUG_LOG << "MinReachSCC result: " << rmap << "\n\n";

  return SCCData(dsbg, rmap, Ediff);
}

////////////////////////////////////////////////////////////////////////////////
// SCC Algorithm Implementation ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

SCC::SCC(SCCDelegPtr deleg) : delegate_(std::move(deleg)) {}

SCCData SCC::calculate(const DSBG& dsbg)
{
  return delegate_->calculate(dsbg);
}

} // namespace LIB

} // namespace SBG
