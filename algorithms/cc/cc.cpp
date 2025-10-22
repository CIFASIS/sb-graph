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

#include <chrono>

#include "algorithms/cc/cc.hpp"
#include "util/logger.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Connected components --------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

PWMap connectedComponents(SBG g)
{
  auto begin = std::chrono::high_resolution_clock::now();
  const PWMapAF &fact_ = g.fact();

  if (!g.V().isEmpty()) {
    PWMap rmap = fact_.createPWMap(g.V()), old_rmap = fact_.createPWMap();

    if (g.E().isEmpty())
      return rmap;

    do {
      old_rmap = rmap;

      PWMap ermap1 = rmap.composition(g.map1());
      PWMap ermap2 = rmap.composition(g.map2());

      PWMap rmap1 = ermap1.minAdjMap(ermap2);
      PWMap rmap2 = ermap2.minAdjMap(ermap1);
      rmap1 = rmap1.combine(rmap);
      rmap2 = rmap2.combine(rmap);

      PWMap aux_rmap = rmap1.minMap(rmap2);
      rmap = rmap.minMap(aux_rmap);

      if (!(rmap == old_rmap)) {
        rmap = aux_rmap;
        rmap = rmap.mapInf();
      }
    } while (rmap != old_rmap); 

    return rmap;
  }
  auto end = std::chrono::high_resolution_clock::now();
  auto total = std::chrono::duration_cast<std::chrono::microseconds>(
    end - begin);
  Util::SBG_LOG << "Total CC exec time: " << total.count() << "\n";

  return fact_.createPWMap();
}

} // namespace LIB

} // namespace SBG
