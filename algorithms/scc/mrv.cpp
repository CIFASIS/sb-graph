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
  , dsbg_(fact), rmap_(fact_.createPWMap()) {}

PWMap MinAdjMRV::recursivePaths(const Set& changedV, const Set& recursiveV) const
{
  PWMap recursion_rmap = fact_.createPWMap();

  PWMap mapB = dsbg_.mapB();
  PWMap mapD = dsbg_.mapD();

  // Vertices that reach the shared representative
  Set repV = rmap_.preImage(rmap_.image(recursiveV));
  Util::DEBUG_LOG << "recursiveV: " << recursiveV << "\n";
  Util::DEBUG_LOG << "repV: " << repV << "\n";

  // Edges with both endings in recursiveV (path to a minimum rep)
  Set ERB = mapB.preImage(repV);
  Set ERD = mapD.preImage(repV);
  Set ER = ERB.intersection(ERD);

  Set end = recursiveV.difference(changedV);
  if (!end.isEmpty()&&  !ER.isEmpty()) {
    // Distance map
    PWMap dmap = fact_.createPWMap();
    Set ith = end;
    NAT dist = 0;
    unsigned int arity = recursiveV.arity();
    // Calculate distance for vertices in same_rep that reach reps
    for (; dmap.dom().intersection(changedV.intersection(recursiveV)).isEmpty();) {
      Set dom = ith.difference(dmap.dom());
      Exp exp(MD_NAT(arity, dist));
      dmap.emplaceBack(fact_.createMap(dom, exp));
      // Update ith to vertices that have outgoing edges entering ith
      ith = mapB.image(mapD.preImage(ith));
      ++dist;
    }
    Util::DEBUG_LOG << "dmap: " << dmap << "\n";
    PWMap dmapB = dmap.composition(mapB);
    PWMap dmapD = dmap.composition(mapD);

    // Get edges where the end is closer to the rep than the beginning
    Set positive = fact_.createSet(SetPiece(arity, Interval(1, 1, Inf)));
    Set not_cycle_edges = (dmapB - dmapD).preImage(positive);
    Util::DEBUG_LOG << "not_cycle_edges: " << not_cycle_edges << "\n";
    ER = ER.intersection(not_cycle_edges);
    Util::DEBUG_LOG << "ER: " << ER << "\n";

    // Extend to subset-edge
    PWMap subEmap = dsbg_.subEmap();
    Set ER_plus = subEmap.preImage(subEmap.image(ER));
    Util::DEBUG_LOG << "ER_plus: " << ER_plus << "\n";

    // Calculate a succesor
    PWMap auxB = mapB.restrict(ER_plus);
    PWMap auxD = mapD.restrict(ER_plus);
    PWMap smap_plus = rmap_.restrict(recursiveV);
    smap_plus = smap_plus.combine(auxB.minAdjMap(auxD));
    Util::DEBUG_LOG << "smap_plus: " << smap_plus << "\n";

    // Update rmap for recursion, and leave the rest unchanged
    recursion_rmap = smap_plus.combine(recursion_rmap).compact();
    Util::DEBUG_LOG << "recursion_rmap: " << recursion_rmap << "\n\n";
  }

  return recursion_rmap;
}

PWMap MinAdjMRV::impl(const DSBG& dsbg)
{
  Util::DEBUG_LOG << "MinAdjMRV:\n" << dsbg << "\n\n";

  dsbg_ = dsbg;
  rmap_ = fact_.createPWMap(dsbg.V());

  Set V = dsbg_.V();
  Set E = dsbg_.E();
  PWMap mapB = dsbg_.mapB();
  PWMap mapD = dsbg_.mapD();
  PWMap subEmap = dsbg_.subEmap();

  if (!V.isEmpty()) {
    PWMap old_rmap = fact_.createPWMap();

    if (E.isEmpty())
      return rmap_;

    Set changedV = fact_.createSet();
    do {
      old_rmap = rmap_;

      PWMap ermapD = rmap_.composition(mapD);

      PWMap new_rmap = mapB.minAdjMap(ermapD);
      rmap_ = rmap_.minMap(new_rmap).combine(rmap_);
      Util::DEBUG_LOG << "rmap before rec: " << rmap_ << "\n\n";

      PWMap recursion_rmap = fact_.createPWMap();
      // Vertices that have a new representative
      changedV = V.difference(old_rmap.equalImage(rmap_));
      if (!changedV.isEmpty()) {
        // If the mrv is in the same SV, the algorithm would detect a false
        // recursion, i.e. if we have a cycle 1 -> 2 -> ... -> 10 -> 1,
        // where SV = [1:10], then it detects a recursion when mrv(10) = 1
        // (false recursion). So we take self mrvs out.
        Set represented = V.difference(rmap_.fixedPoints());
        for (const Map& subv : dsbg.Vmap()) {
          Set vs = subv.dom();
          if (!vs.intersection(changedV).isEmpty()) {
            // Vertices in the set-vertex that share its rep with other vertex
            // in the set-vertex
            Set recursiveV
              = rmap_.restrict(vs.intersection(represented)).sharedImage();
            // There is a recursive vertex that changed its rep in the last step
            // (to avoid computing again an already found recursion)
            if (!recursiveV.intersection(changedV).isEmpty()) {
              recursion_rmap = recursivePaths(changedV, recursiveV);
            }
          }
        }

        PWMap rmap_plus = recursion_rmap.combine(rmap_);
        rmap_plus = rmap_plus.mapInf();
        rmap_ = rmap_.minMap(rmap_plus).compact();

        Util::DEBUG_LOG << "rmap after rec: " << rmap_ << "\n\n";
      }
    } while (!changedV.isEmpty());

    return rmap_;
  }

  return fact_.createPWMap();
}

template class MRV<MinAdjMRV>;

} // namespace LIB

} // namespace SBG
