/*****************************************************************************

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

#ifndef PERF_ORD_BUILDER_HPP
#define PERF_ORD_BUILDER_HPP

#include <cassert>
#include <chrono>
#include <map>

#include <test/performance/boost/ordinary_graph.hpp>
#include <sbg/sbg.hpp>
#include <util/logger.hpp>

namespace OG {

class OrdinaryGraphBuilder {
  public:
  OrdinaryGraphBuilder(SBG::LIB::SBG graph);
  ~OrdinaryGraphBuilder() = default;

  virtual OG::Graph build();

  protected:
  OG::VertexDesc addVertex(SBG::LIB::MD_NAT id, OG::Graph &g);
  OG::EdgeDesc addEdge(SBG::LIB::MD_NAT id, OG::Graph &g);

  SBG::LIB::SBG _sb_graph;
  std::map<SBG::LIB::MD_NAT, OG::VertexDesc> _vertex_map;
};

class OrdinaryDGraphBuilder {
  public:
  OrdinaryDGraphBuilder(SBG::LIB::DSBG graph);
  ~OrdinaryDGraphBuilder() = default;

  virtual OG::DGraph build();

  protected:
  OG::DVertexDesc addVertex(SBG::LIB::MD_NAT id, OG::DGraph &g);
  OG::DEdgeDesc addEdge(SBG::LIB::MD_NAT id, SBG::LIB::MD_NAT v1
    , SBG::LIB::MD_NAT v2, OG::DGraph &g);

  SBG::LIB::DSBG _sb_graph;
  std::map<SBG::LIB::MD_NAT, OG::DVertexDesc> _vertex_map;
};

}  // namespace OG

#endif
