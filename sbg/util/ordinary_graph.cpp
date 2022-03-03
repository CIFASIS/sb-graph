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

#include <sbg/util/ordinary_graph.hpp>

namespace OG {

// Vertex --------------------------------------------------------------------------------------

Vertex::Vertex() : name_(""), id_(-1) {}

Vertex::Vertex(std::string name, int id) : name_(name), id_(id) {}

member_imp(Vertex, std::string, name);
member_imp(Vertex, int, id);

bool Vertex::operator==(const Vertex &other) const { return id() == other.id(); }

std::ostream &operator<<(std::ostream &out, const Vertex &v)
{
  out << v.name() << ": " << v.id() << "\n";
  return out;
}

// Edge ----------------------------------------------------------------------------------------

Edge::Edge() : name_(""), id_(-1) {}

Edge::Edge(std::string name, int id) : name_(name), id_(id) {}

member_imp(Edge, std::string, name);
member_imp(Edge, int, id);

bool Edge::operator==(const Edge &other) const { return id() == other.id(); }

std::ostream &operator<<(std::ostream &out, const Edge &e)
{
  out << e.name() << ": " << e.id() << "\n";
  return out;
}

}  // namespace OG
