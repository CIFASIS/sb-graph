/** @file cut_vertex.hpp

 @brief <b>Cut Vertex Set SBG implementation</b>

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

#ifndef SBG_CUTVERTEX_HPP
#define SBG_CUTVERTEX_HPP

#include "sbg/sbg.hpp"

namespace SBG {

namespace LIB {

///////////////////////////////////////////////////////////////////////////////
// Cut-set algorithm ----------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

/**
* @brief Aims to calculate a minimum cut-set of vertices, that is, a set of
* vertices such that if these vertices are taken out, the resulting graph has no
* SCC left. Since this is a NP-hard problem, heuristics are used, and thus is
* not guaranteed that the set is actually minimum.
*/

struct CutVertex {
  private:
  const PWMapAF &fact_;

  //*** SBG info, constant
  member_class(DSBG, dsbg);

  //-----------------------------
  member_class(bool, debug);

  public:
  CutVertex(const DSBG &dsbg, bool debug);

  Set calculate(); 

  private:
  PWMap getDegMap(const DSBG &dsbg);
};

} // namespace LIB

} // namespace SBG

#endif
