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

namespace SBG {

namespace IO {

// Converter --------------------------------------------------------------------------------------

struct SCCConverter {
  SCCConverter();
  SCCConverter(DSBGraph dg);

  member_class(AtomDSBGraph, dg);

  GraphIO convert();

  private:
  Range convert_interval(Interval i);
  Ranges convert_multi_interval(MultiInterval mi);
  VertexDef convert_vertex(AtomDSVDesc vd);

  Iterators create_iterators(MultiInterval mi);
  Ranges create_ranges(MultiInterval mi);
  LinearExps create_exp(MultiInterval mi);
  EdgeDefs convert_edge(AtomDSEDesc ed);
};

} // namespace IO

} // namespace SBG
