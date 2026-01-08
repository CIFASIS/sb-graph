/** @file ordinary_graph.hpp

 @brief <b>Ordinary Graph</b>

 Module that defines the structure for scalar graphs.

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

#ifndef PERF_ORDINARY_GRAPH_HPP
#define PERF_ORDINARY_GRAPH_HPP

#include <boost/graph/adjacency_list.hpp>

#include "sbg/natural.hpp"

namespace OG {

using Vertex = SBG::LIB::NAT;
using VertexVector = std::vector<Vertex>;
using Edge = std::pair<SBG::LIB::NAT, SBG::LIB::NAT>;
using Graph = boost::adjacency_list<boost::vecS, boost::vecS
  , boost::undirectedS>;

using DGraph = boost::adjacency_list<boost::vecS, boost::vecS
  , boost::bidirectionalS>;

}  // namespace OG

#endif
