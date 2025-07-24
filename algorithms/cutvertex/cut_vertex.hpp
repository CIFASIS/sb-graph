/** @file cut_vertex.hpp

 @brief <b>SBG Vertex Cut Set Algorithm implementation</b>

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

#include "algorithms/scc/af_scc.hpp"

namespace SBG {

namespace LIB {

///////////////////////////////////////////////////////////////////////////////
// Vertex Cut Set Algorithm Abstract Delegate ---------------------------------
///////////////////////////////////////////////////////////////////////////////

struct CVDelegate;

typedef std::unique_ptr<CVDelegate> CVDelegPtr;

/**
* @brief Aims to calculate a minimum cut set of vertices, that is, a set of
* vertices such that if these vertices are taken out, the resulting graph has no
* SCC left. Since this is a NP-hard problem, heuristics are used, and thus is
* not guaranteed that the set is actually minimum.
*/
struct CVDelegate {
  protected:
  const PWMapAF &pw_fact_;
  const SCCAF &scc_fact_;

  public:
  virtual ~CVDelegate() = default;

  CVDelegate(const PWMapAF &pw_fact, const SCCAF &scc_fact);

  virtual Set calculate(const DSBG &dsbg) const = 0;
};

///////////////////////////////////////////////////////////////////////////////
// Maximum Degree Vertex Cut Set Algorithm Implementation (concrete delegate) -
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief In each step takes out the vertex of maximum degree.
 */
struct MaxDegCutVertex : public CVDelegate {
  MaxDegCutVertex(const PWMapAF &pw_fact, const SCCAF &scc_fact);

  Set calculate(const DSBG &dsbg) const override;

  private:
  PWMap getDegMap(const DSBG &dsbg) const;
};

///////////////////////////////////////////////////////////////////////////////
// Vertex Cut Set Algorithm Implementation (delegator) ------------------------ 
///////////////////////////////////////////////////////////////////////////////

struct CutVertex {
  private:
  CVDelegPtr delegate_;

  public:
  CutVertex(CVDelegPtr deleg);

  Set calculate(const DSBG &dsbg) const;
};

} // namespace LIB

} // namespace SBG

#endif
