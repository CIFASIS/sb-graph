/** @file ordinary_graph_builder.hpp

 @brief <b>Ordinary Graph Builder</b>

 Module in charge of constructing the scalar graph used as input of the scalar
 causalization from a SBG. The generated result will be used as input of the
 Edmonds-Karp maximum matching algorithm.

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

#ifndef SBGRAPH_TEST_PERFORMANCE_BOOST_ORDINARY_GRAPH_BUILDER_HPP_
#define SBGRAPH_TEST_PERFORMANCE_BOOST_ORDINARY_GRAPH_BUILDER_HPP_

#include "sbg/bipartite_sbg.hpp"
#include "sbg/natural.hpp"
#include "test/performance/boost/ordinary_graph.hpp"
#include "util/logger.hpp"

#include <map>
#include <vector>

namespace OG {

class OrdinaryGraphBuilder {
public:
  OrdinaryGraphBuilder(SBG::LIB::BipartiteSBG bsbg);

  BipartiteGraph build();
  BipartiteGraph build(SBG::LIB::NAT number_vertices, EdgeVector& edges
    , std::vector<int>&& partition);
  void translateVertices();
  EdgeVector getEdgeList();

private:
  const SBG::LIB::BipartiteSBG _bsbg; ///< Input bipartite SBG to convert
  std::map<SBG::LIB::MD_NAT, Vertex> _vertex_map;
    ///< Map from SBG vertex identifier to Graph element
  std::vector<int> _partition;
};

}  // namespace OG

#endif // SBGRAPH_TEST_PERFORMANCE_BOOST_ORRDINARY_GRAPH_BUILDER_HPP_
