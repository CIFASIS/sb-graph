/** @file scc.hpp

 @brief <b>SBG SCC Algorithm implementation</b>

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

#include "sbg/sbg.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Auxiliary structures --------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Saves input and output data from a SCC algorithm run.
 */
struct SCCData {
  member_class(DSBG, dsbg); // Original input directed SBG
  member_class(PWMap, rmap); // Resulting SCCs
  member_class(Set, Ediff); // Edges connecting vertices in different SCC

  SCCData(DSBG dsbg, PWMap rmap, Set Ediff);
};

////////////////////////////////////////////////////////////////////////////////
// SCC Algorithm Abstract Delegate ---------------------------------------------
////////////////////////////////////////////////////////////////////////////////

struct SCCDelegate;

typedef std::unique_ptr<SCCDelegate> SCCDelegPtr;

struct SCCDelegate {
  protected:
  const PWMapAF &fact_;

  public:
  virtual ~SCCDelegate() = default;

  SCCDelegate(const PWMapAF &fact);

  virtual SCCData calculate(const DSBG &dsbg) = 0;
};

////////////////////////////////////////////////////////////////////////////////
// SCC Algorithm Implementation (concrete delegate) ----------------------------
////////////////////////////////////////////////////////////////////////////////

struct MinReachSCC : public SCCDelegate {
  private:
  /**
   * @brief Edges with the same MRV in both endings, in each step.
   */
  member_class(Set, E);
  /**
   * @brief Edges with different MRV in each ending, in each step.
   */
  member_class(Set, Ediff);

  member_class(PWMap, mapB);
  member_class(PWMap, mapD);
 
  public:
  MinReachSCC(const PWMapAF &fact);

  SCCData calculate(const DSBG &dsbg) override;

  private:
  void init(const DSBG &dsbg);
  PWMap sccMinReach(const DSBG &dsbg) const;
  PWMap sccStep(const DSBG &dsbg);
};

////////////////////////////////////////////////////////////////////////////////
// SCC Algorithm Implementation (delegator) ------------------------------------
////////////////////////////////////////////////////////////////////////////////

struct SCC {
  private:
  SCCDelegPtr delegate_;

  public:
  SCC(SCCDelegPtr deleg);

  SCCData calculate(const DSBG &dsbg);
};

} // namespace LIB

} // namespace SBG

#endif
