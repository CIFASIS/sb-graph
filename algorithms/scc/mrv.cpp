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
// Minimum Adjacent MRV Implementation -----------------------------------------
////////////////////////////////////////////////////////////////////////////////

MinAdjMRV::MinAdjMRV(const PWMapAF& fact) : MRV<MinAdjMRV>(fact)
  , dsbg_(fact), smap_(fact_.createPWMap()), visitedSE_(fact.createSet()) {}

Set MinAdjMRV::decreasingRepresentative(const PWMap& rmap) const
{
  Set result = fact_.createSet();

  PWMap mapB = dsbg_.mapB();
  PWMap mapD = dsbg_.mapD();

  if (mapB.isEmpty() || mapD.isEmpty())
    return result;

  PWMap rmapB = rmap.composition(mapB);
  PWMap rmapD = rmap.composition(mapD);

  unsigned int dims = rmapD.arity();
  PWMap offD = rmapD.offsetImage(MD_NAT(dims, 1)); 
  PWMap subt = (offD - rmapB);
  SetPiece im(dims, Interval(0, 1, Inf));
  Set min_in_mapD = fact_.createSet();
  for (unsigned int k = 0; k < dims; ++k) {
    im[k] = Interval(0, 1, 0);
    if (k > 0)
      im[k-1] = Interval(1, 1, 1);

    Set kth = subt.preImage(fact_.createSet(im));
    min_in_mapD = min_in_mapD.disjointCup(kth);
  }

  return min_in_mapD;
}

Set MinAdjMRV::edgesInPaths(const PWMap& smap) const
{
  PWMap mapB = dsbg_.mapB();
  PWMap mapD = dsbg_.mapD();

  // Vertices that are successors of other vertices in a path
  Set not_fixed = smap.dom().difference(smap.fixedPoints());
  Set succs = smap.restrict(not_fixed).image();
  // Edges whose endings are successors 
  Set ending_edges = mapD.preImage(succs);
  // Map from a 'successor' edge to its start
  PWMap auxB = mapB.restrict(ending_edges);
  // Map from edge to the successor of its start
  PWMap map_succs = smap.composition(auxB);
 
  return map_succs.equalImage(mapD);
}

PWMap MinAdjMRV::recursivePaths(const Set& ith_paths_edges, const Set& outgoing)
{
  PWMap result = fact_.createPWMap();

  PWMap subEmap = dsbg_.subEmap();
  Set ithSE = subEmap.image(ith_paths_edges);
  visitedSE_ = visitedSE_.cup(ithSE);
  Set repeatedSE = visitedSE_.intersection(ithSE);
  if (!repeatedSE.isEmpty()) {
    PWMap mapB = dsbg_.mapB();
    PWMap mapD = dsbg_.mapD();

    Set ith_start = smap_.dom().difference(smap_.image());
    Set E = fact_.createSet(); 
    PWMap subEmap = dsbg_.subEmap();
    bool exit_condition = true;
    do {
      Set ithE = mapB.preImage(ith_start).intersection(ith_paths_edges);
      E = E.disjointCup(ithE);
      ith_start = mapD.image(ithE);
      exit_condition = !repeatedSE.intersection(subEmap.image(E)).isEmpty();
    } while (!exit_condition);

    Set smap_edges = edgesInPaths(smap_);
    Set adj = mapB.preImage(mapB.image(smap_edges));

    Set E_plus = subEmap.preImage(subEmap.image(E));
    E_plus = E_plus.difference(mapB.preImage(outgoing));
    E_plus = E_plus.difference(adj);
    PWMap mapB_plus = dsbg_.mapB().restrict(E_plus);
    PWMap mapD_plus = dsbg_.mapD().restrict(E_plus);
    result = mapB_plus.minAdjMap(mapD_plus);
  }

  return result;
}


PWMap MinAdjMRV::impl(const DSBG& dsbg)
{
  std::cout << "MinAdjMRV dsbg:\n" << dsbg << "\n\n";

  dsbg_ = dsbg;
  PWMap mapB = dsbg_.mapB();
  PWMap mapD = dsbg_.mapD();
  PWMap subEmap = dsbg_.subEmap();
  visitedSE_ = fact_.createSet();

  smap_ = fact_.createPWMap(dsbg.V());
  PWMap rmap = smap_;

  if (!dsbg_.V().isEmpty() && !dsbg_.E().isEmpty()) {
    PWMap old_rmap = fact_.createPWMap();
    Set E = fact_.createSet();
    Set paths_edges = fact_.createSet();
    do {
      old_rmap = rmap;

      // Calculate successor map using edges that lead to a minor representative
      E = decreasingRepresentative(rmap);
      PWMap decreasingB = mapB.restrict(E);
      PWMap decreasingD = mapD.restrict(E);
      PWMap decreasing_smap = decreasingB.minAdjMap(decreasingD); 
      smap_ = decreasing_smap.combine(smap_);

      // Recursive paths
      Set ith_paths_edges = edgesInPaths(decreasing_smap);
      Set outgoing = rmap.image(mapD.image(ith_paths_edges));
      PWMap smap_plus = recursivePaths(ith_paths_edges, outgoing);
      smap_ = smap_plus.combine(smap_);

      // Calculate representatives map
      rmap = smap_.mapInf();
      rmap = rmap.minMap(old_rmap);
    } while (!E.isEmpty());
  }

  return rmap;
}


template class MRV<MinAdjMRV>;

} // namespace LIB

} // namespace SBG
