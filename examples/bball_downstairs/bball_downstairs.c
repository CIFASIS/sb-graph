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
 *
 * COMPILE COMMAND:
 * g++ bball_downstairs.c -o bball_downstairs -lsbgraph
 *
 ******************************************************************************/

#include <sbg/graph_builders/matching_graph_builder.hpp>
#include <sbg/sbg_algorithms.hpp>

using namespace SBG;
using namespace SBG::IO;

int main()
{
  Equations equations;
  Variables variables;

  std::vector<std::string> var_names = {"x", "vx", "y", "vy"};

  // Add model variables.
  for (std::string var_name : var_names) {
    VariableInfo v(var_name, std::vector<int>(), true);
    variables.push_back(v);
  }

  // Add model equations.

  // eq1 -> der(y) = vy;
  EquationInfo eq1;
  VariableUsage var_usage_eq1;
  var_usage_eq1.set_name("y");
  eq1.var_usage_ref().insert(var_usage_eq1);

  // eq2 -> der(vy) = -9.8 - 0.1 * vy - contact * ((y - stair) *1e6+ vy * 30);
  EquationInfo eq2;
  VariableUsage var_usage_eq2;
  var_usage_eq2.set_name("vy");
  eq2.var_usage_ref().insert(var_usage_eq2);

  // eq3 -> der(x) = vx;
  EquationInfo eq3;
  VariableUsage var_usage_eq3;
  var_usage_eq3.set_name("x");
  eq3.var_usage_ref().insert(var_usage_eq3);

  // eq4 -> der(vx) = -0.1 * vx;
  EquationInfo eq4;
  VariableUsage var_usage_eq4;
  var_usage_eq4.set_name("vx");
  eq4.var_usage_ref().insert(var_usage_eq4);

  // Insert model equations.
  equations.push_back(eq1);
  equations.push_back(eq2);
  equations.push_back(eq3);
  equations.push_back(eq4);

  // Build matching graph.
  MatchingGraphBuilder graph_builder(equations, variables);
  SBGraph graph = graph_builder.build();

  // Compute matching.
  MatchingStruct matching(graph);

  std::pair<SBG::Set, bool> res = matching.SBGMatching();
  std::cout << "Generated matching:\n";
  std::cout << get<0>(res) << "\n\n";
  if (get<1>(res)) {
    std::cout << ">>> Matched all unknowns\n";
    Matching matching_info = graph_builder.computeMatchingInfo(get<0>(res));
    std::cout << matching_info;
  }
  return 0;
}
