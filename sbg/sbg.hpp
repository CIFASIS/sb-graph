/** @file sbg.hpp

 @brief <b>Set-based graph implementation</b>

 Given a graph G, if there is a biunivocal assignment of naturals for elements
 in V(G) and E(G), an associated SBG is a tuple composed by:
 - A Set V_ describing V(G).
 - A Set E_ listing elements of E(G).
 - A PWMap map1_ mapping elements of E_ to V_ (one of the endings of each edge).
 - A PWMap map2_ mapping elements of E_ to V_ (the other ending of each edge). \n 
 where map1_ and map_2 share the same domain. These elements all together keep
 the same information as G. Additionally, the SBG keeps:
 - A PWMap Vmap_ mapping elements in V_ to some constant value. Vertices that
   share the same image conform a *Set-Vertex*.
 - A PWMap Emap_ mapping elements in E_ to some constant value. Edges that
   share the same image conform a *Set-Edge*.
 - A PWMap subEmap_ mapping elements in E_ to some constant value. Edges that
   share the same image conform a *Subset-Edge*.\n 
 These components are added to keep track of repetitve structures in G. Note
 that a Set-Edge might be composed by several Subset-Edges.

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

#include "sbg/pwmap_fact.hpp"
#include "util/debug.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Undirected SBG --------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

class SBG {
  public:
  // Vertex definitions
  member_class(Set, V);
  member_class(PWMap, Vmap);

  // Edge definitions
  member_class(Set, E);
  member_class(PWMap, map1);
  member_class(PWMap, map2);
  member_class(PWMap, Emap);
  member_class(PWMap, subEmap);

  /**
   * @brief Empty SBG constructor.
   */
  SBG();

  /**
   * @brief SBG constructor that copies arguments to construct member variables.
   * A set of edges E is not needed, as it will be obtained from the domain of
   * map1_ and map2_.
   */
  SBG(const Set &V, const PWMap &Vmap
    , const PWMap &map1, const PWMap &map2
    , const PWMap &Emap, const PWMap &subEmap);

  SBG &operator=(const SBG &other);

  /**
   * @brief Adds a new set-vertex composed by \p vertices. \n
   * Precondition: V_.intersection(vertices) = {}
   */
  SBG addSV(const Set &vertices) const;

  /**
   * @brief Adds a new set-edge described by \p pw1 and \p pw2. \n 
   * Precondition: dom(pw1) = dom(pw2) and
   * E_.intersection(pw1.dom()) = {} and E_.intersection(pw2.dom()) = {} 
   */
  SBG addSE(const PWMap &pw1, const PWMap &pw2) const;

  /**
   * @brief Returns a new SBG composed by \p times copies of the original SBG,
   * where each copy is isomorphic to the argument.
   */
  SBG copy(unsigned int times) const;
};
std::ostream &operator<<(std::ostream &out, const SBG &g);

////////////////////////////////////////////////////////////////////////////////
// Directed SBG ----------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

class DSBG {
  public:
  // Vertex definitions
  member_class(Set, V);
  member_class(PWMap, Vmap);

  // Edge definitions
  member_class(Set, E);
  member_class(PWMap, mapB);
  member_class(PWMap, mapD);
  member_class(PWMap, Emap);
  member_class(PWMap, subEmap);

  /**
   * @brief Empty SBG constructor.
   */
  DSBG();

  /**
   * @brief SBG constructor that copies arguments to construct member variables.
   * A set of edges E is not needed, as it will be obtained from the domain of
   * map1_ and map2_.
   */
  DSBG(const Set &V, const PWMap &Vmap
   , const PWMap &mapB, const PWMap &mapD
   , const PWMap &Emap, const PWMap &subEmap);

  DSBG &operator=(const DSBG &other);

  /**
   * @brief Adds a new set-vertex composed by \p vertices.
   * Precondition: V_.intersection(vertices) = {}
   */
  DSBG addSV(const Set &vertices) const;

  /**
   * @brief Adds a new set-edge described by \p pw1 and \p pw2.
   * Precondition: dom(pw1) = dom(pw2) and
   * E_.intersection(pw1.dom()) = {} and E_.intersection(pw2.dom()) = {} 
   */
  DSBG addSE(const PWMap &pw1, const PWMap &pw2) const;

  /**
   * @brief Erase vertices \p vs from the DSBG, together with associated edges
   * with \p vs.
   */
  DSBG eraseVertices(const Set &vs) const;
};
std::ostream &operator<<(std::ostream &out, const DSBG &dg);

} // namespace LIB

}  // namespace SBG

#endif
