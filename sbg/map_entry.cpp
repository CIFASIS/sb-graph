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

#include <iostream>
#include "sbg/map_entry.hpp"

namespace SBG {

namespace LIB {

namespace Internal {

SetPerimeter calculatePerimeter(const Set& s)
{
  std::size_t ar = s.arity();
  MD_NAT min_per(ar, Inf); 
  MD_NAT max_per(ar, 0);
  
  for (const SetPiece& mdi : s) {
    MD_NAT candidate_min = mdi.minElem();
    MD_NAT candidate_max = mdi.maxElem();
    for (size_t i = 0; i < ar; ++i) {
      min_per[i] = std::min(min_per[i], candidate_min[i]);
      max_per[i] = std::max(max_per[i], candidate_max[i]);
    }
  }
  
  return {min_per, max_per};
}

bool doInt(const SetPerimeter& p1, const SetPerimeter& p2)
{
  const auto min_per_p1 = p1.first;
  const auto max_per_p1 = p1.second;
  const auto min_per_p2 = p2.first;
  const auto max_per_p2 = p2.second;
  const unsigned int arity = max_per_p1.arity();

  for (unsigned int j = 0; j < arity; ++j) {
    if (max_per_p1[j] < min_per_p2[j] || max_per_p2[j] < min_per_p1[j]) {
      return false;  // No intersection
    }
  }
  
  return true;  // Intersection detected
}

MapEntry createMapEntry(const Map& m)
{
  return {m, calculatePerimeter(m.dom())}; 
}

bool operator<(const MapEntry& mpe1, const MapEntry& mpe2) 
{
  return mpe1.second.first < mpe2.second.first;
}

void emplaceBack(OrdMapCollection& ord_pw, const MapEntry& entry)
{
  if (!entry.first.isEmpty())
    ord_pw.emplace_back(entry);
}

void emplaceBack(OrdMapCollection& ord_pw, const Map& m)
{
  if (!m.isEmpty())
    ord_pw.emplace_back(createMapEntry(m));
}

void emplaceHint(OrdMapCollection& ord_pw, const Map& m, NAT hint)
{
  if (!m.isEmpty()) {
    auto it = ord_pw.begin();
    std::advance(it, hint);
    auto end = ord_pw.end();
    MapEntry mpe = createMapEntry(m);
    while (it != end) {
      if (it->second.first < mpe.second.first) {
        ++it;
      } else {
        break;
      }
    }
    ord_pw.emplace(it, mpe);
  }
}

void advanceHint(OrdMapCollection& ord_pw, const MD_NAT crit, NAT& hint)
{
  auto it = ord_pw.begin();
  std::advance(it, hint);
  auto end = ord_pw.end();
  while (it != end) {
    if (it->second.first < crit) {
      ++it;
      ++hint;
    } else {
      break;
    }
  } 
}

} // namespace Internal

} // namespace LIB

} // namespace SBG
