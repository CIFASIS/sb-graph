/** @file dfs_ts.hpp

 @brief <b>Concrete SBG Depth First Search Topological Sorting Algorithm
 implementation</b>

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

#ifndef SBGRAPH_ALGORITHMS_SORTING_TOPOLOGICAL_DFS_TS_HPP_
#define SBGRAPH_ALGORITHMS_SORTING_TOPOLOGICAL_DFS_TS_HPP_

#include "sbg/directed_sbg.hpp"
#include "sbg/expression.hpp"
#include "sbg/pw_map.hpp"
#include "sbg/set.hpp"
#include <vector>
#include <tuple>

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Depth First Search Topological Sorting Algorithm Implementation -------------
////////////////////////////////////////////////////////////////////////////////

using SVExprTuple = std::tuple<MD_NAT, MD_NAT, Expression>;

enum SVExprFields {From, To, Expr};

class DFS_TS {
public:
  DFS_TS();

  /**
   * @brief Concrete implementation that in each step takes out the minimum
   * vertex without dependencies. It also implements different strategies to
   * sort a large number of vertices.
   */
  PWMap calculate(const DirectedSBG& dsbg, const PWMap& pmap);

private:

  /**
   * @brief Initializes data members determined by the input SBG.
   */
  void init(const DirectedSBG& dsbg);

  void addSV(MD_NAT from_sv, MD_NAT from, MD_NAT to_sv, MD_NAT to, Expression expr);

  void checkRepetitionAndAdd(MD_NAT from, MD_NAT to, bool isSucc);

  MD_NAT getVertex();

  MD_NAT dfs(MD_NAT v, bool isRec);

  DirectedSBG _dsbg;
  PWMap _smap;
  PWMap _Vmap;
  PWMap _mapB;
  PWMap _mapD;
  Set _V;
  Set _visitedSV;
  MD_NAT _last_v;
  std::vector<SVExprTuple> _SVExprVector;
};

} // namespace detail

} // namespace LIB

} // namespace SBG

#endif // SBGRAPH_ALGORITHMS_SORTING_TOPOLOGICAL_DFS_TS_HPP_
