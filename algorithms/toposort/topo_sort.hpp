/** @file topo_sort.hpp

 @brief <b>SBG Topological Sort Algorithm Abstract Interface</b>

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

#ifndef SBG_TOPOSORT_HPP
#define SBG_TOPOSORT_HPP

#include "sbg/directed_sbg.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Topological Sort Algorithm Abstract Strategy --------------------------------
////////////////////////////////////////////////////////////////////////////////

class TSStrategy;

typedef std::unique_ptr<TSStrategy> TSStratPtr;

class TSStrategy {
  public:
  virtual ~TSStrategy() = default;

  TSStrategy();

  virtual PWMap calculate(const DSBG& dsbg) const = 0;

  virtual PWMap calculate(const DSBG& dsbg, const PWMap& rmap) const = 0;
};

////////////////////////////////////////////////////////////////////////////////
// Topological Sort Algorithm Interface (context) ------------------------------
////////////////////////////////////////////////////////////////////////////////

class TopoSort {
  private:
  TSStratPtr strategy_;

  public:
  TopoSort(TSStratPtr strat);

  PWMap calculate(const DSBG& dsbg) const;

  PWMap calculate(const DSBG& dsbg, const PWMap& rmap) const;
};

} // namespace LIB

} // namespace SBG

#endif
