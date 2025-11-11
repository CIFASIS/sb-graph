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

#include "sbg/map_entry.hpp"

namespace SBG {

namespace LIB {

namespace Internal {

SetPerimeter calculatePerimeter(const Set& s)
{
  MD_NAT max_per(s.arity(), 0);
  MD_NAT min_per(s.arity(), Inf); 
  
  for (const SetPiece& mdi : s) {
    MD_NAT candidate_max = mdi.maxElem();
    MD_NAT candidate_min = mdi.minElem();
    
    for (std::size_t i = 0; i < max_per.arity(); ++i) {
      max_per[i] = std::max(max_per[i], candidate_max[i]);
      min_per[i] = std::min(min_per[i], candidate_min[i]);
    }
  }
  
  return {min_per, max_per};
}

bool doInt(const SetPerimeter& p1, const SetPerimeter& p2)
{
  const auto max_per_p1 = p1.second;
  const auto min_per_p1 = p1.first;
  const auto max_per_p2 = p2.second;
  const auto min_per_p2 = p2.first;
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
  SetPerimeter sp = calculatePerimeter(m.dom());
  return {m, sp}; 
}

bool operator<(const MapEntry& mpe1, const MapEntry& mpe2) 
{
  return mpe1.second.first < mpe2.second.first;
}

void pushBack(OrdMapCollection& ord_pw, const MapEntry& m)
{
  ord_pw.emplace_back(m);
}

void pushBack(OrdMapCollection& ord_pw, const Map& m)
{
  ord_pw.emplace_back(createMapEntry(m));
}

void emplaceHint(OrdMapCollection& ord_pw, const Map& m, NAT hint)
{
  auto end = ord_pw.end();
  auto it = ord_pw.begin();
  std::advance(it, hint);
  MapEntry mpe = createMapEntry(m);
  while (it != end) {
    if (it->second.first < mpe.second.first)
      ++it;
    else 
      break;
  }
  ord_pw.insert(it, mpe);
}

void advanceHint(OrdMapCollection& ord_pw, const MD_NAT crit, NAT hint)
{
  auto end = ord_pw.end();
  auto it = ord_pw.begin();
  std::advance(it,hint);
  while (it != end){
    if (it->second.first < crit){
      ++it;
      ++hint;
    }
    else
      break;
  }  
}

} // namespace Internal

} // namespace LIB

} // namespace SBG
