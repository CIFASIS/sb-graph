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

#include <sbgraph/detail/algorithms/sorting/topological/two_phased_ts.hpp>
#include "util/debug.hpp"
#include "util/logger.hpp"

#include <algorithm>

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Two Phased Topological Sort Algorithm ---------------------------------------
////////////////////////////////////////////////////////////////////////////////

TwoPhasedTS::TwoPhasedTS() : _depth(0) {}

// Handling of repetitions -----------------------------------------------------

PWMap TwoPhasedTS::extendRepetition() const
{
  // Extend repetition to whole set-vertices.
  PWMap smap_plus;
  PWMap last_sort = _repetition_sort.last(_depth);
  for (const Map& m : last_sort) {
    Set domain_plus = _original_Vmap.preImage(_original_Vmap.image(m.domain()));
    smap_plus.insert(Map{domain_plus, m.law()});
  }

  // Take out already sorted elements.
  Set sorted = _smap.domain().cup(smap_plus.preImage(_smap.image()));
  Set plus_domain = smap_plus.domain().difference(sorted);
  smap_plus = smap_plus.restrict(plus_domain);

  return smap_plus;
}

PWMap TwoPhasedTS::repetition(const PWMap& Emap)
{
  // Identify repetition and extend it to vertices in the same set-vertex,
  // taking out sorted vertices in the domain, and also vertices that map to
  // another sorted vertex, i.e, checking the successor map is injective.
  PWMap smap_plus = extendRepetition();
  if (smap_plus.isEmpty()) {
    return PWMap{};
  }
  Set plus_domain = smap_plus.domain();

  // Delete set-edges that had an edge that was deleted up to the repetition.
  Set Eplus = _original_Emap.preImage(_repetitionSE.last(_depth));
  DirectedSBG dsbg_copy = _dsbg;
  dsbg_copy.eraseEdges(Eplus);

  // Identify vertices with ingoing edges after the previous step. These are
  // "cut" vertices for the repetition.
  Set dependent_vertices = dsbg_copy.mapD().image().intersection(plus_domain);
  PWMap rmap = PWMap{dependent_vertices}.combine(smap_plus).mapInf();

  Set indep_vertices = plus_domain
    .difference(rmap.preImage(dependent_vertices));
  PWMap result = smap_plus.restrict(indep_vertices);

  // Delete processed elements of the repetition.
  _repetitionSE.dropLast(_depth);
  _repetition_sort.dropLast(_depth);
  _depth = 0;

  return result;
}

// Sort inside each component --------------------------------------------------

PWMap TwoPhasedTS::sortInside(const PWMap& Emap)
{
  Set oldI = _dsbg.V().difference(_dsbg.mapD().image());
  _dsbg.eraseVertices(oldI);
  while (!_dsbg.V().isEmpty()) {
    Set I = _dsbg.V().difference(_dsbg.mapD().image());
    PWMap Imin = _rmap.restrict(I).minAdj(PWMap{I});
    PWMap oldImin = _rmap.restrict(oldI).minAdj(PWMap{oldI}); 
    PWMap smapI = Imin.minAdj(oldImin);
    _smap = smapI.combine(std::move(_smap));
    _repetition_sort.pushBack(smapI);
    _repetitionSE.pushBack(Emap.image(_dsbg.mapB().preImage(I)));

    // Handle repetition.
    _depth = _repetition_sort.repetitionDepth();
    _repetition_sort.pushBack(smapI);
    if (_depth > 0) {
      PWMap smap_plus = repetition(Emap);
      _smap = std::move(smap_plus).concatenation(std::move(_smap));
    }

    // Update values for new iteration.
    oldI = _smap.domain().difference(_smap.image());
    _smap.compact();
    _dsbg.eraseVertices(_smap.domain());

    Util::DEBUG_LOG << "inside_smap: " << _smap << "\n";
    Util::DEBUG_LOG << "inside_dsbg: " << _dsbg << "\n\n";
  }

  return _smap;
}

// Sort between components -----------------------------------------------------

PWMap TwoPhasedTS::sortOutside(const PWMap& Emap)
{
  std::size_t arity = _dsbg.V().arity();

  IntTuple last = _dsbg.V().difference(_dsbg.mapD().image()).minElem();
  _smap = PWMap{Set{last}};
  while (!_dsbg.V().isEmpty()) {
    Set I = _dsbg.V().difference(_dsbg.mapD().image());
    PWMap smapI = sort(I);
    IntTuple first = smapI.fixedPoints().minElem();
    Expression expr = Expression{arity, 1, 0} + Expression{last - first};
    smapI = PWMap{Map{Set{first}, expr}}.combine(smapI); 
    _smap = smapI.combine(std::move(_smap));
    _repetition_sort.pushBack(smapI);
    _repetitionSE.pushBack(Emap.image(_dsbg.mapB().preImage(I)));

    // Handle repetition.
    _depth = _repetition_sort.repetitionDepth();
    if (_depth > 0) {
      PWMap smap_plus = repetition(Emap);
      _smap = std::move(smap_plus).concatenation(std::move(_smap));
    }

    // Update values for new iteration.
    Set starts = _smap.domain().difference(_smap.image());
    if (!starts.isEmpty()) {
      last = _smap.domain().difference(_smap.image()).minElem();
    } else {
      last = _smap.fixedPoints().minElem();
    }
    _smap.compact();
    _dsbg.eraseVertices(_smap.domain());

    Util::DEBUG_LOG << "outside_smap: " << _smap << "\n";
    Util::DEBUG_LOG << "outside_dsbg: " << _dsbg << "\n\n";
  }

  return _smap;
}

// Algorithm -------------------------------------------------------------------

PWMap TwoPhasedTS::calculate(TSInput input)
{
  DirectedSBG dsbg = input.dsbg();
  Util::DEBUG_LOG << "Two phased topological sort dsbg:\n" << dsbg << "\n\n";
  if (dsbg.V().isEmpty()) {
    return PWMap{};
  }

  IntTuple start = dsbg.V().difference(dsbg.mapD().image()).minElem();
  PWMap Vmap_copy = dsbg.Vmap();
  _repetition_sort = SortedMaps{Vmap_copy};

  PWMap scc_map = input.rmap();
  _rmap = scc_map;
  PWMap rmapB = scc_map.composition(dsbg.mapB());
  PWMap rmapD = scc_map.composition(dsbg.mapD());
  Set Esame = rmapB.equalImage(rmapD);
  Set Ediff = dsbg.E().difference(Esame);

  // Sort inside components.
  DirectedSBG copy_dsbg = dsbg;
  PWMap mmap = scc_map.imageMultiplicity();
  Set one{IntTuple{copy_dsbg.V().arity(), 1}};
  copy_dsbg.eraseVertices(mmap.preImage(one));
  copy_dsbg.eraseEdges(Ediff);
  _original_Vmap = copy_dsbg.Vmap();
  _original_Emap = copy_dsbg.Emap();
  _dsbg = copy_dsbg;
  PWMap inside_smap = sortInside(dsbg.Emap());

  // Sort components.
  _smap = PWMap{};
  Set V = scc_map.image();
  PWMap Vmap = dsbg.Vmap().restrict(V);
  PWMap mapB = rmapB.restrict(Ediff);
  PWMap mapD = rmapD.restrict(Ediff);
  PWMap Emap = dsbg.Emap().restrict(Ediff);
  _dsbg = DirectedSBG{V, Vmap, mapB, mapD, Emap};
  _original_Vmap = _dsbg.Vmap();
  _original_Emap = _dsbg.Emap();

  PWMap outside_smap = sortOutside(dsbg.Emap());
  Set fixed = outside_smap.fixedPoints();
  outside_smap = outside_smap.restrict(V.difference(fixed));

  // Convert components result to start/end points.
  PWMap to_starts = input.rmap_starts().restrict(V).inverse(); 
  PWMap starts = outside_smap.composition(to_starts);
  PWMap to_ends = input.rmap_ends().restrict(V);
  outside_smap = to_ends.composition(starts);

  _smap = std::move(inside_smap).combine(std::move(outside_smap));
  _smap = PWMap{Set{start}}.combine(std::move(_smap));
  _smap.compact();
  Util::DEBUG_LOG << "Two phased topological sort result:\n" << _smap << "\n\n";
  return _smap;
}

} // namespace detail

} // namespace LIB

} // namespace SBG
