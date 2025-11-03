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

#include "algorithms/scc/minreach_scc.hpp"
#include "algorithms/scc/scc_fact.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Minimum Reachable SCC V1 Factory --------------------------------------------
////////////////////////////////////////////////////////////////////////////////

SCC MinReachSCCV1Fact::createSCCAlgorithm() const
{
  return SCC(std::make_unique<MinReachSCCV1>());
}

std::string MinReachSCCV1Fact::prettyPrint() const
{
  return "MRV V1";
}

////////////////////////////////////////////////////////////////////////////////
// Minimum Reachable SCC V2 Factory --------------------------------------------
////////////////////////////////////////////////////////////////////////////////

SCC MinReachSCCV2Fact::createSCCAlgorithm() const
{
  return SCC(std::make_unique<MinReachSCCV2>());
}

std::string MinReachSCCV2Fact::prettyPrint() const
{
  return "MRV V2";
}

////////////////////////////////////////////////////////////////////////////////
// Factory for clients --------------------------------------------------------- 
////////////////////////////////////////////////////////////////////////////////

SCCFactory::SCCFactory() : scc_fact_(std::make_unique<MinReachSCCV2Fact>()) {}

SCCFact& SCCFactory::scc_fact()
{
  return *scc_fact_;
}

void SCCFactory::set_scc_fact(SCCFactPtr scc_fact)
{
  scc_fact_ = std::move(scc_fact);
}

} // namespace LIB

} // namespace SBG
