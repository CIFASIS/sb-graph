/** @file tearing_data.hpp

 @brief <b>Tearing Input and Ouput data structure</b>

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

#ifndef SBGRAPH_ALGORITHMS_TEARING_TEARING_DATA_HPP_
#define SBGRAPH_ALGORITHMS_TEARING_TEARING_DATA_HPP_

#include "sbg/directed_sbg.hpp"
#include "sbg/pw_map.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Tearing Factory implementations ---------------------------------------------
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Saves input and output data from a Tearing algorithm run.
 */
class TearingData {
public:
  TearingData(DirectedSBG dsbg, PWMap rmap, PWMap tearIOMap);

  const DirectedSBG& dsbg() const;
  const PWMap& rmap() const;
  const PWMap& tearIOMap() const;

private:
  DirectedSBG _dsbg;  ///< Original input directed SBG
  PWMap _rmap; ///< Resulting SCCs
  PWMap _tearIOMap;  ///< Paired tearing vertices
};

} // namespace LIB

} // namespace SBG

#endif // SBGRAPH_ALGORITHMS_TEARING_TEARING_DATA_HPP_
