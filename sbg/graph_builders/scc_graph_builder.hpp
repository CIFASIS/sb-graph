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
#include <sbg/converter_io.hpp>
#include <sbg/sbg_algorithms.hpp>

namespace SBG {

typedef std::map<SetEdgeDesc, DSetVertexDesc> SCCVertexMap;

struct SCCGraphBuilder { 
  member_class(MatchingStruct, mtchng);
  member_class(DSBGraph, result);
  member_class(SCCVertexMap, vertex_map); // A mapping from matched edges to their respective vertex in the directed graph
  //member_class(SBG::IO::Annotations, merged_vertices);

  SCCGraphBuilder(MatchingStruct m);

  void build();
  void pretty_print();

  private:
  bool fullyMatched(Set vertices);
  void partitionEdges(SBGraph &grph);
  void createVertices(SBGraph &grph);
  SetEdgeDesc findEdge(SBGraph &grph, Set dom);
  void createEdges(SBGraph &grph);
  DSBGraph combineEdges();

  void pretty_print_vertex(DSBGraph grph, DSetVertexDesc vd);
  void pretty_print_edge(DSBGraph grph, DSetEdgeDesc ed);
};

} // namespace SBG
