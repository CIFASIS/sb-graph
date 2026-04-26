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

#include "algorithms/toposort/topo_sort.hpp"
#include "util/logger.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Topological Sort Algorithm Abstract Strategy Constructors -------------------
////////////////////////////////////////////////////////////////////////////////

TSStrategy::TSStrategy() {}

////////////////////////////////////////////////////////////////////////////////
// Topological Sort Algorithm Interface ----------------------------------------
////////////////////////////////////////////////////////////////////////////////

TopoSort::TopoSort(TSStratPtr strat) : strategy_(std::move(strat)) {}

PWMap TopoSort::calculate(const DSBG& dsbg) const
{
  return strategy_->calculate(dsbg);
}

PWMap TopoSort::calculate(const DSBG& dsbg, const PWMap& rmap) const
{
  return strategy_->calculate(dsbg, rmap);
}

} // namespace LIB

} // namespace SBG
