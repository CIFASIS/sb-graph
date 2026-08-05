/** @file scc_data.hpp

 @brief <b>SCC Input and Ouput data structure</b>

 <hr>

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

#ifndef SBGRAPH_ALGORITHMS_SCC_SCC_DATA_HPP_
#define SBGRAPH_ALGORITHMS_SCC_SCC_DATA_HPP_

#include "sbg/directed_sbg.hpp"
#include "sbg/pw_map.hpp"
#include "sbg/set.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// SCC Factory implementations -------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Saves input and output data from a SCC algorithm run.
 */
class SCCData {
public:
  SCCData(DirectedSBG dsbg, PWMap rmap, Set Ediff);

  const DirectedSBG& dsbg() const;
  const PWMap& rmap() const;
  const Set& Ediff() const;

private:
  DirectedSBG _dsbg;  ///< Original input directed SBG
  PWMap _rmap; ///< Resulting SCCs
  Set _Ediff;  ///< Edges connecting vertices in different SCC
};

} // namespace LIB

} // namespace SBG

#endif // SBGRAPH_ALGORITHMS_SCC_SCC_DATA_HPP_
