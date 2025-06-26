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

#include "algorithms/cutvertex/cut_vertex.hpp"
#include "algorithms/scc/scc.hpp"
#include "util/logger.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Cut-set algorithm -----------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

CutVertex::CutVertex(const DSBG &dsbg, bool debug) 
  : fact_(dsbg.fact()), dsbg_(dsbg), debug_(debug) {}

member_imp(CutVertex, DSBG, dsbg);
member_imp(CutVertex, bool, debug);

PWMap CutVertex::getDegMap(const DSBG &dsbg)
{
  Set V = dsbg.V();
  PWMap mapB = dsbg.mapB(), mapD = dsbg.mapD();

  auto dims = V.arity();
  MD_NAT zero(dims, 0), one(dims, 1);
  PWMap dmap = fact_.createPWMap(fact_.createMap(V, Exp(zero))); 

  for (const Map &SE : dsbg.subEmap()) {
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

Set CutVertex::calculate()
{
  auto begin = std::chrono::high_resolution_clock::now();
  DSBG dg = dsbg();
  PWMap Vmap = dg.Vmap(), mapB = dg.mapB(), mapD = dg.mapD();
  PWMap rmap = SCC(dg, debug()).calculate();
  Set newD = fact_.createSet(), oldD = newD, visitedV = newD, V = dg.V();
  if (debug())
    Util::SBG_LOG << "initial dg vertex cut set:\n" << dg << "\n";

  PWMap dmap = getDegMap(dg);
  // Degree map
  if (debug())
    Util::SBG_LOG << "initial dmap: " << dmap << "\n\n";

  while (rmap.dom() != rmap.image()) {
    oldD = newD;
 
    MD_NAT aux = dmap.image().maxElem();
    MD_NAT vmax = dmap.preImage(fact_.createSet(aux)).minElem();
    Set vmaxSV = Vmap.image(fact_.createSet(vmax));
    newD = newD.cup(fact_.createSet(vmax));

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
      if (debug()) {
        Util::SBG_LOG << "vmax: " << vmax << "\n"; 
        Util::SBG_LOG << "vmaxD: " << vmaxD << "\n"; 
        Util::SBG_LOG << "mdi: " << mdi << "\n\n";
      }
      Set mdi_set = fact_.createSet(mdi);
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
    rmap = SCC(dg, debug()).calculate();

    if (debug()) {
      Util::SBG_LOG << "oldD: " << oldD << "\n";
      Util::SBG_LOG << "newD: " << newD << "\n";
      Util::SBG_LOG << "resulting graph:\n" << dg << "\n";
      Util::SBG_LOG << "new degree map: " << dmap << "\n";
      Util::SBG_LOG << "new rmap: " << rmap << "\n\n";
    }
  }
  auto end = std::chrono::high_resolution_clock::now();

  auto total = std::chrono::duration_cast<std::chrono::microseconds>(
    end - begin
  );
  Util::SBG_LOG << "Total vertex cut set exec time: " << total.count() << " [μs]\n\n"; 

  return newD.compact();
}

} // namespace LIB

} // namespace SBG
