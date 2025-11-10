/** @file cut_vertex.hpp

 @brief <b>SBG Vertex Cut Set Algorithm Abstract Interface</b>

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

#include "algorithms/scc/scc_fact.hpp"

namespace SBG {

namespace LIB {

///////////////////////////////////////////////////////////////////////////////
// Vertex Cut Set Algorithm Abstract Strategy ---------------------------------
///////////////////////////////////////////////////////////////////////////////

class CVStrategy;

typedef std::unique_ptr<CVStrategy> CVStratPtr;

/**
* @brief Aims to calculate a minimum cut set of vertices, that is, a set of
* vertices such that if these vertices are taken out, the resulting graph has no
* SCC left. Since this is a NP-hard problem, heuristics are used, and thus is
* not guaranteed that the set is actually minimum.
*/
class CVStrategy {
  public:
  virtual ~CVStrategy() = default;

  CVStrategy();

  virtual Set calculate(const DSBG& dsbg) const = 0;
};

///////////////////////////////////////////////////////////////////////////////
// Vertex Cut Set Algorithm Interface (context) ------------------------------- 
///////////////////////////////////////////////////////////////////////////////

class CutVertex {
  public:
  CutVertex(CVStratPtr strat);

  Set calculate(const DSBG& dsbg) const;

  private:
  CVStratPtr strategy_;
};

} // namespace LIB

} // namespace SBG

#endif
