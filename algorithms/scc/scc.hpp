/** @file scc.hpp

 @brief <b>SBG SCC Algorithm Abstract Interface</b>

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

#ifndef SBG_SCC_HPP
#define SBG_SCC_HPP

#include "sbg/directed_sbg.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Auxiliary classures --------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Saves input and output data from a SCC algorithm run.
 */
struct SCCData {
  public:
  SCCData(DSBG dsbg, PWMap rmap, Set Ediff);

  const DSBG& dsbg() const;
  const PWMap& rmap() const;
  const Set& Ediff() const;

  private:
  DSBG dsbg_;  ///< Original input directed SBG
  PWMap rmap_; ///< Resulting SCCs
  Set Ediff_;  ///< Edges connecting vertices in different SCC
};

////////////////////////////////////////////////////////////////////////////////
// SCC Algorithm Abstract Strategy ---------------------------------------------
////////////////////////////////////////////////////////////////////////////////

class SCCStrategy;

typedef std::unique_ptr<SCCStrategy> SCCStratPtr;

class SCCStrategy {
  public:
  virtual ~SCCStrategy() = default;

  SCCStrategy();

  virtual SCCData calculate(const DSBG& dsbg) = 0;
};

////////////////////////////////////////////////////////////////////////////////
// SCC Algorithm Interface (context) -------------------------------------------
////////////////////////////////////////////////////////////////////////////////

class SCC {
  public:
  SCC(SCCStratPtr strat);

  SCCData calculate(const DSBG& dsbg);

  private:
  SCCStratPtr strategy_;
};

} // namespace LIB

} // namespace SBG

#endif
