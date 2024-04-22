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
#include <map>

#include <test/performance/boost/ordinary_graph.hpp>
#include <sbg/sbg.hpp>

namespace OG {

template<typename Set>
class OrdinaryGraphBuilder {
  using PW = SBG::LIB::PWMap<Set>;
  using SBGraph = SBG::LIB::SBGraph<Set>;

  public:
  OrdinaryGraphBuilder(SBGraph graph);
  ~OrdinaryGraphBuilder() = default;

  virtual OG::Graph build();

  protected:
  OG::VertexDesc addVertex(SBG::Util::MD_NAT id, OG::Graph &g);
  OG::EdgeDesc addEdge(SBG::Util::MD_NAT id, OG::Graph &g);

  SBGraph _sb_graph;
  std::map<SBG::Util::MD_NAT, OG::VertexDesc> _vertex_map;
};

typedef OrdinaryGraphBuilder<SBG::LIB::UnordSet> BaseBuilder;
typedef OrdinaryGraphBuilder<SBG::LIB::OrdSet> CanonBuilder; 

template<typename Set>
class OrdinaryDGraphBuilder {
  using PW = SBG::LIB::PWMap<Set>;
  using DSBGraph = SBG::LIB::DSBGraph<Set>;

  public:
  OrdinaryDGraphBuilder(DSBGraph graph);
  ~OrdinaryDGraphBuilder() = default;

  virtual OG::DGraph build();

  protected:
  OG::DVertexDesc addVertex(SBG::Util::MD_NAT id, OG::DGraph &g);
  OG::DEdgeDesc addEdge(SBG::Util::MD_NAT id, OG::DGraph &g);

  DSBGraph _sb_graph;
  std::map<SBG::Util::MD_NAT, OG::DVertexDesc> _vertex_map;
};

typedef OrdinaryDGraphBuilder<SBG::LIB::UnordSet> BaseDBuilder;
typedef OrdinaryDGraphBuilder<SBG::LIB::OrdSet> CanonDBuilder; 

}  // namespace OG

#endif
