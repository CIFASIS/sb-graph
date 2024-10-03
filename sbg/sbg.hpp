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

template<typename Set>
struct SBGraph {
  using PW = PWMap<Set>;

  // Vertex definitions
  member_class(Set, V);
  member_class(PW, Vmap);

  // Edge definitions
  member_class(Set, E);
  member_class(PW, map1);
  member_class(PW, map2);
  member_class(PW, Emap);
  member_class(PW, subE_map);

  SBGraph();
  SBGraph(Set V, PW Vmap, PW map1, PW map2, PW Emap);

  SBGraph addSV(const Set &vertices) const;
  SBGraph addSE(const PW &pw1, const PW &pw2) const;
  SBGraph copy(unsigned int times) const;
};
template<typename Set>
std::ostream &operator<<(std::ostream &out, const SBGraph<Set> &pw);

typedef SBGraph<UnordSet> BaseSBG;
typedef SBGraph<OrdSet> CanonSBG;

// Directed SBG ----------------------------------------------------------------

template<typename Set>
struct DSBGraph {
  using PW = PWMap<Set>;

  // Vertex definitions
  member_class(Set, V);
  member_class(PW, Vmap);

  // Edge definitions
  member_class(Set, E);
  member_class(PW, mapB);
  member_class(PW, mapD);
  member_class(PW, Emap);
  member_class(PW, subE_map);

  DSBGraph();
  DSBGraph(Set V, PW Vmap, PW mapB, PW mapD, PW Emap);

  DSBGraph addSV(const Set &vertices) const;
  DSBGraph addSE(const PW &pw1, const PW &pw2) const;

  DSBGraph eraseVertices(Set vs) const;
};
template<typename Set>
std::ostream &operator<<(std::ostream &out, const DSBGraph<Set> &pw);

typedef DSBGraph<UnordSet> BaseDSBG;
typedef DSBGraph<OrdSet> CanonDSBG;

} // namespace LIB

}  // namespace SBG

#endif
