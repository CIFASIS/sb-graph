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

#include "algorithms/tearing/tearing.hpp"
#include "util/logger.hpp"

namespace SBG {

namespace LIB {
    
////////////////////////////////////////////////////////////////////////////////
// Tearing ---------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

bool sccNotEqId(const Map &sbgmap) { return !(sbgmap.isId()); }

Tearing::Tearing(const DSBG &dsbg, bool debug)
    : fact_(dsbg.fact()),
      dsbg_(dsbg),
      V_(fact_.createSet()),
      Vmap_(fact_.createPWMap()),
      Emap_(fact_.createPWMap()),
      subEmap_(fact_.createPWMap()),
      E_(fact_.createSet()),
      Ediff_(fact_.createSet()),
      mapB_(fact_.createPWMap()),
      mapD_(fact_.createPWMap()),
      rmap_(fact_.createPWMap()),
      debug_(debug)
{
  DSBG dg = dsbg;
  dsbg_ = dg;

  V_ = dg.V();
  Vmap_ = dg.Vmap();

  E_ = dg.E();
  Emap_ = dg.Emap();
  subEmap_ = dg.subEmap();

  mapB_ = dg.mapB();
  mapD_ = dg.mapD();

  rmap_ = fact_.createPWMap(V_);
}

member_imp(Tearing, DSBG, dsbg);
member_imp(Tearing, Set, V);
member_imp(Tearing, PWMap, Vmap);
member_imp(Tearing, Set, E);
member_imp(Tearing, PWMap, Emap);
member_imp(Tearing, PWMap, subEmap);

member_imp(Tearing, PWMap, mapB);
member_imp(Tearing, PWMap, mapD);

member_imp(Tearing, Set, Ediff);

member_imp(Tearing, PWMap, rmap);

member_imp(Tearing, bool, debug);

PWMap Tearing::sccMinReach(const DSBG &dg) const
{
  if (debug())
    Util::SBG_LOG << "Min reach graph:\n" << dg << "\n\n";

  Set V = dg.V(), E = dg.E();
  PWMap mapB = dg.mapB(), mapD = dg.mapD(), subEmap = dg.subEmap();
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
      if (debug())
        Util::SBG_LOG << "rmap before rec: " << rmap << "\n\n";

      Set positive = fact_.createSet(SetPiece(copies, Interval(1, 1, Inf)));
      PWMap rec_rmap = fact_.createPWMap();
      Vc = V.difference(old_rmap.equalImage(rmap));
      if (!Vc.isEmpty()) {
        for (const Map &subv : dg.Vmap()) {
          Set vs = subv.dom();
          if (!vs.intersection(Vc).isEmpty()) {
            // If the mrv is in the same SV, the algorithm would detect a false
            // recursion, i.e. if we have a cycle 1 -> 2 -> ... -> 10 -> 1,
            // where SV = [1:10], then it detects a recursion when mrv(10)
            // becomes "1" (false recursion). So we take self mrvs out.
            auto other_rep = rmap.filterMap([](const Map &sbgmap) {
              return sccNotEqId(sbgmap);
            }).dom();
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

                if (debug()) {
                  Util::SBG_LOG << "VR: " << VR << "\n";
                  Util::SBG_LOG << "repV: " << repV << "\n";
                  Util::SBG_LOG << "ER: " << ER << "\n";
                  Util::SBG_LOG << "dmap: " << dmap << "\n";
                  Util::SBG_LOG << "not_cycle_edges: " << not_cycle_edges << "\n";
                  Util::SBG_LOG << "ER_plus: " << ER_plus << "\n";
                  Util::SBG_LOG << "smap_plus: " << smap_plus << "\n";
                  Util::SBG_LOG << "rec_rmap: " << rec_rmap << "\n\n";
                }
              }
            }
          }
        }
        PWMap rmap_plus = rec_rmap.combine(rmap);
        rmap_plus = rmap_plus.mapInf();
        rmap = rmap.minMap(rmap_plus).compact();

        if (debug())
          Util::SBG_LOG << "rmap after rec: " << rmap << "\n\n";
      }
    } while (!Vc.isEmpty()); 

    return rmap;
  }

  return fact_.createPWMap();
}

PWMap Tearing::sccStep()
{
  PWMap id_V = fact_.createPWMap(V());

  DSBG aux_dsbg(
    fact_, V(), Vmap()
    , mapB().restrict(E()), mapD().restrict(E()), Emap().restrict(E()).compact()
    , subEmap().restrict(E())
  );
  PWMap new_rmap = sccMinReach(aux_dsbg);
  if (debug())
    Util::SBG_LOG << "SCC new_rmap: " << new_rmap << "\n";

  PWMap rmap_B = new_rmap.composition(mapB());
  PWMap rmap_D = new_rmap.composition(mapD());
  Set Esame = rmap_B.equalImage(rmap_D); // Edges in the same SCC
  
  // Leave edges in the same SCC
  Ediff_ = E().difference(Esame);
  E_ = Esame;
  if (debug())
    Util::SBG_LOG << "SCC erased edges: " << Ediff() << "\n\n";

  // Swap directions
  PWMap aux_B = mapB();
  mapB_ = mapD();
  mapD_ = aux_B;

  return new_rmap;
}

void Tearing::restoreSBG() 
{
  V_ = dsbg_.V();
  Vmap_ = dsbg_.Vmap();

  E_ = dsbg_.E();
  Emap_ = dsbg_.Emap();
  subEmap_ = dsbg_.subEmap();

  mapB_ = dsbg_.mapB();
  mapD_ = dsbg_.mapD();
}

PWMap Tearing::calculate()
{
  if (debug()) Util::SBG_LOG << "Tearing dsbg: \n" << dsbg() << "\n\n";

  auto begin = std::chrono::high_resolution_clock::now();
  PWMap rmap = sccStep();
  Set vrem = fact_.createSet();
  Set e_scc = fact_.createSet();
  do {
    // SCC
    do {
      rmap = sccStep();
    } while (Ediff() != fact_.createSet());
    //
    e_scc = E();
    if (E() != fact_.createSet()) {
      Set erem_b = mapB().preImage(rmap.image());
      Set erem_d = mapD().preImage(rmap.image());
      vrem = vrem.cup(mapB().image(erem_b)).cup(mapD().image(erem_d));
      E_ = E().difference(erem_b.cup(erem_d));
    }
    rmap_ = rmap.compact();
  } while (E() != fact_.createSet());
  restoreSBG();
  Set e_notscc = E().difference(e_scc);
  Set e_to_R = mapD().preImage(rmap.image());
  PWMap mapD_notscc = mapD().restrict(e_to_R);
  
  Set erem = e_scc.intersection(mapB().preImage(vrem));
  Set mapD_aux = mapD().image(erem);

  auto end = std::chrono::high_resolution_clock::now();

  auto total = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
  Util::SBG_LOG << "Total Tearing exec time: " << total.count() << " [μs]\n\n";
  std::cout << vrem << std::endl;
  std::cout << dsbg().E() << std::endl;

  if (debug()) {
    Util::SBG_LOG << "Tearing result: " << rmap.compact() << "\n\n";
    E().print(std::cout);
  }

  return rmap.compact();
}

const PWMapAF &Tearing::fact() const { return fact_; }

} // namespace LIB

} // namespace SBG
