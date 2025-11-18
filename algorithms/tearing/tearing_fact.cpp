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

#include "algorithms/tearing/tearing_impl.hpp"
#include "algorithms/tearing/tearing_fact.hpp"
#include "algorithms/scc/scc_fact.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Tearing V1 Factory --------------------------------------------
////////////////////////////////////////////////////////////////////////////////

Tearing TearingV1Fact::createTearingAlgorithm() const
{ 
  SCCPtr sccPtr = std::make_unique<SCC>(SCC_FACT.createSCCAlgorithm()); 
  return Tearing(std::make_unique<TearingV1>(std::move(sccPtr)));
}

std::string TearingV1Fact::prettyPrint() const
{
  return "TEARING V1";
}

////////////////////////////////////////////////////////////////////////////////
// Factory for clients --------------------------------------------------------- 
////////////////////////////////////////////////////////////////////////////////

TearingFactory::TearingFactory() : tearing_fact_(std::make_unique<TearingV1Fact>()) {}

TearingFact& TearingFactory::tearing_fact()
{
  return *tearing_fact_;
}

void TearingFactory::set_tearing_fact(TearingFactPtr tearing_fact)
{
  tearing_fact_ = std::move(tearing_fact);
}

} // namespace LIB

} // namespace SBG
