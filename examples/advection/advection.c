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

/*********************************************************************
 * Simple use example of Advection model.
 * model advection
 * parameter Real alpha=0.5,mu=1000;
 * constant Integer N = 20000;
 * Real u[N];
 * initial algorithm
 * for i in 1:N/3 loop
 *   u[i]:=1;
 * end for;
 *
 * equation
 *  der(u[1])=(-u[1]+1)*N-mu*u[1]*(u[1]-alpha)*(u[1]-1);
 *  for j in 2:N loop
 *     der(u[j])=(-u[j]+u[j-1])*N-mu*u[j]*(u[j]-alpha)*(u[j]-1);
 *  end for;
 * end advection;
 *
 * COMPILE COMMAND:
 * g++ advection.c -o advection -lsbgraph
 *
 **********************************************************************/

#include <sbg/graph_builders/matching_graph_builder.hpp>
#include <sbg/sbg_algorithms.hpp>

using namespace SBG;
using namespace SBG::IO;

int main()
{
  Equations equations;
  Variables variables;

  // Add model variables.
  VariableInfo u("u", {20000}, true);
  variables.push_back(u);

  // Add model equations.

  // eq1 -> der(u[1])=(-u[1]+1)*N-mu*u[1]*(u[1]-alpha)*(u[1]-1);
  EquationInfo eq1;
  VariableUsage var_usage_eq1("u", {LinearMap(1, 0, "")});
  eq1.var_usage_ref().insert(var_usage_eq1);

  // eq2 -> der(u[j])=(-u[j]+u[j-1])*N-mu*u[j]*(u[j]-alpha)*(u[j]-1);
  EquationInfo eq2;
  VariableUsage var_usage_eq2("u", {LinearMap(0, 1, "j")});
  eq2.set_size({RangeDef("j", 2, 1, 20000)});
  eq2.var_usage_ref().insert(var_usage_eq2);

  // Insert model equations.
  equations.push_back(eq1);
  equations.push_back(eq2);

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
