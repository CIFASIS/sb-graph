/** @file sbg_algorithms.hpp

 @brief <b>SBG algorithms implementation</b>

 <hr>

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

#ifndef SBG_SBGALGO_HPP
#define SBG_SBGALGO_HPP

#include "sbg/sbg.hpp"

namespace SBG {

namespace LIB {

// Connected components --------------------------------------------------------

template<typename Set>
PWMap<Set> connectedComponents(SBGraph<Set> g);

// Minimum reachable -----------------------------------------------------------

template<typename Set>
struct PathInfo {
  member_class(PWMap<Set>, succs);
  member_class(PWMap<Set>, reps);

  PathInfo();
  PathInfo(PWMap<Set> succs, PWMap<Set> reps);
};

template<typename Set>
PWMap<Set> minReach1(Set V, PWMap<Set> mapB, PWMap<Set> mapD, PWMap<Set> smap, PWMap<Set> rmap);

template<typename Set>
PWMap<Set> recursion(unsigned int n, Set ER, DSBGraph<Set> dsbg, PWMap<Set> smap, PWMap<Set> semap, PWMap<Set> rmap);

template<typename Set>
struct MinReach {
  member_class(DSBGraph<Set>, sbg);

  MinReach();
  MinReach(DSBGraph<Set> sbg);

  PathInfo<Set> calculate();
};

typedef MinReach<UnordSet> BaseMR;
typedef MinReach<OrdSet> CanonMR;

// Matching --------------------------------------------------------------------

template<typename Set>
struct SBGMatching {
  member_class(SBGraph<Set>, sbg);

  SBGMatching();
  SBGMatching(SBGraph<Set> sbg);
};

} // namespace LIB

}  // namespace SBG

#endif
