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

#include "algorithms/sorting/topological/dfs_ts.hpp"
#include "sbg/natural.hpp"
#include "util/debug.hpp"
#include "util/logger.hpp"
#include <iostream>

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Minimum Vertex Topological Sort Algorithm -----------------------------------
////////////////////////////////////////////////////////////////////////////////

DFS_TS::DFS_TS()
  : _smap(), _Vmap(), _mapB(), _mapD(), _dsbg(), _visitedSV() {}

// Algorithm -------------------------------------------------------------------

void DFS_TS::init(const DirectedSBG& dsbg) {
  _V = dsbg.V();
  _dsbg = dsbg;
  _smap = PWMap{};
  _Vmap = _dsbg.Vmap();
  _mapB = _dsbg.mapB();
  _mapD = _dsbg.mapD();
}

/**
 * @brief Checks that the returned map is a topological sort of the dsbg. 
 */
void checkSort(PWMap& smap, const DirectedSBG& dsbg)
{
  // Check if it is injective, that is, if is an actual order.
  Set fixed_points = smap.fixedPoints();
  Set visited_img;
  for (const Map& m : smap) {
    Set m_image = m.image().difference(fixed_points);
    if (!visited_img.intersection(m_image).isEmpty()) {
      Util::ERROR("checkSort: proposed result is not an order\n");
    }
    visited_img = std::move(visited_img).disjointCup(std::move(m_image));
  }

  // Check if the sort is total.
  Util::ERROR_UNLESS(smap.domain() == dsbg.V()
    , "checkSort: the order is partial\n");
}

Set DFS_TS::checkRepetition(MD_NAT from, MD_NAT to, Expression expr) {
  MD_NAT from_sv = _Vmap.image(from).minElem();
  MD_NAT to_sv = _Vmap.image(to).minElem();
  SVExprTuple tuple = std::make_tuple(from_sv, to_sv, expr);

  if (!_SVExprSet.count(tuple)) {
    _SVExprSet.insert(tuple);
    return Set{from};
  }
  Set sv{std::get<From>(tuple)};
  Set d = _Vmap.preImage(sv);
  Set last = Set{d.maxElem()};
  d = d.difference(last).difference(_visitedSV);
  _visitedSV.cup(d);
  return d;
}

void DFS_TS::dfs(MD_NAT v) {

  Set sv{v};
  Set prev = _mapB.image(_mapD.preImage(sv));

  if (prev.isEmpty()) {
    Expression e{v, v};
    MD_NAT sv_id = _Vmap.image(sv).minElem();
    Set d = checkRepetition(v, v, e);
    _smap.emplace(d, e);
    return;
  }

  MD_NAT _v = v;
  do {
    MD_NAT prev_v = prev.minElem();
    Expression e{_v, prev_v};
    Set prev_sv{prev_v};
    MD_NAT prev_sv_id = _Vmap.image(sv).minElem();
    
    Set d = checkRepetition(v, prev_v, e);
    _smap.emplace(d, e);

    if (_visitedSV.intersection(prev_sv).isEmpty()) {
      dfs(prev_v);
    }
    _v = prev_v;
    sv = prev_sv;
    prev = prev.difference(_visitedSV);
  } while (!prev.isEmpty());
}

PWMap DFS_TS::calculate(const DirectedSBG& dsbg
  , const PWMap& scc_map)
{
  Util::DEBUG_LOG << "Topological sort dsbg:\n" << dsbg << "\n\n";

  init(dsbg);

  if (_V.isEmpty()) {
    return _smap;
  }

  do {
    MD_NAT v = _V.minElem();
    Set v_sv{v};
    if (_visitedSV.intersection(v_sv).isEmpty()) {
      dfs(v);
    }
    _V = _V.difference(_visitedSV);
  } while (!_V.isEmpty());

  _smap.compact();

  Util::DEBUG_LOG << "Topological sort result:\n" << _smap << "\n\n";
  checkSort(_smap, dsbg);
  return _smap;
}

} // namespace detail

} // namespace LIB

} // namespace SBG
