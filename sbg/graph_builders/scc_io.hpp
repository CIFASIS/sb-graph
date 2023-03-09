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

struct Annotation {
  Annotation();
  Annotation(SetVertex og_lvertex, SetVertex og_rvertex, SetVertex lvertex, SetVertex rvertex);

  member_class(SetVertex, og_lvertex);
  member_class(SetVertex, og_rvertex);
  member_class(SetVertex, lvertex);
  member_class(SetVertex, rvertex);
};

std::ostream &operator<<(std::ostream &out, Annotation &a);

typedef std::list<Annotation> Annotations;
std::ostream &operator<<(std::ostream &out, Annotations &as);

struct AnnotatedGraphIO {
  AnnotatedGraphIO();
  AnnotatedGraphIO(Annotations ans, GraphIO g);

  member_class(Annotations, ans);
  member_class(GraphIO, g);
};

std::ostream &operator<<(std::ostream &out, AnnotatedGraphIO &g);

// Converter --------------------------------------------------------------------------------------

struct SCCConverter {
  SCCConverter();
  SCCConverter(DSBGraph dg);

  member_class(AtomDSBGraph, dg);

  GraphIO convert_graph();
  AnnotatedGraphIO convert_with_annotations(Annotations ans);
  RMapIO convert_map(PWLMap rmap);

  private:
  Range convert_interval(Interval i);
  Ranges convert_multi_interval(MultiInterval mi);
  VertexDef convert_vertex(AtomDSVDesc vd);

  Iterators create_iterators(MultiInterval mi);
  Ranges create_ranges(MultiInterval mi);
  LinearExps create_exp(MultiInterval mi);
  EdgeDefs convert_edge(AtomDSEDesc ed);
 
  std::string get_vertex(MultiInterval mi);
};

} // namespace IO

} // namespace SBG
