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

#include "algorithms/scc/minadj_mrv.hpp"
#include "util/logger.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Minimum Adjacent MRV Implementation -----------------------------------------
////////////////////////////////////////////////////////////////////////////////

MinAdjMRV::MinAdjMRV() {}

PWMap MinAdjMRV::calculate(const DSBG& dsbg)
{
  Set V = dsbg.V(), E = dsbg.E();
  PWMap mapB = dsbg.mapB(), mapD = dsbg.mapD(), subEmap = dsbg.subEmap();
  if (!V.isEmpty()) {
    unsigned int copies = V.arity();
    PWMap rmap = PW_FACT.createPWMap(V), old_rmap = PW_FACT.createPWMap();

    if (E.isEmpty())
      return rmap;

    Set Vc = SET_FACT.createSet();
    do {
      old_rmap = rmap;

      PWMap ermapD = rmap.composition(mapD);

      PWMap new_rmap = mapB.minAdjMap(ermapD);
      rmap = rmap.minMap(new_rmap).combine(rmap);
      Util::DEBUG_LOG << "rmap before rec: " << rmap << "\n\n";

      PWMap rec_rmap = PW_FACT.createPWMap();
      Vc = V.difference(old_rmap.equalImage(rmap));
      if (!Vc.isEmpty()) {
        // If the mrv is in the same SV, the algorithm would detect a false
        // recursion, i.e. if we have a cycle 1 -> 2 -> ... -> 10 -> 1,
        // where SV = [1:10], then it detects a recursion when mrv(10)
        // becomes "1" (false recursion). So we take self mrvs out.
        Set other_rep = rmap.dom().difference(rmap.fixedPoints());
        for (const Map &subv : dsbg.Vmap()) {
          Set vs = subv.dom();
          if (!vs.intersection(Vc).isEmpty()) {
            // Vertices in the set-vertex that share its rep with other vertex
            // in the set-vertex
            Set VR = rmap.restrict(vs.intersection(other_rep)).sharedImage();
            // There is a recursive vertex that changed its rep in the last step
            // (to avoid computing again an already found recursion)
            if (!VR.intersection(Vc).isEmpty()) {
              // Vertices that reach the shared representative
              Set repV = rmap.preImage(rmap.image(VR));
              Set end = VR.difference(Vc);

              // Edges with both endings in VR (path to a minimum rep)
              Set ERB = mapB.preImage(repV), ERD = mapD.preImage(repV);
              Set ER = ERB.intersection(ERD);
              if (!end.isEmpty() && !ER.isEmpty()) {
                // Distance map
                PWMap dmap = PW_FACT.createPWMap();
                Set ith = end;
                NAT dist = 0;
                // Calculate distance for vertices in same_rep that reach reps
                for (; dmap.dom().intersection(Vc.intersection(VR)).isEmpty();) {
                  Set dom = ith.difference(dmap.dom());
                  Exp exp(MD_NAT(copies, dist));
                  dmap.emplaceBack(Map(dom, exp));
                  // Update ith to vertices that have outgoing edges entering ith
                  ith = mapB.image(mapD.preImage(ith));
                  ++dist;
                }
                PWMap dmapB = dmap.composition(mapB), dmapD = dmap.composition(mapD);
                // Get edges where the end is closer to the rep than the beginning
                Set not_cycle_edges = dmapD.lessImage(dmapB);
                ER = ER.intersection(not_cycle_edges);

                // Extend to subset-edge
                Set ER_plus = subEmap.preImage(subEmap.image(ER));
                // Calculate a succesor
                PWMap auxB = mapB.restrict(ER_plus), auxD = mapD.restrict(ER_plus);
                PWMap smap_plus = rmap.restrict(VR);
                smap_plus = smap_plus.combine(auxB.minAdjMap(auxD));

                // Update rmap for recursion, and leave the rest unchanged
                rec_rmap = smap_plus.combine(rec_rmap).compact();

                Util::DEBUG_LOG << "VR: " << VR << "\n";
                Util::DEBUG_LOG << "repV: " << repV << "\n";
                Util::DEBUG_LOG << "ER: " << ER << "\n";
                Util::DEBUG_LOG << "dmap: " << dmap << "\n";
                Util::DEBUG_LOG << "not_cycle_edges: " << not_cycle_edges << "\n";
                Util::DEBUG_LOG << "ER_plus: " << ER_plus << "\n";
                Util::DEBUG_LOG << "smap_plus: " << smap_plus << "\n";
                Util::DEBUG_LOG << "rec_rmap: " << rec_rmap << "\n\n";
              }
            }
          }
        }
        PWMap rmap_plus = rec_rmap.combine(rmap);
        rmap_plus = rmap_plus.mapInf();
        rmap = rmap.minMap(rmap_plus).compact();

        Util::DEBUG_LOG << "rmap after rec: " << rmap << "\n\n";
      }
    } while (!Vc.isEmpty()); 

    return rmap;
  }

  return PW_FACT.createPWMap();
}

} // namespace LIB

} // namespace SBG
