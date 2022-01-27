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
***************************************************************************************************/

#include <sbg/graph_builders/matching_graph_builder.h>
#include <sbg/sbg_algorithms.h>

int main()
{
    SBG::Equations equations;
    SBG::Variables variables;

    std::vector<std::string> state_var_names = { "iL", "uC"};
    std::vector<std::string> alg_var_names = { "iD", "s"};

    // Add model variables.
    for (std::string var_name : state_var_names) {
        SBG::VariableInfo v;
        v.name = var_name;
        v.is_state = true;
        variables.push_back(v);
    }

    for (std::string var_name : alg_var_names) {
        SBG::VariableInfo v;
        v.name = var_name;
        v.is_state = false;
        variables.push_back(v);
    }

    // Add model equations.
 
    // eq1 -> iD=(iL*Rs-U)/(Rs+Rd);
    SBG::EquationInfo eq1;
    SBG::VariableUsage var_usage_eq1_1;
    var_usage_eq1_1.name = "iD";
    SBG::VariableUsage var_usage_eq1_2;
    var_usage_eq1_2.name = "iL";
    eq1.var_usage.insert(var_usage_eq1_1);
    eq1.var_usage.insert(var_usage_eq1_2);

    // eq2 -> s=diodeon*iD+(1-diodeon)*iD*Rd;
    SBG::EquationInfo eq2;
    SBG::VariableUsage var_usage_eq2_1;
    var_usage_eq2_1.name = "s";
    SBG::VariableUsage var_usage_eq2_2;
    var_usage_eq2_2.name = "iD";
    eq2.var_usage.insert(var_usage_eq2_1);
    eq2.var_usage.insert(var_usage_eq2_2);

    // eq3 -> der(iL) = (-iD*Rd- uC)/L;
    SBG::EquationInfo eq3;
    SBG::VariableUsage var_usage_eq3_1;
    var_usage_eq3_1.name = "iL";
    SBG::VariableUsage var_usage_eq3_2;
    var_usage_eq3_2.name = "iD";
    eq3.var_usage.insert(var_usage_eq3_1);
    eq3.var_usage.insert(var_usage_eq3_2);

    // eq4 -> der(uC) = (iL - uC/R)/C;
    SBG::EquationInfo eq4;
    SBG::VariableUsage var_usage_eq4_1;
    var_usage_eq4_1.name = "uC";
    SBG::VariableUsage var_usage_eq4_2;
    var_usage_eq4_2.name = "iD";
    eq4.var_usage.insert(var_usage_eq4_1);
    eq4.var_usage.insert(var_usage_eq4_2);

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
