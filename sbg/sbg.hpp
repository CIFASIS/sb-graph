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

#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/subgraph.hpp>
#include <optional>
#include <string>

#include <sbg/descs.hpp>
#include <sbg/util/defs.hpp>
#include <sbg/interval.hpp>
#include <sbg/multi_interval.hpp>
#include <sbg/set.hpp>
#include <sbg/lmap.hpp>
#include <sbg/atom_pw_map.hpp>
#include <sbg/pw_map.hpp>

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

  SetVertexImp restrictVertex(Set dom);

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
  Set dom();

  eq_class(SetEdgeImp);
};

printable_temp(SET_EDGE_TEMPLATE, SETE_TEMP_TYPE);

typedef SetEdgeImp<SEDesc> SetEdge;

// Directed Set-edge -------------------------------------------------------------------------------

#define DSET_EDGE_TEMPLATE template <typename DESC>

#define DSETE_TEMP_TYPE DSetEdgeImp<DESC>

DSET_EDGE_TEMPLATE
struct DSetEdgeImp {
  member_class(std::string, name);  // For pretty-printing
  member_class(int, id);            // Unique identifier
  member_class(PWLMap, map_b);      // Forward map
  member_class(PWLMap, map_d);      // Backward map
  member_class(int, index);         // For debugging
  member_class(DESC, desc);         // Description

  DSetEdgeImp();
  DSetEdgeImp(std::string name, PWLMap map_b, PWLMap map_d);
  DSetEdgeImp(std::string name, PWLMap map_b, PWLMap map_d, DESC desc);
  DSetEdgeImp(std::string name, int id, PWLMap map_b, PWLMap map_d, int index);
  DSetEdgeImp(std::string name, int id, PWLMap map_b, PWLMap map_d, int index, DESC desc);

  DSetEdgeImp restrictEdge(Set dom);
  Set dom();

  eq_class(DSetEdgeImp);
};

printable_temp(DSET_EDGE_TEMPLATE, DSETE_TEMP_TYPE);

typedef DSetEdgeImp<SEDesc> DSetEdge;

// Set graph ---------------------------------------------------------------------------------------

typedef boost::adjacency_list<boost::listS, boost::listS, boost::undirectedS, SetVertex, SetEdge> SBGraph;
typedef SBGraph::vertex_descriptor SetVertexDesc;
typedef boost::graph_traits<SBGraph>::vertex_iterator VertexIt;
typedef SBGraph::edge_descriptor SetEdgeDesc;
typedef boost::graph_traits<SBGraph>::edge_iterator EdgeIt;
typedef boost::graph_traits<SBGraph>::out_edge_iterator OutEdgeIt;

std::ostream &operator<<(std::ostream &out, SBGraph &g);

std::string get_name(Set s, SBGraph dg); // Given a set of vertices, get the name of their corresponding set-vertex
std::optional<SetVertexDesc> get_vertex(Set s, SBGraph &dg); // Given a set of vertices, get the corresponding set-vertex

// Directed Set graph ------------------------------------------------------------------------------

typedef boost::adjacency_list<boost::listS, boost::listS, boost::directedS, SetVertex, DSetEdge> DSBGraph;
typedef DSBGraph::vertex_descriptor DSetVertexDesc;
typedef boost::graph_traits<DSBGraph>::vertex_iterator DVertexIt;
typedef DSBGraph::edge_descriptor DSetEdgeDesc;
typedef boost::graph_traits<DSBGraph>::edge_iterator DEdgeIt;

std::ostream &operator<<(std::ostream &out, DSBGraph &dg);

std::string get_name(Set s, DSBGraph dg); // Given a set of vertices, get the name of their corresponding set-vertex
std::optional<DSetVertexDesc> get_vertex(Set s, DSBGraph &dg); // Given a set of vertices, get the corresponding set-vertex

}  // namespace SBG
