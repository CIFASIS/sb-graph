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
#include <sbg/sbg_io.hpp>
#include <sbg/util/defs.hpp>

#include <boost/bimap.hpp>

namespace SBG {

namespace IO {

typedef std::map<MultiInterval, LMap> CompactVertexMap;
typedef std::pair<MultiInterval, LMap> CompPair;

// Undirected graph converter -----------------------------------------------------------------------

typedef boost::bimap<DSetVertexDesc, VertexDef> UConverterVertexMap;
typedef UConverterVertexMap::value_type VConv;

struct UndirectedConverter {
  UndirectedConverter();
  UndirectedConverter(SBGraph g);

  member_class(SBGraph, g);
  member_class(CompactVertexMap, compacted_vertices);
  member_class(UConverterVertexMap, converted_vertices);
  member_class(GraphIO, converted_g);

  GraphIO convert_graph();
  MatchingIO convert_matching(Set edges);
  RMapIO convert_map(PWLMap rmap);

  VertexDef get_vertex_def(std::string vertex_name);
  EdgeDefs get_edge(SetEdge e, std::string s_name, std::string t_name);

  private:
  Range convert_interval(Interval i);
  Ranges convert_multi_interval(MultiInterval mi);
  
  Iterators create_iterators(MultiInterval mi);
  Ranges create_ranges(MultiInterval mi);
  LinearExps create_exp(MultiInterval mi);

  VertexDef convert_vertex(SetVertexDesc vd);
  EdgeDef convert_subsetedge(SetEdge e, MultiInterval mi_dom, std::string s_name, std::string t_name);
  EdgeDefs convert_edge(SetEdgeDesc ed);

  CompPair get_vertex(MultiInterval mi);
};

// Directed graph converter -----------------------------------------------------------------------

typedef boost::bimap<DSetVertexDesc, VertexDef> DConverterVertexMap;
typedef DConverterVertexMap::value_type DVConv;

struct DirectedConverter {
  DirectedConverter();
  DirectedConverter(DSBGraph dg);

  member_class(DSBGraph, dg);
  member_class(CompactVertexMap, compacted_vertices);
  member_class(DConverterVertexMap, converted_vertices);
  member_class(GraphIO, converted_dg);

  GraphIO convert_graph();
  RMapIO convert_map(PWLMap rmap);

  VertexDef get_vertex_def(std::string vertex_name);
  EdgeDefs get_edge(DSetEdge e, std::string s_name, std::string t_name);

  private:
  Range convert_interval(Interval i);
  Ranges convert_multi_interval(MultiInterval mi);
  
  Iterators create_iterators(MultiInterval mi);
  Ranges create_ranges(MultiInterval mi);
  LinearExps create_exp(MultiInterval mi);

  VertexDef convert_vertex(DSetVertexDesc vd);
  EdgeDef convert_subsetedge(DSetEdge e, MultiInterval mi_dom, std::string s_name, std::string t_name);
  EdgeDefs convert_edge(DSetEdgeDesc ed);

  CompPair get_vertex(MultiInterval mi);
};

} // namespace IO

} // namespace SBG
