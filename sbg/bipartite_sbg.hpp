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

#ifndef SBG_BIPARTITE_SBG_HPP
#define SBG_BIPARTITE_SBG_HPP

#include "sbg/sbg.hpp"
#include "util/debug.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Bipartite SBG ---------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

class BipartiteSBG : public SBG {
  public:
  /**
   * @brief Empyt Bipartite SBG.
   */
  BipartiteSBG();

  /**
   * @brief Bipartite SBG constructor that copies arguments to construct member
   * variables. A set of edges E is not needed, as it will be obtained from the
   * domain of map1_ and map2_.
   */
  BipartiteSBG(const Set& V, const PWMap& Vmap
    , const PWMap& map1, const PWMap& map2
    , const PWMap& Emap, const PWMap& subEmap
    , const Set& X, const Set& Y);

  const Set& X() const;
  const Set& Y() const;

  private:
  Set _X;
  Set _Y;
};
std::ostream& operator<<(std::ostream& out, const BipartiteSBG& g);

} // namespace LIB

}  // namespace SBG

#endif
