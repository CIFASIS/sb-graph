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

#include <test/performance/boost/ordinary_graph.hpp>

namespace OG {

using namespace std;

// Vertex ----------------------------------------------------------------------

Vertex::Vertex() : id_(0) {}

Vertex::Vertex(SBG::Util::MD_NAT id) : id_(id) {}

member_imp(Vertex, SBG::Util::MD_NAT, id);

bool Vertex::operator==(const Vertex &other) const
{
  return id() == other.id();
}

std::ostream &operator<<(std::ostream &out, const Vertex &v)
{
  out << v.id() << "\n";
  return out;
}

// Edge ------------------------------------------------------------------------

Edge::Edge() : id_(0) {}

Edge::Edge(SBG::Util::MD_NAT id) : id_(id) {}

member_imp(Edge, SBG::Util::MD_NAT, id);

bool Edge::operator==(const Edge &other) const { return id() == other.id(); }

std::ostream &operator<<(std::ostream &out, const Edge &e)
{
  out << e.id() << "\n";
  return out;
}

}  // namespace OG
