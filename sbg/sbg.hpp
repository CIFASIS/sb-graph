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

#include "sbg/af_pwmap.hpp"
#include "util/debug.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Undirected SBG --------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

struct SBGraph {
  private:
  const PWMapAF &fact_;

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

  SBGraph(const PWMapAF &fact);
  SBGraph(const PWMapAF &fact, const Set &V, const PWMap &Vmap
    , const PWMap &map1, const PWMap &map2
    , const PWMap &Emap, const PWMap &subEmap);

  SBGraph addSV(const Set &vertices) const;
  SBGraph addSE(const PWMap &pw1, const PWMap &pw2) const;
  SBGraph copy(unsigned int times) const;
};
std::ostream &operator<<(std::ostream &out, const SBGraph &g);

// Directed SBG ----------------------------------------------------------------

struct DSBGraph {
  private:
  const PWMapAF &fact_;

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

  DSBGraph(const PWMapAF &fact);
  DSBGraph(const PWMapAF &fact, const Set &V, const PWMap &Vmap
   , const PWMap &mapB, const PWMap &mapD
   , const PWMap &Emap, const PWMap &subEmap);

  DSBGraph addSV(const Set &vertices) const;
  DSBGraph addSE(const PWMap &pw1, const PWMap &pw2) const;

  DSBGraph eraseVertices(const Set &vs) const;
};
std::ostream &operator<<(std::ostream &out, const DSBGraph &dg);

} // namespace LIB

}  // namespace SBG

#endif
