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
#include <sbg/sbg_algorithms.hpp>

namespace SBG {

struct OrderGraphBuilder {
  OrderGraphBuilder();
  OrderGraphBuilder(SCCStruct scc);

  member_class(SCCStruct, scc);
  member_class(Set, part_edges); // For each atom set left vertices have the same rep (same with right vertices)
  member_class(PWLMap, rmapB);
  member_class(PWLMap, rmapD);
  member_class(DSBGraph, result);

  void build();

  private:
  Set get_representants();
  void create_vertices();
  void create_edges();
};

} // namespace SBG
