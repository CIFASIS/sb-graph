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

#include <sbgraph/detail/algorithms/sorting/topological/ts_utils.hpp>
#include "util/debug.hpp"
#include "util/logger.hpp"

#include <sbgraph/sbg/map.hpp>
#include <sbgraph/sbg/set.hpp>

namespace SBG {

namespace LIB {

std::size_t SortedSets::size() const { return _sets.size(); }

Set& SortedSets::operator[](std::size_t k) { return _sets[k]; }

const Set& SortedSets::operator[](std::size_t k) const { return _sets[k]; }

void SortedSets::pushBack(Set s) { _sets.push_back(s); }

Set SortedSets::accumulated() const
{
  Set result;
  for (const Set& s : _sets) {
    result = std::move(result).disjointCup(s);
  }
  return result;
}

Set SortedSets::accumulated(int n) const
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

void checkSort(PWMap& smap, const DirectedSBG& dsbg)
{
  // Check that the order is unique.
  Set fixed_points = smap.fixedPoints();
  Util::ERROR_UNLESS(fixed_points.cardinal() == 1, "checkSort: the order ", smap, " has multiple ends\n");

  Set domain = smap.domain();
  Set image = smap.image();
  Set starts = domain.difference(image);
  Util::ERROR_UNLESS(starts.cardinal() == 1, "checkSort: the order ", smap, " has multiple starts\n");

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
  Util::ERROR_UNLESS(domain == V && image == V.difference(starts), "checkSort: the order ", smap, " is partial\n");
}

}  // namespace LIB

}  // namespace SBG
