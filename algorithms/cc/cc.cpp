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

#include "algorithms/cc/cc.hpp"
#include "sbg/set.hpp"
#include "util/logger.hpp"
#include "util/time_profiler.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Connected components --------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

PWMap connectedComponents(const SBG& sbg)
{
  Util::Internal::TimeProfiler profiler{"Total CC execution time"};

  Set V = sbg.V();
  if (!V.isEmpty()) {
    PWMap rmap{V};
    PWMap old_rmap;

    if (sbg.E().isEmpty()) {
      return rmap;
    }

    PWMap map1 = sbg.map1();
    PWMap map2 = sbg.map2();
    do {
      old_rmap = rmap;

      PWMap ermap1 = rmap.composition(map1);
      PWMap ermap2 = rmap.composition(map2);

      PWMap rmap1 = ermap1.minAdj(ermap2);
      PWMap rmap2 = ermap2.minAdj(ermap1);
      rmap1 = std::move(rmap1).combine(rmap);
      rmap2 = std::move(rmap2).combine(rmap);

      PWMap aux_rmap = rmap1.min(rmap2);
      rmap = rmap.min(aux_rmap);

      if (!(rmap == old_rmap)) {
        rmap = aux_rmap;
        rmap = rmap.mapInf();
      }
    } while (rmap != old_rmap); 

    rmap.compact();
    return rmap;
  }

  return PWMap{};
}

} // namespace LIB

} // namespace SBG
