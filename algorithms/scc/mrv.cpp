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

#include "algorithms/scc/mrv.hpp"
#include "util/logger.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// MRV Algorithm ---------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

template<class MRVImpl>
MRV<MRVImpl>::MRV(const PWMapAF& fact) : fact_(fact) {}

template<class MRVImpl>
PWMap MRV<MRVImpl>::calculate(const DSBG& dsbg)
{
  return static_cast<MRVImpl*>(this)->impl(dsbg);
}

////////////////////////////////////////////////////////////////////////////////
// Path Finder BFS Implementation ----------------------------------------------
////////////////////////////////////////////////////////////////////////////////

MinAdjMRV::MinAdjMRV(const PWMapAF& fact) : MRV<MinAdjMRV>(fact)
  , dsbg_(DSBG(fact)), Vc_(fact.createSet()), VR_(fact.createSet()) {}

PWMap MinAdjMRV::recursivePaths(const PWMap& rmap) const
{
  PWMap recursion_rmap = fact_.createPWMap();

  PWMap mapB = dsbg_.mapB();
  PWMap mapD = dsbg_.mapD();

  // Vertices that reach the shared representative
  Set repV = rmap.preImage(rmap.image(VR_));
  Set end = VR_.difference(Vc_);

  // Edges with both endings in VR (path to a minimum rep)
  Set ERB = mapB.preImage(repV);
  Set ERD = mapD.preImage(repV);
  Set ER = ERB.intersection(ERD);
  if (!end.isEmpty()&&  !ER.isEmpty()) {
    // Distance map
    PWMap dmap = fact_.createPWMap();
    Set ith = end;
    NAT dist = 0;
    unsigned int arity = VR_.arity();
    // Calculate distance for vertices in same_rep that reach reps
    for (; dmap.dom().intersection(Vc_.intersection(VR_)).isEmpty();) {
      Set dom = ith.difference(dmap.dom());
      Exp exp(MD_NAT(arity, dist));
      dmap.emplaceBack(fact_.createMap(dom, exp));
      // Update ith to vertices that have outgoing edges entering ith
      ith = mapB.image(mapD.preImage(ith));
      ++dist;
    }
    PWMap dmapB = dmap.composition(mapB), dmapD = dmap.composition(mapD);
    // Get edges where the end is closer to the rep than the beginning
    Set positive = fact_.createSet(SetPiece(arity, Interval(1, 1, Inf)));
    Set not_cycle_edges = (dmapB - dmapD).preImage(positive);
    ER = ER.intersection(not_cycle_edges);

    // Extend to subset-edge
    PWMap subEmap = dsbg_.subEmap();
    Set ER_plus = subEmap.preImage(subEmap.image(ER));
    // Calculate a succesor
    PWMap auxB = mapB.restrict(ER_plus);
    PWMap auxD = mapD.restrict(ER_plus);
    PWMap smap_plus = rmap.restrict(VR_);
    smap_plus = smap_plus.combine(auxB.minAdjMap(auxD));

    // Update rmap for recursion, and leave the rest unchanged
    recursion_rmap = smap_plus.combine(recursion_rmap).compact();

    Util::DEBUG_LOG << "VR: " << VR_ << "\n";
    Util::DEBUG_LOG << "repV: " << repV << "\n";
    Util::DEBUG_LOG << "ER: " << ER << "\n";
    Util::DEBUG_LOG << "dmap: " << dmap << "\n";
    Util::DEBUG_LOG << "not_cycle_edges: " << not_cycle_edges << "\n";
    Util::DEBUG_LOG << "ER_plus: " << ER_plus << "\n";
    Util::DEBUG_LOG << "smap_plus: " << smap_plus << "\n";
    Util::DEBUG_LOG << "recursion_rmap: " << recursion_rmap << "\n\n";
  }

  return recursion_rmap;
}

PWMap MinAdjMRV::impl(const DSBG& dsbg)
{
  Util::DEBUG_LOG << "MinAdjMRV:\n" << dsbg << "\n\n";

  dsbg_ = dsbg;

  Set V = dsbg_.V();
  Set E = dsbg_.E();
  PWMap mapB = dsbg_.mapB();
  PWMap mapD = dsbg_.mapD();
  PWMap subEmap = dsbg_.subEmap();

  Util::DEBUG_LOG << "V: " << V << "\n";
  Util::DEBUG_LOG << "E: " << E << "\n\n";

  if (!V.isEmpty()) {
    PWMap rmap = fact_.createPWMap(V);
    PWMap old_rmap = fact_.createPWMap();

    if (E.isEmpty())
      return rmap;

    do {
      old_rmap = rmap;

      PWMap ermapD = rmap.composition(mapD);

      PWMap new_rmap = mapB.minAdjMap(ermapD);
      rmap = rmap.minMap(new_rmap).combine(rmap);
      Util::DEBUG_LOG << "rmap before rec: " << rmap << "\n\n";

      PWMap rec_rmap = fact_.createPWMap();
      Vc_ = V.difference(old_rmap.equalImage(rmap));
      if (!Vc_.isEmpty()) {
        // If the mrv is in the same SV, the algorithm would detect a false
        // recursion, i.e. if we have a cycle 1 -> 2 -> ... -> 10 -> 1,
        // where SV = [1:10], then it detects a recursion when mrv(10) = 1
        // (false recursion). So we take self mrvs out.
        Set represented = V.difference(rmap.fixedPoints());
        for (const Map& subv : dsbg.Vmap()) {
          Set vs = subv.dom();
          if (!vs.intersection(Vc_).isEmpty()) {
            // Vertices in the set-vertex that share its rep with other vertex
            // in the set-vertex
            VR_ = rmap.restrict(vs.intersection(represented)).sharedImage();
            // There is a recursive vertex that changed its rep in the last step
            // (to avoid computing again an already found recursion)
            if (!VR_.intersection(Vc_).isEmpty()) {
              rec_rmap = recursivePaths(rmap);
            }
          }
        }

        PWMap rmap_plus = rec_rmap.combine(rmap);
        rmap_plus = rmap_plus.mapInf();
        rmap = rmap.minMap(rmap_plus).compact();

        Util::DEBUG_LOG << "rmap after rec: " << rmap << "\n\n";
      }
    } while (!Vc_.isEmpty()); 

    return rmap;
  }

  return fact_.createPWMap();
}

template class MRV<MinAdjMRV>;

} // namespace LIB

} // namespace SBG
