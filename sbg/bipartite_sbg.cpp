/*******************************************************************************

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

#include "sbg/bipartite_sbg.hpp"

namespace SBG {

namespace LIB {

BipartiteSBG::BipartiteSBG()
  : _X(SET_FACT.createSet()), _Y(SET_FACT.createSet()) {}

BipartiteSBG::BipartiteSBG(const Set& V, const PWMap& Vmap
  , const PWMap& map1, const PWMap& map2
  , const PWMap& Emap, const PWMap& subEmap
  , const Set& X, const Set& Y)
  : SBG(V, Vmap, map1, map2, Emap, subEmap), _X(X), _Y(Y) {}

const Set& BipartiteSBG::X() const { return _X; }

const Set& BipartiteSBG::Y() const { return _Y; }

std::ostream& operator<<(std::ostream& out, const BipartiteSBG& g)
{
  out << "V: " << g.V() << "\n";
  out << "Vmap: " << g.Vmap() << "\n\n";
  out << "E: " << g.E() << "\n";
  out << "map1: " << g.map1() << "\n";
  out << "map2: " << g.map2() << "\n";
  out << "Emap: " << g.Emap() << "\n";
  out << "subEmap: " << g.subEmap() << "\n";
  out << "X: " << g.X() << "\n";
  out << "Y: " << g.Y() << "\n";

  return out;
}

} // namespace LIB

} // namespace SBG
