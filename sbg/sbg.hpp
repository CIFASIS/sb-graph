/** @file sbg.hpp

 @brief <b>Set-based graph implementation</b>

 <hr>

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

#ifndef SBG_SBG_HPP
#define SBG_SBG_HPP

#include "sbg/pw_map.hpp"

namespace SBG {

namespace LIB {

// Undirected SBG --------------------------------------------------------------

struct SBGraph {
  // Vertex definitions
  member_class(Set, V);
  member_class(PWMap, Vmap);

  // Edge definitions
  member_class(Set, E);
  member_class(PWMap, map1);
  member_class(PWMap, map2);
  member_class(PWMap, Emap);

  SBGraph();
  SBGraph(Set V, PWMap VMap, PWMap map1, PWMap map2, PWMap Emap);
};
std::ostream &operator<<(std::ostream &out, const SBGraph &pw);

void addSV(Set vertices, SBGraph &g);
unsigned int nmbrSV(SBGraph g);

void addSE(Set edges, SBGraph &g);

SBGraph copy(unsigned int times, SBGraph g);

// Directed SBG ----------------------------------------------------------------

struct DSBGraph {
  // Vertex definitions
  member_class(Set, V);
  member_class(PWMap, Vmap);

  // Edge definitions
  member_class(Set, E);
  member_class(PWMap, mapB);
  member_class(PWMap, mapD);
  member_class(PWMap, Emap);

  DSBGraph();
  DSBGraph(Set V, PWMap VMap, PWMap mapB, PWMap mapD, PWMap Emap);
};
std::ostream &operator<<(std::ostream &out, const DSBGraph &pw);

void addSV(Set vertices, DSBGraph &g);
unsigned int nmbrSV(DSBGraph g);

void addSE(Set edges, DSBGraph &g);

} // namespace LIB

}  // namespace SBG

#endif
