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

#include "algorithms/tearing/tearing.hpp"
#include "util/logger.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Auxiliary structures --------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

TearingData::TearingData(DSBG dsbg, PWMap rmap, PWMap tearIOMap)
  : dsbg_(dsbg), rmap_(rmap), tearIOMap_(tearIOMap) {}

const DSBG& TearingData::dsbg() const { return dsbg_; }
const PWMap& TearingData::rmap() const { return rmap_; }
const PWMap& TearingData::tearIOMap() const { return tearIOMap_; }

////////////////////////////////////////////////////////////////////////////////
// Tearing Algorithm Abstract Strategy Constructors --------------------------------
////////////////////////////////////////////////////////////////////////////////

TearingStrategy::TearingStrategy() {}

////////////////////////////////////////////////////////////////////////////////
// Tearing Algorithm Interface -----------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

Tearing::Tearing(TearingStratPtr strat) : strategy_(std::move(strat)) {}

TearingData Tearing::calculate(const DSBG& dsbg)
{
  return strategy_->calculate(dsbg);
}

} // namespace LIB

} // namespace SBG
