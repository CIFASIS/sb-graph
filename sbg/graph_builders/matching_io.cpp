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

#include <sbg/graph_builders/matching_io.hpp>

namespace SBG {

std::ostream &operator<<(std::ostream &out, LinearMap &lm)
{
  if(lm.slope == 1) {
    out << lm.iterator;
  } else if (lm.slope != 0) {
    out << lm.slope << " * " << lm.iterator;
  }
  if (lm.constant > 0) {
    out << " + " << lm.constant;
  } else if (lm.constant < 0) {
    out << lm.constant;
  }
  return out;
}

std::ostream &operator<<(std::ostream &out, LinearMaps &lms)
{
  if(lms.empty()) {
    return out;
  }
  unsigned int size = lms.size()-1;
  unsigned int i = 0;
  out << "[";
  while(i < size) {
    out << lms[i] << ",";
    i++;
  } 
  out << lms[size] << "]";
  return out;
}

std::ostream &operator<<(std::ostream &out, VariableUsage &var_usage)
{
  out << var_usage.name << var_usage.usage;
  return out;
}

}  // namespace SBG
