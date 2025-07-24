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

#include "algorithms/scc/scc.hpp"
#include "util/logger.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Auxiliary structures --------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

member_imp(SCCData, DSBG, dsbg);
member_imp(SCCData, PWMap, rmap);
member_imp(SCCData, Set, Ediff);

SCCData::SCCData(DSBG dsbg, PWMap rmap, Set Ediff)
  : dsbg_(dsbg), rmap_(rmap), Ediff_(Ediff) {}

////////////////////////////////////////////////////////////////////////////////
// SCC Algorithm Delegate Constructors -----------------------------------------
////////////////////////////////////////////////////////////////////////////////

SCCDelegate::SCCDelegate(const PWMapAF &fact) : fact_(std::move(fact)) {}

////////////////////////////////////////////////////////////////////////////////
// Minimum Reachable SCC Algorithm ---------------------------------------------
////////////////////////////////////////////////////////////////////////////////

MinReachSCC::MinReachSCC(const PWMapAF &fact)
  : SCCDelegate(fact), E_(fact_.createSet()), Ediff_(fact_.createSet())
    , mapB_(fact_.createPWMap()), mapD_(fact_.createPWMap()) {}

member_imp(MinReachSCC, Set, E);

member_imp(MinReachSCC, PWMap, mapB);
member_imp(MinReachSCC, PWMap, mapD);

member_imp(MinReachSCC, Set, Ediff);

PWMap MinReachSCC::sccMinReach(const DSBG &dsbg) const
{
  Util::DEBUG_LOG << "Min reach graph:\n" << dsbg << "\n\n";

  Set V = dsbg.V(), E = dsbg.E();
  PWMap mapB = dsbg.mapB(), mapD = dsbg.mapD(), subEmap = dsbg.subEmap();
  if (!V.isEmpty()) {
    unsigned int copies = V.arity();
    PWMap rmap = fact_.createPWMap(V), old_rmap = fact_.createPWMap();

    if (E.isEmpty())
      return rmap;

    Set Vc = fact_.createSet();
    do {
      old_rmap = rmap;

      PWMap ermapD = rmap.composition(mapD);

      PWMap new_rmap = mapB.minAdjMap(ermapD);
      rmap = rmap.minMap(new_rmap).combine(rmap);
      Util::DEBUG_LOG << "rmap before rec: " << rmap << "\n\n";

      Set positive = fact_.createSet(SetPiece(copies, Interval(1, 1, Inf)));
      PWMap rec_rmap = fact_.createPWMap();
      Vc = V.difference(old_rmap.equalImage(rmap));
      if (!Vc.isEmpty()) {
        // If the mrv is in the same SV, the algorithm would detect a false
        // recursion, i.e. if we have a cycle 1 -> 2 -> ... -> 10 -> 1,
        // where SV = [1:10], then it detects a recursion when mrv(10) = 1
        // (false recursion). So we take self mrvs out.
        Set other_rep = V.difference(rmap.fixedPoints());
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
                PWMap dmap = fact_.createPWMap();
                Set ith = end;
                NAT dist = 0;
                // Calculate distance for vertices in same_rep that reach reps
                for (; dmap.dom().intersection(Vc.intersection(VR)).isEmpty();) {
                  Set dom = ith.difference(dmap.dom());
                  Exp exp(MD_NAT(copies, dist));
                  dmap.emplaceBack(fact_.createMap(dom, exp));
                  // Update ith to vertices that have outgoing edges entering ith
                  ith = mapB.image(mapD.preImage(ith));
                  ++dist;
                }
                PWMap dmapB = dmap.composition(mapB), dmapD = dmap.composition(mapD);
                // Get edges where the end is closer to the rep than the beginning
                Set not_cycle_edges = (dmapB - dmapD).preImage(positive);
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

  return fact_.createPWMap();
}

PWMap MinReachSCC::sccStep(const DSBG &dsbg)
{
  Set V = dsbg.V();
  PWMap Vmap = dsbg.Vmap(), Emap = dsbg.Emap(), subEmap = dsbg.subEmap();

  PWMap id_V = fact_.createPWMap(V);

  DSBG aux_dsbg(
    fact_, V, Vmap
    , mapB().restrict(E_), mapD().restrict(E_), Emap.restrict(E_).compact()
    , subEmap.restrict(E_)
  );
  PWMap new_rmap = sccMinReach(aux_dsbg);
  Util::DEBUG_LOG << "MinReachSCC new_rmap: " << new_rmap << "\n";

  PWMap rmapB = new_rmap.composition(mapB());
  PWMap rmapD = new_rmap.composition(mapD());
  Set Esame = rmapB.equalImage(rmapD); // Edges in the same MinReachSCC
  
  // Leave edges in the same MinReachSCC
  Ediff_ = E().difference(Esame);
  E_ = Esame;
  Util::DEBUG_LOG << "MinReachSCC erased edges: " << Ediff_ << "\n\n";

  // Swap directions
  PWMap aux_B = mapB_;
  mapB_ = mapD_;
  mapD_ = aux_B;

  return new_rmap;
}

void MinReachSCC::init(const DSBG &dsbg)
{
  E_ = dsbg.E();
  mapB_ = dsbg.mapB();
  mapB_ = dsbg.mapB();
}

SCCData MinReachSCC::calculate(const DSBG &dsbg)
{
  Util::DEBUG_LOG << "MinReachSCC dsbg: \n" << dsbg << "\n\n";

  init(dsbg);

  auto begin = std::chrono::high_resolution_clock::now();
  PWMap rmap = sccStep(dsbg);
  do {
    rmap = sccStep(dsbg);
  } while (Ediff_ != fact_.createSet());
  rmap = rmap.compact();
  auto end = std::chrono::high_resolution_clock::now();

  auto total = std::chrono::duration_cast<std::chrono::microseconds>(
    end - begin
  );
  Util::SBG_LOG << "Total MinReachSCC exec time: " << total.count() << " [μs]\n\n"; 

  Util::DEBUG_LOG << "MinReachSCC result: " << rmap << "\n\n";

  return SCCData(dsbg, rmap, dsbg.E().difference(E_));
}

////////////////////////////////////////////////////////////////////////////////
// SCC Algorithm Implementation ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

SCC::SCC(SCCDelegPtr deleg) : delegate_(std::move(deleg)) {}

SCCData SCC::calculate(const DSBG &dsbg)
{
  return delegate_->calculate(dsbg);
}

} // namespace LIB

} // namespace SBG
