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

Vertex::Vertex(SBG::LIB::MD_NAT id) : id_(id) {}

const SBG::LIB::MD_NAT& Vertex::id() const
{
  return id_;
}

bool Vertex::operator==(const Vertex &other) const
{
  return id() == other.id();
}

std::ostream &operator<<(std::ostream &out, const Vertex &v)
{
  out << v.id();
  return out;
}

// Edge ------------------------------------------------------------------------

Edge::Edge() : id_(0) {}

Edge::Edge(SBG::LIB::MD_NAT id) : id_(id) {}

const SBG::LIB::MD_NAT& Edge::id() const
{
  return id_;
}

bool Edge::operator==(const Edge &other) const { return id() == other.id(); }

std::ostream &operator<<(std::ostream &out, const Edge &e)
{
  out << e.id();
  return out;
}

}  // namespace OG
