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

template<typename Set>
struct SBGraph {
  // Vertex definitions
  member_class(Set, V);
  member_class(PWMap<Set>, Vmap);

  // Edge definitions
  member_class(Set, E);
  member_class(PWMap<Set>, map1);
  member_class(PWMap<Set>, map2);
  member_class(PWMap<Set>, Emap);

  SBGraph();
  SBGraph(Set V, PWMap<Set> VMap, PWMap<Set> map1, PWMap<Set> map2, PWMap<Set> Emap);
};
template<typename Set>
std::ostream &operator<<(std::ostream &out, const SBGraph<Set> &pw);

template<typename Set>
void addSV(Set vertices, SBGraph<Set> &g);

template<typename Set>
void addSE(Set edges, SBGraph<Set> &g);

template<typename Set>
PWMap<Set> connectedComponents(SBGraph<Set> g);

typedef SBGraph<UnordSet> BaseSBG;
typedef SBGraph<OrdSet> CanonSBG;

} // namespace LIB

}  // namespace SBG

#endif
