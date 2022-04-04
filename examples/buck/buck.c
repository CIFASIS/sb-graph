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

/**************************************************************************************************
 * model buck
 *  parameter Real C = 1e-4, L = 1e-4, R = 10, U = 24, T = 1e-4, DC = 0.5, ROn = 1e-5, ROff = 1e5;
 *  discrete Real Rd(start=1e5), Rs(start=1e-5), nextT(start=T),lastT,diodeon;
 *  Real uC,iL,iD,s;
 *
 *  equation
 *   iD=(iL*Rs-U)/(Rs+Rd);
 *   s=diodeon*iD+(1-diodeon)*iD*Rd;
 *   der(iL) = (-iD*Rd- uC)/L;
 *   der(uC) = (iL - uC/R)/C;
 *
 *  algorithm
 *   when time > nextT then
 *     lastT:=nextT;
 *     nextT:=nextT+T;
 *     Rs := ROn;
 *   end when;
 *   when time - lastT-DC*T>0 then
 *     Rs := ROff;
 *   end when;
 *   when s>0 then
 *     Rd:=ROn;
 *     diodeon:=1;
 *   elsewhen s<0 then
 *     Rd := ROff;
 *      diodeon:=0;
 *   end when;
 * end buck;
 *
 * COMPILE COMMAND:
 * g++ buck.c -o buck -lsbgraph
 *
 ***************************************************************************************************/

#include <sbg/graph_builders/matching_graph_builder.hpp>
#include <sbg/sbg_algorithms.hpp>

using namespace SBG;
using namespace SBG::IO;

int main()
{
  Equations equations;
  Variables variables;

  std::vector<std::string> state_var_names = {"iL", "uC"};
  std::vector<std::string> alg_var_names = {"iD", "s"};

  // Add model variables.
  for (std::string var_name : state_var_names) {
    VariableInfo v(var_name, std::vector<int>(), true);
    variables.push_back(v);
  }

  for (std::string var_name : alg_var_names) {
    VariableInfo v(var_name, std::vector<int>(), true);
    variables.push_back(v);
  }

  // Add model equations.

  // eq1 -> iD=(iL*Rs-U)/(Rs+Rd);
  EquationInfo eq1;
  VariableUsage var_usage_eq1;
  var_usage_eq1.set_name("iD");
  eq1.var_usage_ref().insert(var_usage_eq1);

  // eq2 -> s=diodeon*iD+(1-diodeon)*iD*Rd;
  EquationInfo eq2;
  VariableUsage var_usage_eq2_1;
  var_usage_eq2_1.set_name("s");
  VariableUsage var_usage_eq2_2;
  var_usage_eq2_2.set_name("iD");
  eq2.var_usage_ref().insert(var_usage_eq2_1);
  eq2.var_usage_ref().insert(var_usage_eq2_2);

  // eq3 -> der(iL) = (-iD*Rd- uC)/L;
  EquationInfo eq3;
  VariableUsage var_usage_eq3_1;
  var_usage_eq3_1.set_name("iL");
  VariableUsage var_usage_eq3_2;
  var_usage_eq3_2.set_name("iD");
  eq3.var_usage_ref().insert(var_usage_eq3_1);
  eq3.var_usage_ref().insert(var_usage_eq3_2);

  // eq4 -> der(uC) = (iL - uC/R)/C;
  EquationInfo eq4;
  VariableUsage var_usage_eq4_1;
  var_usage_eq4_1.set_name("uC");
  eq4.var_usage_ref().insert(var_usage_eq4_1);

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
