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

#pragma once

#include <sbg/sbg.hpp>
#include <sbg/util/ordinary_graph.hpp>

namespace OG {

class OrdinaryGraphBuilder {
  public:
  OrdinaryGraphBuilder(SBG::SBGraph graph);
  ~OrdinaryGraphBuilder() = default;

  virtual OG::Graph build();

  protected:
  OG::VertexDesc addVertex(std::string vertex_name, OG::Graph& graph);
  std::string getNodeName(std::string name, SBG::ORD_INTS elems);

  SBG::SBGraph _sb_graph;
  std::map<std::string, OG::VertexDesc> _vertex_map;
};

}  // namespace OG
