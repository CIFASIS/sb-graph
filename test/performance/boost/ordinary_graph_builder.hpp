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

#ifndef PERF_ORDINARY_BUILDER_HPP
#define PERF_ORDINARY_BUILDER_HPP

#include <map>

#include "test/performance/boost/ordinary_graph.hpp"
#include "sbg/sbg.hpp"
#include "util/logger.hpp"

namespace OG {

using EdgeVector = std::vector<Edge>;

class OrdinaryGraphBuilder {
  public:
  OrdinaryGraphBuilder(SBG::LIB::SBG sbg);

  Graph build();
  Graph build(SBG::LIB::NAT number_vertices, EdgeVector& edges);
  void translateVertices();
  EdgeVector getEdgeList();

  private:
  const SBG::LIB::SBG _sbg; ///< Input SBG to convert
  std::map<SBG::LIB::MD_NAT, Vertex> _vertex_map;
    ///< Auxiliar map for addEdge 
};

}  // namespace OG

#endif
