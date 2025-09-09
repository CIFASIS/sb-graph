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
// SCC Algorithm Abstract Strategy Constructors --------------------------------
////////////////////////////////////////////////////////////////////////////////

SCCStrategy::SCCStrategy(const PWMapAF& fact) : fact_(std::move(fact)) {}

////////////////////////////////////////////////////////////////////////////////
// SCC Algorithm Interface -----------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

SCC::SCC(SCCStratPtr strat) : strategy_(std::move(strat)) {}

SCCData SCC::calculate(const DSBG& dsbg)
{
  return strategy_->calculate(dsbg);
}

} // namespace LIB

} // namespace SBG
