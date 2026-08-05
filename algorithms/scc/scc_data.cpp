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

#include "algorithms/scc/scc_data.hpp"

namespace SBG {

namespace LIB {

SCCData::SCCData(DirectedSBG dsbg, PWMap rmap, Set Ediff)
  : _dsbg(dsbg), _rmap(rmap), _Ediff(Ediff) {}

const DirectedSBG& SCCData::dsbg() const { return _dsbg; }

const PWMap& SCCData::rmap() const { return _rmap; }

const Set& SCCData::Ediff() const { return _Ediff; }

} // namespace LIB

} // namespace SBG
