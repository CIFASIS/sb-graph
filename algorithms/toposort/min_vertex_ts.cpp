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

#include "algorithms/toposort/min_vertex_ts.hpp"
#include "util/logger.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Minimum Vertex Topological Sort Algorithm -----------------------------------
////////////////////////////////////////////////////////////////////////////////

MinVertexTopoSort::MinVertexTopoSort() {}

Exp calculateExp(const MD_NAT& from, const MD_NAT&  to)
{
  Exp res;

  RATIONAL one(1, 1);
  for (unsigned int j = 0; j < from.arity(); ++j) { 
    RATIONAL r_from(from[j]), r_to(to[j]);
    res.emplaceBack(LExp(1, r_to - r_from));
  }

  return res;
}

PWMap MinVertexTopoSort::calculate(const DSBG& dsbg) const
{
  Util::DEBUG_LOG << "Topological sort dsbg:\n" << dsbg << "\n\n";

  auto begin = std::chrono::high_resolution_clock::now();
  PWMap mapB = dsbg.mapB(), mapD = dsbg.mapD(), Vmap = dsbg.Vmap();
  PWMap smap = PW_FACT.createPWMap();
  Set U = dsbg.V(), Nd = U.difference(mapB.image());
  if (!Nd.isEmpty()) {
    MD_NAT vsucc = Nd.minElem();
    Set SV = SET_FACT.createSet(), E = dsbg.E();
    do {
      Set vsucc_set = SET_FACT.createSet(SetPiece(vsucc));
      Set Nd_vsucc = Nd.intersection(Vmap.preImage(Vmap.image(vsucc_set)));
      MD_NAT v = Nd.minElem();
      if (!Nd_vsucc.isEmpty())
        v = Nd_vsucc.minElem();
      Set d = SET_FACT.createSet(v);
      Exp e = calculateExp(v, vsucc);
      vsucc = v;

      Set SVd = Vmap.image(d);
      bool cond = SVd.intersection(SV).isEmpty();
      if (!cond) {
        Set dvs = Vmap.preImage(SVd);
        for (const Map& map : smap.restrict(dvs)) {
          if (e == map.exp()) {
            d = dvs.difference(smap.dom());
            break;
          }
        }
      }
      smap.emplaceBack(MAP_FACT.createMap(d, e));
      
      Set Nsucc = U.difference(smap.dom());
      Set S = smap.dom().difference(smap.preImage(Nsucc));

      E = E.difference(mapD.preImage(S));
      mapB = mapB.restrict(E);
      mapD = mapD.restrict(E);

      U = U.difference(S);
      Nd = U.difference(mapB.image());
      SV = SV.cup(Vmap.image(d));
      if (S == smap.dom()) {
        Set start = smap.dom().difference(smap.image());
        if (!start.isEmpty())
          vsucc = start.minElem(); 
      }

      Util::DEBUG_LOG << "S: " << S << "\n";
      Util::DEBUG_LOG << "U: " << U << "\n";
      Util::DEBUG_LOG << "E: " << E << "\n";
      Util::DEBUG_LOG << "Nd: " << Nd << "\n";
      Util::DEBUG_LOG << "smap: " << smap << "\n\n";
    } while (!U.isEmpty());
  }
  auto end = std::chrono::high_resolution_clock::now();

  auto total = std::chrono::duration_cast<std::chrono::microseconds>(
    end - begin
  );
  Util::SBG_LOG << "Total topological sort exec time: " << total.count() << " [μs]\n\n"; 

  Util::DEBUG_LOG << "Topological sort result:\n" << smap.compact() << "\n\n";

  return smap.compact();
}

} // namespace LIB

} // namespace SBG
