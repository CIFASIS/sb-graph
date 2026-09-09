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

#include "sbg/perimeter.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Set perimeter ---------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

Perimeter::Perimeter(const IntTuple& min, const IntTuple& max)
  : _min(min), _max(max) {}

const IntTuple& Perimeter::min() const { return _min; }

const IntTuple& Perimeter::max() const { return _max; }

bool Perimeter::overlap(const Perimeter& other) const
{
  std::size_t arity = _min.arity();
  for (std::size_t j = 0; j < arity; ++j) {
    if (_max[j] < other._min[j] || other._max[j] < _min[j]) {
      return false;
    }
  }
  
  return true;
}

} // namespace LIB

} // namespace SBG
