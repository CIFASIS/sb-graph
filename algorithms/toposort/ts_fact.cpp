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

#include "algorithms/toposort/ts_fact.hpp"
#include "algorithms/toposort/min_vertex_ts.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Minimum Vertex Topological Sort AF ------------------------------------------
////////////////////////////////////////////////////////////////////////////////

TopoSort MinVertexTSFact::createTSAlgorithm() const
{
  return TopoSort(std::make_unique<MinVertexTopoSort>());
}

////////////////////////////////////////////////////////////////////////////////
// Factory for clients --------------------------------------------------------- 
////////////////////////////////////////////////////////////////////////////////

TSFactory::TSFactory() : ts_fact_(std::make_unique<MinVertexTSFact>()) {}

TSFact& TSFactory::ts_fact()
{
  return *ts_fact_;
}

void TSFactory::set_ts_fact(TSFactPtr ts_fact)
{
  ts_fact_ = std::move(ts_fact);
}

} // namespace LIB

} // namespace SBG
