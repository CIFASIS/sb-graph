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

namespace SBG {

namespace IO {

// Directed graph converter -----------------------------------------------------------------------

typedef std::map<MultiInterval, MultiInterval> ConverterVertexMap;

struct DirectedConverter {
  DirectedConverter();
  DirectedConverter(DSBGraph dg);

  member_class(DSBGraph, dg);
  member_class(ConverterVertexMap, converted_vertices);

  GraphIO convert_graph();
  //AnnotatedGraphIO convert_with_annotations(Annotations ans);
  RMapIO convert_map(PWLMap rmap);

  private:
  Range convert_interval(Interval i);
  Ranges convert_multi_interval(MultiInterval mi);
  
  Iterators create_iterators(MultiInterval mi);
  Ranges create_ranges(MultiInterval mi);
  LinearExps create_exp(MultiInterval mi);

  VertexDef convert_vertex(DSetVertexDesc vd);
  EdgeDefs convert_edge(DSetEdgeDesc ed);
};

} // namespace IO

} // namespace SBG
