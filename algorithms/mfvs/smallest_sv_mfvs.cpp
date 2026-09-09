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

#include "algorithms/mfvs/smallest_sv_mfvs.hpp"
#include "algorithms/scc/scc.hpp"
#include "sbg/integer.hpp"
#include "sbg/pw_map.hpp"
#include "util/logger.hpp"

#include <functional>
#include <numeric>

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Smallest set-vertex MFVS Algorithm ------------------------------------------
////////////////////////////////////////////////////////////////////////////////

SmallestSVMFVS::SmallestSVMFVS() {}

/**
 * @brief It calculates the set of vertices that belong to the smallest
 * set-vertices described by Vmap.
 */
Set getVerticesFromSmallestSV(const PWMap& Vmap)
{
  Set Vmap_image = Vmap.image();
  std::size_t min_sz = std::numeric_limits<std::size_t>::max();
  Set remaining = Vmap_image;
  while (!remaining.isEmpty()) {
    Set jth_sv{remaining.minElem()};
    std::size_t jth_sz = Vmap.preImage(jth_sv).cardinal(); 
    if (jth_sz < min_sz) {
      min_sz = jth_sz;
    }

    remaining = remaining.difference(jth_sv);
  }

  Set Vsmall;
  remaining = Vmap_image;
  while (!remaining.isEmpty()) {
    Set jth_sv{remaining.minElem()};
    Set jth_vertices = Vmap.preImage(jth_sv);
    if (jth_vertices.cardinal() == min_sz) {
      Vsmall = Vsmall.disjointCup(jth_vertices);
    }

    remaining = remaining.difference(jth_sv);
  }

  return Vsmall;
}

/**
 * @brief It calculates the minimum vertex of maximum degree of V. 
 */
IntTuple getMaxDegreeVertex(const Set& V, const DirectedSBG& dsbg)
{
  PWMap mapB = dsbg.mapB();
  PWMap mapD = dsbg.mapD();
  Set adj_edges = mapB.preImage(V).cup(mapD.preImage(V));
  PWMap multB = mapB.restrict(adj_edges).imageMultiplicity();
  PWMap multD = mapD.restrict(adj_edges).imageMultiplicity();
  PWMap mmap = (multB + multD).restrict(V);

  if (mmap.isEmpty()) {
    return V.minElem();
  }

  Set max_mult_set{mmap.image().maxElem()};
  Set max_degree_vertices = mmap.preImage(max_mult_set);

  return max_degree_vertices.minElem();
}

Set SmallestSVMFVS::calculate(const DirectedSBG& input_dsbg) const
{
  if (input_dsbg.V().isEmpty()) {
    return Set{};
  }

  DirectedSBG dsbg = input_dsbg;

  Util::DEBUG_LOG << "initial smallest set-vertex mfvs dsbg:\n" << dsbg << "\n";

  PWMap rmap = SCC{}.calculate(dsbg).rmap();
  Set init_fixed = rmap.fixedPoints();
  Set fvs_result;
  Set visitedSV;
  while (rmap.fixedPoints() != rmap.domain()) {
    // Get vertex from smallest set-vertex, of maximum degree
    Set Vsmall = getVerticesFromSmallestSV(dsbg.Vmap());
    Set Vj{getMaxDegreeVertex(Vsmall, dsbg)};
    fvs_result = std::move(fvs_result).disjointCup(Vj);

    // Handle repetition
    PWMap Vmap = dsbg.Vmap();
    Set repeatedSV = visitedSV.intersection(Vmap.image(Vj));
    if (!repeatedSV.isEmpty()) {
      Set V_plus = Vmap.preImage(Vmap.image(Vj));
      fvs_result = std::move(fvs_result).cup(std::move(V_plus));
    } else {
      visitedSV = std::move(repeatedSV).disjointCup(Vmap.image(Vj));
    }

    // Erase selected vertices
    dsbg.eraseVertices(fvs_result);

    // Resulting SCC from induced graph
    rmap = SCC{}.calculate(dsbg).rmap();

    // Erase vertices that get isolated after removing the elements of Vj
    dsbg.eraseVertices(rmap.fixedPoints().difference(init_fixed));

    Util::DEBUG_LOG << "Vj: " << Vj << "\n";
    Util::DEBUG_LOG << "new rmap: " << rmap << "\n\n";
  }

  fvs_result.compact();
  return fvs_result;
}

} // namespace detail

} // namespace LIB

} // namespace SBG
