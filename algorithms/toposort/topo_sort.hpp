/** @file topo_sort.hpp

 @brief <b>Topological Sort SBG implementation</b>

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

#ifndef SBG_TOPSORT_HPP
#define SBG_TOPSORT_HPP

#include "sbg/sbg.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Topological sort ------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

struct TopoSort {
  private:
  const PWMapAF &fact_;

  //*** SBG info, constant
  member_class(DSBG, dsbg);

  //-----------------------------
  member_class(bool, debug);

  public:
  TopoSort(const DSBG &dsbg, bool debug);

  PWMap calculate(); 

  const PWMapAF &fact() const;

  private:
  Exp calculateExp(const MD_NAT &from, const MD_NAT &to);
};

} // namespace LIB

} // namespace SBG

#endif
