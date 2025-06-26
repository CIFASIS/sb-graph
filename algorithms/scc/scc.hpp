/** @file scc.hpp

 @brief <b>SCC SBG implementation</b>

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
// SCC -------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

struct SCC {
  private:
  const PWMapAF &fact_;

  //*** SBG info, constant
  member_class(DSBG, dsbg);

  member_class(Set, V);
  member_class(PWMap, Vmap);

  member_class(PWMap, Emap);
  member_class(PWMap, subEmap);

  //-----------------------------
  member_class(Set, E); // Edges in the same SCC in each step
  member_class(Set, Ediff); // Edges between different SCC in each step

  member_class(PWMap, mapB);
  member_class(PWMap, mapD);
 
  member_class(PWMap, rmap);

  member_class(bool, debug);

  public:
  SCC(const DSBG &dsbg, bool debug);

  PWMap calculate();

  const PWMapAF &fact() const;

  private:
  PWMap sccMinReach(const DSBG &dg) const;
  PWMap sccStep();
};

} // namespace LIB

} // namespace SBG

#endif
