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

#include "algorithms/misc/causalization_builders.hpp"
#include "sbg/bipartite_sbg.hpp"
#include "sbg/map.hpp"
#include "sbg/pw_map.hpp"
#include "sbg/set.hpp"
#include "util/time_profiler.hpp"

#include <tuple>

namespace misc {

////////////////////////////////////////////////////////////////////////////////
// Algebraic loops detection graph builder -------------------------------------
////////////////////////////////////////////////////////////////////////////////

std::tuple<SBG::LIB::Set, SBG::LIB::PWMap> buildSCCVertices(
  const SBG::LIB::MatchData& data)
{
  const SBG::LIB::BipartiteSBG& bsbg = data.bsbg();
  SBG::LIB::Set M = data.M();

  M.compact();
  SBG::LIB::Set V = M;
  SBG::LIB::PWMap auxVmap = data.bsbg().Emap().restrict(M);
  SBG::LIB::PWMap Vmap;
  for (const SBG::LIB::Map& m : auxVmap) { 
    SBG::LIB::Set domain = m.domain();
    domain.compact();
    Vmap.emplace(domain, m.law());
  }

  return {V, Vmap};
}

std::tuple<SBG::LIB::PWMap, SBG::LIB::PWMap, SBG::LIB::PWMap> buildSCCEdges(
  const SBG::LIB::MatchData& data, const SBG::LIB::PWMap& Vmap)
{
  const SBG::LIB::BipartiteSBG& bsbg = data.bsbg();
  SBG::LIB::Set M = data.M();

  SBG::LIB::PWMap map1 = bsbg.map1();
  SBG::LIB::PWMap map2 = bsbg.map2();

  SBG::LIB::Set X = bsbg.X();
  SBG::LIB::PWMap map1_toX = map1.restrict(map1.preImage(X));
  SBG::LIB::PWMap map2_toX = map2.restrict(map2.preImage(X));
  SBG::LIB::PWMap mapF = map1_toX.concatenation(map2_toX);

  SBG::LIB::Set Y = bsbg.Y();
  SBG::LIB::PWMap map1_toY = map1.restrict(map1.preImage(Y));
  SBG::LIB::PWMap map2_toY = map2.restrict(map2.preImage(Y));
  SBG::LIB::PWMap mapU = map1_toY.concatenation(map2_toY);

  SBG::LIB::Set free_edges = bsbg.E().difference(M);
  SBG::LIB::PWMap matchedF_inv = mapF.restrict(M).inverse();
  SBG::LIB::PWMap unmatchedF = mapF.restrict(free_edges);
  SBG::LIB::PWMap mapB = matchedF_inv.composition(unmatchedF);
  mapB.compact();

  SBG::LIB::PWMap matchedU_inv = mapU.restrict(M).inverse();
  SBG::LIB::PWMap unmatchedU = mapU.restrict(free_edges);
  SBG::LIB::PWMap mapD = matchedU_inv.composition(unmatchedU);
  mapD.compact();

  SBG::LIB::PWMap Emap = bsbg.Emap().restrict(free_edges);
  Emap.compact();

  return {mapB, mapD, Emap};
}

void partitionEmap(SBG::LIB::DirectedSBG& dsbg)
{
  SBG::LIB::Set V = dsbg.V();
  SBG::LIB::PWMap Vmap = dsbg.Vmap();
  SBG::LIB::PWMap mapB = dsbg.mapB();
  SBG::LIB::PWMap mapD = dsbg.mapD();
  SBG::LIB::PWMap Emap = dsbg.Emap();

  std::size_t arity = V.arity();
  SBG::LIB::NAT j = 1;
  SBG::LIB::PWMap partitioned_Emap;
  dsbg.foreachSetEdge([&](const SBG::LIB::MD_NAT& SE)
  {
    SBG::LIB::Set E = Emap.preImage(SBG::LIB::Set{SE});
    SBG::LIB::Set SV_starts = Vmap.image(mapB.image(E));
    SBG::LIB::Set SV_ends = Vmap.image(mapD.image(E));
    if (SV_starts.cardinal()*SV_ends.cardinal() > 1) {
      SBG::LIB::Set remaining1 = SV_starts;
      while (!remaining1.isEmpty()) {
        SBG::LIB::Set SV1 = SBG::LIB::Set{remaining1.minElem()};
        SBG::LIB::Set V1 = Vmap.preImage(SV1); 

        SBG::LIB::Set remaining2 = SV_ends;
        while (!remaining2.isEmpty()) {
          SBG::LIB::Set SV2 = SBG::LIB::Set{remaining2.minElem()};
          SBG::LIB::Set V2 = Vmap.preImage(SV2); 

          SBG::LIB::Set edges_V1_V2 = mapB.preImage(V1).intersection(
            mapD.preImage(V2));
          partitioned_Emap.emplace(E.intersection(edges_V1_V2)
            , SBG::LIB::Expression{SBG::LIB::MD_NAT{arity, j}});
          ++j;

          remaining2 = remaining2.difference(SV2);
        }
        remaining1 = remaining1.difference(SV1);
      }
    } else {
      partitioned_Emap.emplace(E
        , SBG::LIB::Expression{SBG::LIB::MD_NAT{arity, j}});
      ++j;
    }
  });

  dsbg = SBG::LIB::DirectedSBG{V, Vmap, mapB, mapD, partitioned_Emap};
}

SBG::LIB::DirectedSBG buildLoopDetectionSBG(const SBG::LIB::MatchData& data)
{
  SBG::Util::Internal::TimeProfiler profiler{"SBG Loop Detection builder: "};

  SBG::LIB::Set V;
  SBG::LIB::PWMap Vmap;
  std::tie(V, Vmap) = buildSCCVertices(data);

  SBG::LIB::PWMap mapB;
  SBG::LIB::PWMap mapD;
  SBG::LIB::PWMap Emap;
  std::tie(mapB, mapD, Emap) = buildSCCEdges(data, Vmap);

  SBG::LIB::DirectedSBG dsbg{V, Vmap, mapB, mapD, Emap};
  partitionEmap(dsbg);

  return dsbg;
}

////////////////////////////////////////////////////////////////////////////////
// Algebraic loops breaker graph builder ---------------------------------------
////////////////////////////////////////////////////////////////////////////////

SBG::LIB::DirectedSBG buildTearingSBG(const SBG::LIB::SCCData& data)
{
  SBG::Util::Internal::TimeProfiler profiler{"SBG Tearing builder: "};

  SBG::LIB::DirectedSBG dsbg = data.dsbg();
  dsbg.eraseEdges(data.Ediff());

  return dsbg;
}

////////////////////////////////////////////////////////////////////////////////
// Vertical sort graph builder ------------------------------------------------- 
////////////////////////////////////////////////////////////////////////////////

SBG::LIB::DirectedSBG buildVerticalSortingSBG(const SBG::LIB::SCCData& data
  , const SBG::LIB::Set& mfvs)
{
  SBG::Util::Internal::TimeProfiler profiler{"SBG Vertical Sorting builder: "}; 

  const SBG::LIB::DirectedSBG& dsbg = data.dsbg();

  SBG::LIB::Set V = dsbg.V();
  SBG::LIB::PWMap Vmap = dsbg.Vmap();

  SBG::LIB::Set dependencies = dsbg.mapD().preImage(mfvs);
  SBG::LIB::Set E = dsbg.E().difference(dependencies);
  SBG::LIB::PWMap mapB = dsbg.mapB().restrict(E);
  SBG::LIB::PWMap mapD = dsbg.mapD().restrict(E);
  SBG::LIB::PWMap Emap = dsbg.Emap().restrict(E);

  return SBG::LIB::DirectedSBG{V, Vmap, mapB, mapD, Emap};
}

} // namespace misc
