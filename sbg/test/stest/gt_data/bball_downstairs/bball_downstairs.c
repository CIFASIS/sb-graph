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

/******************************************************************************
 * model bball_downstairs
 *  Real x(start=0.575),vx(start=0.5),y(start=10.5),vy;
 *  discrete Real stair(start=10),contact;
 *  equation
 *   der(y) = vy;
 *   der(vy) = -9.8 - 0.1 * vy - contact * ((y - stair) *1e6+ vy * 30);
 *   der(x) = vx;
 *   der(vx) = -0.1 * vx;
 *  algorithm
 *  when y < stair then
 *   contact := 1;
 *  elsewhen y > stair then
 *   contact := 0;
 *  end when;
 *  when x - 11 +stair > 0 then
 *   stair := stair - 1;
 *  end when;
 * end bball_downstairs;
 ******************************************************************************/

#include <sbg/graph_builders/matching_graph_builder.hpp>
#include <sbg/sbg_algorithms.hpp>

int main()
{
  SBG::Equations equations;
  SBG::Variables variables;

  std::vector<std::string> var_names = {"x", "vx", "y", "vy"};

  // Add model variables.
  for (std::string var_name : var_names) {
    SBG::VariableInfo v;
    v.name = var_name;
    v.is_state = true;
    variables.push_back(v);
  }

  // Add model equations.

  // eq1 -> der(y) = vy;
  SBG::EquationInfo eq1;
  SBG::VariableUsage var_usage_eq1_1;
  var_usage_eq1_1.name = "y";
  SBG::VariableUsage var_usage_eq1_2;
  var_usage_eq1_2.name = "vy";
  eq1.var_usage.insert(var_usage_eq1_1);
  eq1.var_usage.insert(var_usage_eq1_2);

  // eq2 -> der(vy) = -9.8 - 0.1 * vy - contact * ((y - stair) *1e6+ vy * 30);
  SBG::EquationInfo eq2;
  SBG::VariableUsage var_usage_eq2_1;
  var_usage_eq2_1.name = "vy";
  SBG::VariableUsage var_usage_eq2_2;
  var_usage_eq2_2.name = "y";
  eq2.var_usage.insert(var_usage_eq2_2);
  eq2.var_usage.insert(var_usage_eq2_1);

  // eq3 -> der(x) = vx;
  SBG::EquationInfo eq3;
  SBG::VariableUsage var_usage_eq3_1;
  var_usage_eq3_1.name = "x";
  SBG::VariableUsage var_usage_eq3_2;
  var_usage_eq3_2.name = "vx";
  eq3.var_usage.insert(var_usage_eq3_1);
  eq3.var_usage.insert(var_usage_eq3_2);

  // eq4 -> der(vx) = -0.1 * vx;
  SBG::EquationInfo eq4;
  SBG::VariableUsage var_usage_eq4_1;
  var_usage_eq4_1.name = "vx";
  eq4.var_usage.insert(var_usage_eq4_1);

  // Insert model equations.
  equations.push_back(eq1);
  equations.push_back(eq2);
  equations.push_back(eq3);
  equations.push_back(eq4);

  // Build matching graph.
  SBG::MatchingGraphBuilder graph_builder(equations, variables);
  SBG::SBGraph graph = graph_builder.build();

  // Compute matching.
  MatchingStruct matching(graph);

  matching.SBGMatching();
  return 0;
}
