/** @file topo_sort.hpp

 @brief <b>SBG Topological Sort Algorithm implementation</b>

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
// Topological Sort Algorithm Abstract Delegate --------------------------------
////////////////////////////////////////////////////////////////////////////////

struct TSDelegate;

typedef std::unique_ptr<TSDelegate> TSDelegPtr;

struct TSDelegate {
  protected:
  const PWMapAF &fact_;

  public:
  virtual ~TSDelegate() = default;

  TSDelegate(const PWMapAF &fact);

  virtual PWMap calculate(const DSBG &dsbg) const = 0;
};

////////////////////////////////////////////////////////////////////////////////
// Minimum Vertex Topological Sort Algorithm Implementation (concrete delegate)
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief In each step takes out the minimum vertex without dependencies.
 */
struct MinVertexTopoSort : public TSDelegate {
  MinVertexTopoSort(const PWMapAF &fact);

  PWMap calculate(const DSBG &dsbg) const override; 
};

////////////////////////////////////////////////////////////////////////////////
// Topological Sort Algorithm Implementation (delegator) -----------------------
////////////////////////////////////////////////////////////////////////////////

struct TopoSort {
  private:
  TSDelegPtr delegate_;

  public:
  TopoSort(TSDelegPtr deleg);

  PWMap calculate(const DSBG &dsbg) const;
};

} // namespace LIB

} // namespace SBG

#endif
