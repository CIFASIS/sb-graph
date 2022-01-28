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

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/subgraph.hpp>

#include <sbg/descs.h>
#include <sbg/defs.h>
#include <sbg/interval.h>
#include <sbg/multi_interval.h>
#include <sbg/set.h>
#include <sbg/lmap.h>
#include <sbg/atom_pw_map.h>
#include <sbg/pw_map.h>

namespace SBG {

// Set-vertex --------------------------------------------------------------------------------------

#define SET_VERTEX_TEMPLATE template <typename DESC>

#define SETV_TEMP_TYPE SetVertexImp<DESC>

SET_VERTEX_TEMPLATE
struct SetVertexImp {
  member_class(std::string, name);  // For pretty-printing
  member_class(int, id);            // Unique identifier
  member_class(Set, range);         // Vertices of the set-vertex
  member_class(int, index);         // For debugging
  member_class(DESC, desc);         // Description

  SetVertexImp();
  SetVertexImp(std::string name, Set range);
  SetVertexImp(std::string name, int id, Set range, int index);
  SetVertexImp(std::string name, int id, Set range, int index, DESC desc);

  eq_class(SetVertexImp);
};

printable_temp(SET_VERTEX_TEMPLATE, SETV_TEMP_TYPE);

typedef SetVertexImp<SVDesc> SetVertex;

// Set-edge ----------------------------------------------------------------------------------------

#define SET_EDGE_TEMPLATE template <typename DESC>

#define SETE_TEMP_TYPE SetEdgeImp<DESC>

SET_EDGE_TEMPLATE
struct SetEdgeImp {
  member_class(std::string, name);  // For pretty-printing
  member_class(int, id);            // Unique identifier
  member_class(PWLMap, map_f);      // Map from edges to "left" vertices
  member_class(PWLMap, map_u);      // Map from edges to "right" vertices
  member_class(int, index);         // For debugging
  member_class(DESC, desc);         // Description

  SetEdgeImp();
  SetEdgeImp(std::string name, PWLMap map_f, PWLMap map_u);
  SetEdgeImp(std::string name, PWLMap map_f, PWLMap map_u, DESC desc);
  SetEdgeImp(std::string name, int id, PWLMap map_f, PWLMap map_u, int index);
  SetEdgeImp(std::string name, int id, PWLMap map_f, PWLMap map_u, int index, DESC desc);

  SetEdgeImp restrictEdge(Set dom);

  eq_class(SetEdgeImp);
};

printable_temp(SET_EDGE_TEMPLATE, SETE_TEMP_TYPE);

typedef SetEdgeImp<SEDesc> SetEdge;

// Set graph ---------------------------------------------------------------------------------------

typedef boost::adjacency_list<boost::listS, boost::listS, boost::undirectedS, SetVertex, SetEdge> SBGraph;
typedef SBGraph::vertex_descriptor SetVertexDesc;
typedef boost::graph_traits<SBGraph>::vertex_iterator VertexIt;
typedef SBGraph::edge_descriptor SetEdgeDesc;
typedef boost::graph_traits<SBGraph>::edge_iterator EdgeIt;
typedef boost::graph_traits<SBGraph>::out_edge_iterator OutEdgeIt;

}  // namespace SBG
