/** @file bipartite_sbg.hpp

 @brief <b>Bipartite Set-based graph implementation</b>

 A bipartite SBG is a SBG such that two sets X and Y conform a bipartition of
 the vertices, and each edge has one ending in X and the other in Y.

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

#ifndef SBGRAPH_SBG_BIPARTITE_SBG_HPP_
#define SBGRAPH_SBG_BIPARTITE_SBG_HPP_

#include "sbg/pw_map.hpp"
#include "sbg/set.hpp"

#include <iosfwd>

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Bipartite SBG ---------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

class BipartiteSBG {
public:
  /**
   * @brief Empyt Bipartite SBG.
   */
  BipartiteSBG();

  /**
   * @brief Bipartite SBG constructor that copies arguments to construct member
   * variables. A set of edges E is not needed, as it will be obtained from the
   * domain of map1_ and map2_.
   * Preconditions:
   *   - \p V = dom(\p Vmap).
   *   - dom(\p map1) = dom(\p map2) = dom(\p Emap).
   *   - \p X, \p Y is a bipartition of \p V.
   */
  BipartiteSBG(const Set& V, const PWMap& Vmap
    , const PWMap& map1, const PWMap& map2
    , const PWMap& Emap 
    , const Set& X, const Set& Y);

  const Set& V() const;
  const PWMap& Vmap() const;
  const Set& E() const;
  const PWMap& map1() const;
  const PWMap& map2() const;
  const PWMap& Emap() const;
  const Set& X() const;
  const Set& Y() const;

  /**
   * @brief Adds a set-vertex to the bipartite SBG. The elements of the new
   * set-vertex will be all those of \p X and \p Y. Then, elements of \p X will
   * be added to _X, and the same goes for \p Y and _Y.
   * Precondition: \p X ∩ \p Y = {}.
   */
  void addSetVertex(const Set& X, const Set& Y);

  /**
   * @brief Adds a set-edge to the bipartite SBG.
   * Preconditions: dom(\p pw1) = dom(\p pw2).
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
  Set _X; ///< "Left" vertices by convention of the bipartite SBG
  Set _Y; ///< "Right" vertices by convention of the bipartite SBG
};

std::ostream& operator<<(std::ostream& out, const BipartiteSBG& g);

// Template definitions --------------------------------------------------------

template<typename FuncT>
inline void BipartiteSBG::foreachSetVertex(FuncT&& f) const
{
  Set remaining = _Vmap.image();
  while (!remaining.isEmpty()) {
    const IntTuple& x = remaining.minElem();
    f(x);
    remaining = remaining.difference(Set{x});
  }
}

template<typename FuncT>
inline void BipartiteSBG::foreachSetEdge(FuncT&& f) const
{
  Set remaining = _Emap.image();
  while (!remaining.isEmpty()) {
    const IntTuple& x = remaining.minElem();
    f(x);
    remaining = remaining.difference(Set{x});
  }
}

// Extra operations ------------------------------------------------------------

BipartiteSBG copy(unsigned int copies, BipartiteSBG sbg);

} // namespace LIB

}  // namespace SBG

#endif // SBGRAPH_SBG_BIPARTITE_SBG_HPP_
