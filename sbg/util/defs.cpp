/*****************************************************************************

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

#include <sbg/defs.hpp>

namespace SBG {

std::ostream &operator<<(std::ostream &out, const ORD_INTS &nums)
{
  out << "[";
  if (nums.size() == 1)
    out << *(nums.begin());

  else if (nums.size() > 1) {
    for (auto it = nums.begin(); std::next(it) != nums.end(); it++) out << *it << ", ";
    out << *(nums.end());
  }
  out << "]";

  return out;
}

std::ostream &operator<<(std::ostream &out, const ORD_REALS &nums)
{
  out << "[";
  if (nums.size() == 1)
    out << *(nums.begin());

  else if (nums.size() > 1) {
    for (auto it = nums.begin(); std::next(it) != nums.end(); it++) out << *it << ", ";
    out << *(nums.end());
  }
  out << "]";

  return out;
}

}  // namespace SBG
