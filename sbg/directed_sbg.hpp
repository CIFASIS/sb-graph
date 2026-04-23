/** @file directed_sbg.hpp

 @brief <b>Directed Set-based graph implementation</b>

 Given a graph G, if there is a biunivocal assignment of naturals for elements
 in V(G) and E(G), an associated directed SBG is a tuple composed by:
 - A Set V_ describing V(G).
 - A Set E_ listing elements of E(G).
 - A PWMap mapB_ mapping elements of E_ to V_ (start of each edge).
 - A PWMap mapD_ mapping elements of E_ to V_ (ending of each edge). \n 
 where mapB_ and mapD_ share the same domain. These elements all together keep
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

#ifndef SBGRAPH_SBG_DIRECTED_SBG_HPP_
#define SBGRAPH_SBG_DIRECTED_SBG_HPP_

#include "sbg/set.hpp"
#include "sbg/set_fact.hpp"
#include "sbg/pw_map.hpp"

#include <iosfwd>

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Directed SBG ----------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

class DirectedSBG {
public:
  /**
   * @brief Empty SBG constructor.
   */
  DirectedSBG();

  /**
   * @brief SBG constructor that copies arguments to construct member variables.
   * A set of edges E is not needed, as it will be obtained from the domain of
   * map1_ and map2_.
   */
  DirectedSBG(const Set& V, const PWMap& Vmap
   , const PWMap& mapB, const PWMap& mapD
   , const PWMap& Emap);

  const Set& V() const;
  const PWMap& Vmap() const;
  const Set& E() const;
  const PWMap& mapB() const;
  const PWMap& mapD() const;
  const PWMap& Emap() const;

  /**
   * @brief Adds a new set-vertex composed by \p vertices.
   * Precondition: V_.intersection(vertices) = {}
   */
  void addSetVertex(const Set& vertices);

  /**
   * @brief Adds a new set-edge described by \p pw1 and \p pw2.
   * Precondition: dom(pw1) = dom(pw2) and
   * E_.intersection(pw1.dom()) = {} and E_.intersection(pw2.dom()) = {} 
   */
  void addSetEdge(const PWMap& pw1, const PWMap& pw2);

  /**
   * @brief Erase vertices \p vs from the DirectedSBG, together with adjacent
   * edges of \p vs.
   */
  void eraseVertices(const Set& V);

  void eraseEdges(const Set& E);

  template<typename FuncT>
  void foreachSetVertex(FuncT&& f) const;

  template<typename FuncT>
  void foreachSetEdge(FuncT&& f) const;

private:
  Set _V; ///< Vertices definitions
  PWMap _Vmap;
  Set _E; ///< Edges definitions
  PWMap _mapB;
  PWMap _mapD;
  PWMap _Emap;
};

std::ostream& operator<<(std::ostream& out, const DirectedSBG& dg);

// Template definitions --------------------------------------------------------

template<typename FuncT>
inline void DirectedSBG::foreachSetVertex(FuncT&& f) const
{
  Set remaining = _Vmap.image();
  while (!remaining.isEmpty()) {
    const MD_NAT& x = remaining.minElem();
    f(x);
    remaining = remaining.difference(SET_FACT.createSet(x));
  }
}

template<typename FuncT>
inline void DirectedSBG::foreachSetEdge(FuncT&& f) const
{
  Set remaining = _Emap.image();
  while (!remaining.isEmpty()) {
    const MD_NAT& x = remaining.minElem();
    f(x);
    remaining = remaining.difference(SET_FACT.createSet(x));
  }
}

} // namespace LIB

}  // namespace SBG

#endif // SBGRAPH_SBG_DIRECTED_SBG_HPP_
