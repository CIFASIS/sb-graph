/*********************************************************************
 * Simple example of Advection model.
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
 * 
**********************************************************************/

#include <sbg/graph_builders/matching_graph_builder.h>
#include <sbg/sbg_algorithms.h>

int main()
{
    SBG::Equations equations;
    SBG::Variables variables;
    
    // Add model variables.
    SBG::VariableInfo u;
    u.name = "u";
    u.size = {20000};
    u.is_state = true;
    variables.push_back(u);

    // Add model equations.

    // eq1 -> der(u[1])=(-u[1]+1)*N-mu*u[1]*(u[1]-alpha)*(u[1]-1);
    SBG::EquationInfo eq1;
    SBG::VariableUsage var_usage_eq1;
    var_usage_eq1.name = "u";
    eq1.var_usage.insert(var_usage_eq1);

    // eq2 -> der(u[j])=(-u[j]+u[j-1])*N-mu*u[j]*(u[j]-alpha)*(u[j]-1);
    SBG::EquationInfo eq2;
    SBG::VariableUsage var_usage_eq2_1;
    var_usage_eq2_1.name = "u";
    var_usage_eq2_1.usage = {{0,1,"j"}};
    SBG::VariableUsage var_usage_eq2_2;
    var_usage_eq2_2.name = "u";
    var_usage_eq2_2.usage = {{-1,1,"j"}};
    eq2.size = {{"j", 2, 1, 20000}};
    eq2.var_usage.insert(var_usage_eq2_1);
    eq2.var_usage.insert(var_usage_eq2_2);
    
    // Insert model equations.
    equations.push_back(eq1);
    equations.push_back(eq2);

    // Build matching graph.
    SBG::MatchingGraphBuilder graph_builder(equations, variables);
    SBG::SBGraph graph = graph_builder.build();

    // Compute matching.
    MatchingStruct matching(graph);

    matching.SBGMatching();
    return 0;
}
