/** @file maxdeg_cv.hpp

 @brief <b>SBG Maximum Degree Vertex Cut Set Algorithm implementation</b>

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

#ifndef SBG_MAXDEG_CUTVERTEX_HPP
#define SBG_MAXDEG_CUTVERTEX_HPP

#include "algorithms/cutvertex/cut_vertex.hpp"
#include "algorithms/scc/scc_fact.hpp"

namespace SBG {

namespace LIB {

///////////////////////////////////////////////////////////////////////////////
// Maximum Degree Vertex Cut Set Algorithm Implementation (concrete strategy) -
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief In each step takes out the vertex of maximum degree.
 */
struct MaxDegCutVertex : public CVStrategy {
  MaxDegCutVertex(const PWMapAF& pw_fact, const SCCFact& scc_fact);

  Set calculate(const DSBG& dsbg) const override;

  private:
  PWMap getDegMap(const DSBG& dsbg) const;
};

} // namespace LIB

} // namespace SBG

#endif
