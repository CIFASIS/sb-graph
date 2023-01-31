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

#include <sbg/sbg_algorithms.hpp>

namespace SBG {

typedef std::map<SetVertexDesc, DSetVertexDesc> VertexMap;
typedef std::map<SetEdgeDesc, DSetEdgeDesc> EdgeMap;

struct SCCGraphBuilder { 
  member_class(MatchingStruct, mtchng);
  member_class(VertexMap, vertex_map); // A mapping from matching vertices to SCC vertices
  member_class(EdgeMap, edge_map); // A mapping from matching edges to SCC edges

  SCCGraphBuilder(MatchingStruct m);

  DSBGraph build();

  private:
  bool fullyMatched(Set vertices);
};


} // namespace SBG
