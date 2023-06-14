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

/**
 * @brief Linear map that represents the use of an array expression.
 *
 * Each Linear map is used to represent a variable use, for instance,
 * the expression @f$ x[i+1] @f$ should generate a linear map
 * LinearMap(1, 1, i), where:
 *  + The first parameter is the @c constant: 1
 *  + The second parameter is the @c slope: 1
 *  + The third paramter is the @c iterator variable used in the expression: i
 *
 * The members of the structure are defined using the @c member_class macro that
 * defines the getters and setters for the method. The macros are defined in
 * @c defs.hpp
 */

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

/**
 * @brief Represents the usage of a variable in an equation,
 * for instance from the following @c Modelica equation:
 *
 * **der(u[i+1])=(-a[i+1]+1)*N-mu*u[i]*(a[i]-alpha)*(u[i]-1)**
 *
 * has 2 different variable usages:
 *  + **u[i+1]** Corresponding to the state variable @c u
 *  + **a[i+1]** and **a[i]** Corresponding to the algebraic variable @c a
 *
 * In this case, two different variable usages should be generated conataining  the correponding
 * linear maps and the name of the variable.
 *
 * The members of the structure are defined using the @c member_class macro that
 * defines the getters and setters for the method. The macros are defined in
 * @c defs.hpp
 *
 * @sa LinearMap LinearMaps
 */

struct VariableUsage {
  VariableUsage();
  VariableUsage(std::string name, LinearMaps usage);

  member_class(std::string, name);
  member_class(LinearMaps, usage);

  bool operator<(const VariableUsage &other) const;
};

typedef std::set<VariableUsage> VariableUsages;

std::ostream &operator<<(std::ostream &out, VariableUsage &var_usage);

/**
 * @brief Defines the range in which an eqaution/variable is defined.
 *
 * The range is represented with three integer numbers and the variable iterator name:
 *  + **begin** Interval start.
 *  + **step** Interval start.
 *  + **end** Interval start.
 *  + **iterator** Name of the iterator variable used in the expression.
 *
 * For example, the following Modelica for loop:
 *
 * **for i in [1:2:10] loop**
 *
 * could be represented with: **RangeDef(i, 1, 2, 10)**.
 *
 * For members definitions, see @c defs.hpp
 */

struct RangeDef {
  RangeDef();
  RangeDef(std::string iterator, int begin, int step, int end);

  member_class(std::string, iterator);
  member_class(int, begin);
  member_class(int, step);
  member_class(int, end);
};

typedef std::vector<RangeDef> Ranges;

std::ostream &operator<<(std::ostream &out, RangeDef &range);
std::ostream &operator<<(std::ostream &out, Ranges &ranges);

/**
 * @brief Store information about equation definition and variable usages.
 *
 * It is composed of a vector of @c Ranges representing the definition of
 * the range in each dimension (it could be empty for scalar values) and
 * a set of @c VariableUsages that contain all variable appearences in the expression
 * @sa VariableUsage
 *
 */

struct EquationInfo {
  EquationInfo();
  EquationInfo(Ranges size, VariableUsages var_usage);

  member_class(Ranges, size);
  member_class(VariableUsages, var_usage);
};

typedef std::vector<int> Dims;

/**
 * @brief Variable definition, basic symbols information.
 *
 */

struct VariableInfo {
  VariableInfo();
  VariableInfo(std::string name, Dims size, bool is_state);

  member_class(std::string, name);
  member_class(Dims, size);
  member_class(bool, is_state);
};

typedef std::vector<EquationInfo> Equations;

typedef std::vector<VariableInfo> Variables;

/**
 * @brief Store matching information.
 *
 * Matching info composed of:
 *  + Equation Id: given a @c Equations  vector, the id represents the index in the vector.
 *  + Equation Range: The set of equations matched.
 *  + Variable Id: The index entry of a given @c Variables vector.
 *  + Variable Range: The set of variables matched.
 *
 */

struct MatchingInfo {
  MatchingInfo();
  MatchingInfo(std::string eq_id, Ranges eq_range, std::string var_id, Ranges var_range);

  member_class(std::string, eq_id);
  member_class(Ranges, eq_range);
  member_class(std::string, var_id);
  member_class(Ranges, var_range);
};

typedef std::vector<MatchingInfo> Matching;

typedef std::map<std::string, int> NodeMap;

std::ostream &operator<<(std::ostream &out, MatchingInfo &matching_info);
std::ostream &operator<<(std::ostream &out, Matching &matching);

}  // namespace IO

}  // namespace SBG
