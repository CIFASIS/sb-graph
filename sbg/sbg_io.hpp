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

#include <sbg/sbg.hpp>

namespace SBG {

struct LinearMap {
  int constant;
  int slope;
  std::string iterator;
  bool operator<(const LinearMap& other) const
  {
    if (constant == other.constant) {
      return slope < other.slope;
    }
    return constant < other.constant;
  }
};

typedef std::vector<LinearMap> LinearMaps;

struct VariableUsage {
  std::string name;
  LinearMaps usage;
  bool operator<(const VariableUsage& other) const
  {
    if (name == other.name) {
      for (unsigned int dim = 0; dim < usage.size(); dim++) {
        if (usage[dim] < other.usage[dim]) return true;
      }
      return false;
    }
    return name < other.name;
  }
};

struct RangeDef {
  std::string iterator;
  int begin;
  int step;
  int end;
};

struct EquationInfo {
  std::vector<RangeDef> size;
  std::set<VariableUsage> var_usage;
};

struct VariableInfo {
  std::string name;
  std::vector<int> size;
  bool is_state;
};

typedef std::list<EquationInfo> Equations;
typedef std::list<VariableInfo> Variables;

}  // namespace SBG
