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

#include <sbg/sbg.hpp>

namespace SBG {

// Set-vertex --------------------------------------------------------------------------------------

#define ATOM_SET_VERTEX_TEMPLATE template <typename DESC>

#define ATOM_SETV_TEMP_TYPE AtomSetVertexImp<DESC>

ATOM_SET_VERTEX_TEMPLATE
struct AtomSetVertexImp {
  member_class(std::string, id);    // For pretty-printing
  member_class(MultiInterval, range); // Vertices of the set-vertex
  member_class(DESC, desc);           // Description

  AtomSetVertexImp();
  AtomSetVertexImp(std::string id, MultiInterval range);
  AtomSetVertexImp(std::string id, MultiInterval range, DESC desc);

  eq_class(AtomSetVertexImp);
};

printable_temp(ATOM_SET_VERTEX_TEMPLATE, ATOM_SETV_TEMP_TYPE);

typedef AtomSetVertexImp<SVDesc> AtomSetVertex;

// Set graph ---------------------------------------------------------------------------------------

typedef boost::adjacency_list<boost::listS, boost::listS, boost::undirectedS, AtomSetVertex, SetEdge> AtomSBGraph;
typedef AtomSBGraph::vertex_descriptor AtomSVDesc;
typedef AtomSBGraph::edge_descriptor AtomSEDesc;

std::ostream &operator<<(std::ostream &out, AtomSBGraph &g);

// Directed Set graph ------------------------------------------------------------------------------

typedef boost::adjacency_list<boost::listS, boost::listS, boost::directedS, AtomSetVertex, DSetEdge> AtomDSBGraph;
typedef AtomDSBGraph::vertex_descriptor AtomDSVDesc;
typedef AtomDSBGraph::edge_descriptor AtomDSEDesc;

std::ostream &operator<<(std::ostream &out, AtomDSBGraph &g);
AtomDSBGraph atomize(DSBGraph dg);

std::string get_name(Set s, AtomDSBGraph dg); // Given a set of vertices, get the name of their corresponding set-vertex
std::optional<AtomDSVDesc> get_vertex(Set s, AtomDSBGraph &dg); // Given a set of vertices, get the corresponding set-vertex

} // namespace SBG
