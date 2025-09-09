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

#include "algorithms/cutvertex/maxdeg_cv.hpp"
#include "algorithms/scc/scc.hpp"
#include "util/logger.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Maximum Degree Vertex Cut Set Algorithm -------------------------------------
////////////////////////////////////////////////////////////////////////////////

MaxDegCutVertex::MaxDegCutVertex(const PWMapAF& pw_fact
  , const SCCFact& scc_fact)
  : CVStrategy(pw_fact, scc_fact) {}

PWMap MaxDegCutVertex::getDegMap(const DSBG& dsbg) const
{
  Set V = dsbg.V();
  PWMap mapB = dsbg.mapB(), mapD = dsbg.mapD();

  auto dims = V.arity();
  MD_NAT zero(dims, 0), one(dims, 1);
  PWMap dmap = pw_fact_.createPWMap(pw_fact_.createMap(V, Exp(zero))); 

  for (const Map& SE : dsbg.subEmap()) {
    Set dom = SE.dom();
    MD_NAT n(dims, dom.cardinal());
    Set vsB = mapB.image(dom), vsD = mapD.image(dom);
    if (vsB.cardinal() == 1) {
      PWMap ith = dmap.restrict(vsD).offsetImage(n);
      dmap = ith.combine(dmap);
      ith = dmap.restrict(vsB).offsetImage(one);
      dmap = ith.combine(dmap);
    }
    else if (vsD.cardinal() == 1) {
      PWMap ith = dmap.restrict(vsB).offsetImage(n);
      dmap = ith.combine(dmap);
      ith = dmap.restrict(vsD).offsetImage(one);
      dmap = ith.combine(dmap);
    }
    else {
      PWMap ith = dmap.restrict(vsB.cup(vsD)).offsetImage(one);
      dmap = ith.combine(dmap);
    }
  }

  return dmap;
}

Set MaxDegCutVertex::calculate(const DSBG& dsbg) const
{
  auto start = std::chrono::high_resolution_clock::now();

  DSBG dg = dsbg;
  PWMap Vmap = dg.Vmap(), mapB = dg.mapB(), mapD = dg.mapD();
  PWMap rmap = scc_fact_.createSCCAlgorithm(pw_fact_).calculate(dg).rmap();
  Set newD = pw_fact_.createSet(), oldD = newD, visitedV = newD, V = dg.V();

  Util::DEBUG_LOG << "initial dg vertex cut set:\n" << dg << "\n";

  // Degree map
  PWMap dmap = getDegMap(dg);
  Util::DEBUG_LOG << "initial dmap: " << dmap << "\n\n";

  while (rmap.dom() != rmap.image()) {
    oldD = newD;
 
    MD_NAT aux = dmap.image().maxElem();
    MD_NAT vmax = dmap.preImage(pw_fact_.createSet(aux)).minElem();
    Set vmaxSV = Vmap.image(pw_fact_.createSet(vmax));
    newD = newD.cup(pw_fact_.createSet(vmax));

    if (!visitedV.intersection(vmaxSV).isEmpty()) {
      MD_NAT vmaxD = newD.intersection(Vmap.preImage(vmaxSV)).minElem();

      SetPiece mdi;
      for (unsigned int j = 0; j < vmax.arity(); ++j) {
        if (vmax[j] < vmaxD[j]) {
          NAT st = vmaxD[j] - vmax[j];
          NAT beg = vmax[j] % st;
          mdi.emplaceBack(Interval(beg, st, vmax[j]));
        }

        else
          mdi.emplaceBack(Interval(vmaxD[j], vmax[j] - vmaxD[j], Inf));    
      }

      Util::DEBUG_LOG << "vmax: " << vmax << "\n"; 
      Util::DEBUG_LOG << "vmaxD: " << vmaxD << "\n"; 
      Util::DEBUG_LOG << "mdi: " << mdi << "\n\n";

      Set mdi_set = pw_fact_.createSet(mdi);
      newD = newD.cup(mdi_set.intersection(Vmap.preImage(vmaxSV)));
    }

    // Update graph info erasing newD vertices
    visitedV = visitedV.cup(vmaxSV);
    dg = dg.eraseVertices(newD);
    Vmap = dg.Vmap();
    mapB = dg.mapB(); 
    mapD = dg.mapD(); 

    // Update degree map
    dmap = getDegMap(dg);

    // Resulting SCC from induced graph
    rmap = scc_fact_.createSCCAlgorithm(pw_fact_).calculate(dg).rmap();

    Util::DEBUG_LOG << "oldD: " << oldD << "\n";
    Util::DEBUG_LOG << "newD: " << newD << "\n";
    Util::DEBUG_LOG << "resulting graph:\n" << dg << "\n";
    Util::DEBUG_LOG << "new degree map: " << dmap << "\n";
    Util::DEBUG_LOG << "new rmap: " << rmap << "\n\n";
  }
  auto end = std::chrono::high_resolution_clock::now();

  auto total = std::chrono::duration_cast<std::chrono::microseconds>(
    end - start
  );
  Util::SBG_LOG << "Total vertex cut set exec time: " << total.count() << " [μs]\n\n"; 

  return newD.compact();
}

} // namespace LIB

} // namespace SBG
