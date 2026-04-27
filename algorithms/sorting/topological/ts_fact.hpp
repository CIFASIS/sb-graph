/** @file ts_fact.hpp

 @brief <b>Topological Sorting Algorithm Factory</b>

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

#ifndef SBGRAPH_ALGORITHMS_SORTING_TOPOLOGICAL_TS_FACT_HPP_
#define SBGRAPH_ALGORITHMS_SORTING_TOPOLOGICAL_TS_FACT_HPP_

#include "algorithms/sorting/topological/min_vertex_ts.hpp"
#include "algorithms/sorting/topological/topological_sorting.hpp"

namespace SBG {

namespace LIB {

#define TS_FACT TSFactory::instance()

class MinVertexTSFact {
public:
  MinVertexTSFact() = default;

  TopologicalSorting createTSAlgorithm() const;
};

/**
 * @brief Single instance of topological sorting factory to be used by clients
 * in need of creating an instance of a matching algorithm. A client includes
 * this file and calls TS_FACT.createTSAlgorithm(args).
 */
class TSFactory {
public:
  ~TSFactory() = default;

  static TSFactory& instance();
  const TSKind& kind() const;

  void set_ts_fact(TSKind kind);

  TopologicalSorting createTSAlgorithm() const;

private:
  using TSFactImpl = std::variant<MinVertexTSFact>;

  TSFactory();

  TSKind _kind;
  TSFactImpl _impl;
};

} // namespace LIB

}  // namespace SBG

#endif // SBGRAPH_ALGORITHMS_SORTING_TOPOLOGICAL_TS_FACT_HPP_
