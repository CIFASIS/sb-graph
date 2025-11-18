/** @file scc.hpp

 @brief <b>SBG Tearing Algorithm Abstract Interface</b>

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

#ifndef SBG_TEARING_HPP
#define SBG_TEARING_HPP

#include "sbg/sbg.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Auxiliary classures --------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Saves input and output data from a Tearing algorithm run.
 */
struct TearingData {
  public:
  TearingData(DSBG dsbg, PWMap rmap, PWMap tearIOMap);

  const DSBG& dsbg() const;
  const PWMap& rmap() const;
  const PWMap& tearIOMap() const;

  private:
  DSBG dsbg_;  ///< Original input directed SBG
  PWMap rmap_; ///< Resulting SCCs
  PWMap tearIOMap_;  ///< Paired tearing vertices
};

////////////////////////////////////////////////////////////////////////////////
// Tearing Algorithm Abstract Strategy ---------------------------------------------
////////////////////////////////////////////////////////////////////////////////

class TearingStrategy;

typedef std::unique_ptr<TearingStrategy> TearingStratPtr;

class TearingStrategy {
  public:
  virtual ~TearingStrategy() = default;

  TearingStrategy();

  virtual TearingData calculate(const DSBG& dsbg) = 0;
};

////////////////////////////////////////////////////////////////////////////////
// Tearing Algorithm Interface (context) -------------------------------------------
////////////////////////////////////////////////////////////////////////////////

class Tearing {
  public:
  Tearing(TearingStratPtr strat);

  TearingData calculate(const DSBG& dsbg);

  private:
  TearingStratPtr strategy_;
};

} // namespace LIB

} // namespace SBG

#endif
