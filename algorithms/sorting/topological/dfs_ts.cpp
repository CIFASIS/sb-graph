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

void DFS_TS::addSV(MD_NAT from_sv, MD_NAT from, MD_NAT to_sv, MD_NAT to, Expression expr) {
  // Set last_v
  Set d_to = _Vmap.preImage(to_sv);
  Set last = Set{d_to.maxElem()};
  d_to = d_to.difference(last).difference(_visitedSV);
  if (d_to.isEmpty()) {
    d_to = Set{to};
  }
  _last_v = d_to.maxElem();
  //
  Set sv{from_sv};
  Set d = _Vmap.preImage(sv);
  MD_NAT max_v = d.maxElem();
  if (max_v == from) {
    d = Set{from};
  }
  else {
    d = d.difference(Set{max_v});
  }
  _visitedSV = _visitedSV.cup(d);
  _smap.emplace(d, expr);
}

void DFS_TS::checkRepetitionAndAdd(MD_NAT from, MD_NAT to, bool isSucc) {
  Expression expr{from, to};
  MD_NAT from_sv = _Vmap.image(from).minElem();
  MD_NAT to_sv = _Vmap.image(to).minElem();
  SVExprTuple tuple = std::make_tuple(from_sv, to_sv, expr);

  if (std::find(_SVExprVector.begin(), _SVExprVector.end(), tuple) == _SVExprVector.end()) {
    _SVExprVector.push_back(tuple);
    Set sv{from};
    _visitedSV = _visitedSV.cup(sv);
    _last_v = to;
    _smap.emplace(sv, expr);
    return;
  }

  addSV(from_sv, from, to_sv, to, expr);

  if (!isSucc) {
    return;
  }
  for (auto it = _SVExprVector.begin(); it != _SVExprVector.end(); ++it) {
    SVExprTuple t = *it;
    if (std::get<To>(t) == from_sv) {
      addSV(std::get<From>(t), std::get<From>(t), std::get<To>(t), std::get<To>(t),std::get<Expr>(t));
    }
  }
}

MD_NAT DFS_TS::dfs(MD_NAT v, bool isRec) {
  Set sv{v};
  Set succ = _mapD.image(_mapB.preImage(sv));
  if (!isRec) {
    succ = succ.difference(_visitedSV);
  }
  if (succ.isEmpty()) {
    _visitedSV = _visitedSV.cup(sv);
    return v;
  }

  MD_NAT _v = v;
  MD_NAT last_v;
  do {
    MD_NAT succ_v = succ.minElem();
    Set succ_sv{succ_v};
    last_v = succ_v;

    checkRepetitionAndAdd(_v, succ_v, true);

    dfs(succ_v, true);
    
    _v = succ_v;
    sv = succ_sv;
    succ = succ.difference(_visitedSV);
  } while (!succ.isEmpty());

  return last_v;
}

PWMap DFS_TS::calculate(const DirectedSBG& dsbg
  , const PWMap& scc_map)
{
  Util::DEBUG_LOG << "Topological sort dsbg:\n" << dsbg << "\n\n";

  init(dsbg);

  if (_V.isEmpty()) {
    return _smap;
  }

  MD_NAT v = _V.minElem();
  _last_v = v;
  dfs(v, false);
  _V = _V.difference(_visitedSV);
  
  while (!_V.isEmpty()) {
    v = _V.minElem();
    Set v_sv{v};
    dfs(v, false);
    checkRepetitionAndAdd(_last_v, v, false);
    _V = _V.difference(_visitedSV);
  } 

  Expression e{_last_v, _last_v};
  _smap.emplace(Set{_last_v}, e);
  _smap.compact();

  Util::DEBUG_LOG << "Topological sort result:\n" << _smap << "\n\n";
  checkSort(_smap, dsbg);
  return _smap;
}

} // namespace detail

} // namespace LIB

} // namespace SBG
