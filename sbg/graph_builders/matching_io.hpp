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

#pragma once

#include <ostream>

#include <sbg/util/defs.hpp>
#include <sbg/sbg.hpp>

namespace SBG {

namespace IO {

struct LinearMap {
  LinearMap();
  LinearMap(int constant, int slope, std::string iterator);

  member_class(std::string, iterator);
  member_class(int, constant);
  member_class(int, slope);

  bool operator<(const LinearMap &other) const;
};

std::ostream &operator<<(std::ostream &out, LinearMap &lm);

typedef std::vector<LinearMap> LinearMaps;

std::ostream &operator<<(std::ostream &out, LinearMaps &lms);

struct VariableUsage {
  VariableUsage();
  VariableUsage(std::string name, LinearMaps usage);

  member_class(std::string, name);
  member_class(LinearMaps, usage);

  bool operator<(const VariableUsage &other) const;
};

std::ostream &operator<<(std::ostream &out, VariableUsage &var_usage);

struct RangeDef {
  RangeDef();
  RangeDef(std::string iterator, int begin, int step, int end);

  member_class(std::string, iterator);
  member_class(int, begin);
  member_class(int, step);
  member_class(int, end);
};

std::ostream &operator<<(std::ostream &out, RangeDef &range);
std::ostream &operator<<(std::ostream &out, std::vector<RangeDef> &ranges);

struct EquationInfo {
  EquationInfo();
  EquationInfo(std::vector<RangeDef> size, std::set<VariableUsage> var_usage);

  member_class(std::vector<RangeDef>, size);
  member_class(std::set<VariableUsage>, var_usage);
};

struct VariableInfo {
  VariableInfo();
  VariableInfo(std::string name, std::vector<int> size, bool is_state);

  member_class(std::string, name);
  member_class(std::vector<int>, size);
  member_class(bool, is_state);
};

struct MatchingInfo {
  MatchingInfo();
  MatchingInfo(int eq_id, std::vector<RangeDef> eq_range, int var_id, std::vector<RangeDef> var_range);

  member_class(int, eq_id);
  member_class(std::vector<RangeDef>, eq_range);
  member_class(int, var_id);
  member_class(std::vector<RangeDef>, var_range);
};

typedef std::vector<EquationInfo> Equations;
typedef std::vector<VariableInfo> Variables;
typedef std::vector<MatchingInfo> Matching;
typedef std::map<int, int> NodeMap;

std::ostream &operator<<(std::ostream &out, MatchingInfo &matching_info);
std::ostream &operator<<(std::ostream &out, Matching &matching);

}  // namespace IO

}  // namespace SBG
