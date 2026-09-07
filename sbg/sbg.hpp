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

#ifndef SBGRAPH_SBG_SBG_HPP_
#define SBGRAPH_SBG_SBG_HPP_

#include "sbg/pw_map.hpp"
#include "sbg/set.hpp"

#include <iosfwd>

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Undirected SBG --------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

class SBG {
public:
  /**
   * @brief Empty SBG constructor.
   */
  SBG();

  /**
   * @brief SBG constructor that copies arguments to construct member variables.
   * A set of edges E is not needed, as it will be obtained from the domain of
   * map1_ and map2_.
   * Preconditions:
   *   - \p V = dom(\p Vmap)
   *   - dom(\p map1) = dom(\p map2) = dom(\p Emap) = dom(\p subEmap)
   *   - \p map1.image() ⊆ \p V and \p map2.image() ⊆ \p V
   */
  SBG(const Set& V, const PWMap& Vmap
    , const PWMap& map1, const PWMap& map2
    , const PWMap& Emap);

  const Set& V() const;
  const PWMap& Vmap() const;
  const Set& E() const;
  const PWMap& map1() const;
  const PWMap& map2() const;
  const PWMap& Emap() const;

  /**
   * @brief Adds a new set-vertex composed by \p vertices. \n
   * Precondition: V_.intersection(vertices) = {}
   */
  void addSetVertex(const Set& vertices);

  /**
   * @brief Adds a new set-edge described by \p pw1 and \p pw2. \n 
   * Precondition: dom(pw1) = dom(pw2) and
   * E_.intersection(pw1.dom()) = {} and E_.intersection(pw2.dom()) = {} 
   */
  void addSetEdge(const PWMap& pw1, const PWMap& pw2);

  template<typename FuncT>
  void foreachSetVertex(FuncT&& f) const;

  template<typename FuncT>
  void foreachSetEdge(FuncT&& f) const;

private:
  Set _V; ///< Vertex definitions
  PWMap _Vmap;
  Set _E; ///< Edge definitions
  PWMap _map1;
  PWMap _map2;
  PWMap _Emap;
};

std::ostream& operator<<(std::ostream& out, const SBG& g);

// Template definitions --------------------------------------------------------

template<typename FuncT>
inline void SBG::foreachSetVertex(FuncT&& f) const
{
  Set remaining = _Vmap.image();
  while (!remaining.isEmpty()) {
    const IntTuple& x = remaining.minElem();
    f(x);
    remaining = remaining.difference(Set{x});
  }
}

template<typename FuncT>
inline void SBG::foreachSetEdge(FuncT&& f) const
{
  Set remaining = _Emap.image();
  while (!remaining.isEmpty()) {
    const IntTuple& x = remaining.minElem();
    f(x);
    remaining = remaining.difference(Set{x});
  }
}


} // namespace LIB

}  // namespace SBG

#endif // SBGRAPH_SBG_SBG_HPP_
