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

#include <sbgraph/algorithms/sorting/topological/ts_utils.hpp>
#include "util/debug.hpp"
#include "util/logger.hpp"

#include <sbgraph/sbg/map.hpp>
#include <sbgraph/sbg/set.hpp>

namespace SBG {

namespace LIB {

// TSInput ---------------------------------------------------------------------

const DirectedSBG& TSInput::dsbg() const { return _dsbg; }

const PWMap& TSInput::rmap() const { return _rmap; }

const PWMap& TSInput::rmap_starts() const { return _rmap_starts; }

const PWMap& TSInput::rmap_ends() const { return _rmap_ends; }

void TSInput::set_dsbg(DirectedSBG dsbg) { _dsbg = dsbg; }

void TSInput::set_rmap(PWMap rmap) { _rmap = rmap; }

void TSInput::set_rmap_starts(PWMap rmap_starts) { _rmap_starts = rmap_starts; }

void TSInput::set_rmap_ends(PWMap rmap_ends) { _rmap_ends = rmap_ends; }

// SortedSets ------------------------------------------------------------------

std::size_t SortedSets::size() const { return _sets.size(); }

Set& SortedSets::operator[](std::size_t k) { return _sets[k]; }

const Set& SortedSets::operator[](std::size_t k) const { return _sets[k]; }

void SortedSets::pushBack(Set s) { _sets.push_back(s); }

void SortedSets::dropLast(int n)
{
  _sets.erase(_sets.end() - n, _sets.end());
}

Set SortedSets::accumulated() const
{
  Set result;
  for (const Set& s : _sets) {
    result = std::move(result).disjointCup(s);
  }
  return result;
}

Set SortedSets::last(int n) const
{
  Set result;
  std::size_t sz = _sets.size();
  for (int j = sz - n; j < sz; ++j) {
    result = std::move(result).disjointCup(_sets[j]);
  }
  return result;
}

int SortedSets::repetitionDepth() const
{
  Set repeated = _sets.back();
  int result = 0;
  for (int j = _sets.size() - 2; j >= 0; --j) {
    ++result;
    if (!_sets[j].intersection(repeated).isEmpty()) {
      break;
    }
  }
  return result;
}

// SortedMaps ------------------------------------------------------------------

SortedMaps::SortedMaps(PWMap Vmap) : _Vmap(Vmap) {}

std::size_t SortedMaps::size() const { return _maps.size(); }

PWMap& SortedMaps::operator[](std::size_t k) { return _maps[k]; }

const PWMap& SortedMaps::operator[](std::size_t k) const { return _maps[k]; }

void SortedMaps::pushBack(PWMap pw) { _maps.push_back(pw); }

void SortedMaps::dropLast(int n)
{
  _maps.erase(_maps.end() - n, _maps.end());
}

Set SortedMaps::accumulatedDomain() const
{
  Set result;
  for (const PWMap& pw : _maps) {
    result = std::move(result).disjointCup(pw.domain());
  }
  return result;
}

PWMap SortedMaps::last(int n) const
{
  PWMap result;
  std::size_t sz = _maps.size();
  for (int j = sz-n; j < sz; ++j) {
    result = std::move(result).concatenation(_maps[j]);
  }
  return result;
}

int SortedMaps::repetitionDepth() const
{
  PWMap repeated = _maps.back();
  int result = 0;
  for (int j = _maps.size() - 2; j >= 0; --j) {
    ++result;
    PWMap jth = _maps[j];
    for (const Map& jthm : jth) {
      Set jth_SV = _Vmap.image(jthm.domain());
      for (const Map& repeatm : repeated) {
        Set repeat_SV = _Vmap.image(repeatm.domain());
        if (!jth_SV.intersection(repeat_SV).isEmpty()
          && jthm.law() == repeatm.law()) {
          return result;
        }
      }
    }
  }
  return 0;
}

// Check sort ------------------------------------------------------------------

void checkSort(PWMap& smap, const DirectedSBG& dsbg)
{
  // Check that the order is unique.
  Set fixed_points = smap.fixedPoints();
  Util::ERROR_UNLESS(fixed_points.cardinal() == 1
    , "checkSort: the order ", smap, " has multiple starts\n");

  Set domain = smap.domain();
  Set image = smap.image();
  Set starts = domain.difference(image);
  Util::ERROR_UNLESS(starts.cardinal() == 1
    , "checkSort: the order ", smap, " has multiple ends\n");

  // Check if it is injective, that is, if is an actual order.
  Set visited_img;
  for (const Map& m : smap) {
    Set m_image = m.image().difference(fixed_points);
    if (!visited_img.intersection(m_image).isEmpty()) {
      Util::ERROR("checkSort: proposed result ", smap, " is not an order\n");
    }
    visited_img = std::move(visited_img).disjointCup(std::move(m_image));
  }

  // Check if the sort is total.
  Set V = dsbg.V();
  Util::ERROR_UNLESS(domain == V && image == V.difference(starts)
    , "checkSort: the order ", smap, " is partial\n"
    , "Not in domain: ", V.difference(domain), "\n"
    , "Not in image: ", V.difference(image), "\n");

  // Delete dependencies of constant depth.
  PWMap smap_copy = smap;
  DirectedSBG copy = dsbg;
  copy.eraseEdges(copy.mapB().preImage(fixed_points));
  for (std::size_t j = 0; j < copy.Vmap().image().cardinal(); ++j) {
    copy.eraseEdges(smap_copy.composition(copy.mapD()).equalImage(copy.mapB()));
    smap_copy = smap_copy.composition(smap);
  }

  // Check order for long dependencies.
  PWMap smap_inv = smap.restrict(copy.V().difference(fixed_points)).inverse();
  copy.foreachSetEdge([&](const IntTuple& SE) {
    Set E = copy.Emap().preImage(SE);
    PWMap mapB = copy.mapB().restrict(E);
    PWMap mapD = copy.mapD().restrict(E);

    Set init = mapB.image();
    PWMap rmap = PWMap{init}.combine(smap).mapInf();
    Set intermediate = rmap.image(mapD.image());

    rmap = PWMap{intermediate}.combine(smap_inv).mapInf();
    Set before = rmap.preImage(init);

    Util::ERROR_UNLESS(init.difference(before).isEmpty()
      , "checkSort: the order ", smap, " is partial\n");
  });
}

}  // namespace LIB

}  // namespace SBG
