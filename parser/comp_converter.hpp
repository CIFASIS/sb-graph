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
#include <parser/compact_parser.hpp>

namespace Parser {

// Converter --------------------------------------------------------------------------------------

typedef std::map<std::string, SBG::MultiInterval> CompVertexMap;
typedef std::map<int, Parser::VariantEdge> CompEdgeMap;

typedef std::pair<std::string, std::string> VertexPair;

struct CompConverter {
  member_class(SBG::OrdCT<SBG::INT>, vertex_offset);
  member_class(SBG::OrdCT<SBG::INT>, edge_offset);
  member_class(CompVertexMap, vmap);
  member_class(Parser::CompSetGraph, sg);

  CompConverter();
  CompConverter(Parser::CompSetGraph sg);

  Grph convertGraph();

  private:
  SBG::SetVertex convertVertex(Parser::CompSetVertex sv);

  SBG::MultiInterval createDom(SBG::MultiInterval mi);
  SBG::LMap createMap(SBG::MultiInterval dom, SBG::MultiInterval sbs, std::string vertex);
  VariantEdge convertEdge(Parser::CompSetEdge se);

  SBG::SBGraph convertUndirectedGraph();
  SBG::DSBGraph convertDirectedGraph();
};

} // namespace Parser
