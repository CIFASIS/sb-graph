/*******************************************************************************

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

#include <chrono>
#include <iostream>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/max_cardinality_matching.hpp>
#include <boost/graph/strong_components.hpp>
#include <boost/graph/topological_sort.hpp>

#include "algorithms/matching/matching_fact.hpp"
#include "algorithms/scc/scc_fact.hpp"
#include "algorithms/toposort/ts_fact.hpp"
#include "algorithms/misc/causalization_builders.hpp"
#include "eval/file_evaluator.cpp"
#include "test/performance/boost/boost_exec.hpp"
#include "test/performance/boost/ordinary_graph_builder.hpp"

namespace Boost {

namespace Internal {

void computeMaxCardinalityMatching(OG::Graph graph)
{
  int n_vertices = num_vertices(graph);

  std::vector<OG::VertexDesc> mate(n_vertices);
  auto begin = std::chrono::high_resolution_clock::now();
  checked_edmonds_maximum_cardinality_matching(graph, &mate[0]);
  auto end = std::chrono::high_resolution_clock::now();
  auto total = std::chrono::duration_cast<std::chrono::microseconds>(
    end - begin
  );

  SBG::Util::SBG_LOG << "Boost Edmonds Maximum cardinality matching time: " 
    << total.count() << " [μs]" << std::endl;
  SBG::Util::SBG_LOG << "Matching sz: " << matching_size(graph, &mate[0]) << "\n";
}

void computeSCC(OG::DGraph graph)
{
  int n_vertices = num_vertices(graph);

  std::vector<int> component(n_vertices);
  auto begin = std::chrono::high_resolution_clock::now();
  int num = strong_components(graph, &component[0]);
  auto end = std::chrono::high_resolution_clock::now();
  auto total = std::chrono::duration_cast<std::chrono::microseconds>(
    end - begin
  );

  SBG::Util::SBG_LOG << "Boost Tarjan SCC time: " 
    << total.count() << " [μs]" << std::endl;
  SBG::Util::SBG_LOG << "SCC sz: " << num << "\n";
}

void computeTS(OG::DGraph graph)
{
  std::vector<int> order;
  auto begin = std::chrono::high_resolution_clock::now();
  topological_sort(graph, std::back_inserter(order));
  auto end = std::chrono::high_resolution_clock::now();
  auto total = std::chrono::duration_cast<std::chrono::microseconds>(
    end - begin
  );

  SBG::Util::SBG_LOG << "Boost Topological Sort time: " 
    << total.count() << " [μs]" << std::endl;
}

void algorithmEvaluator(int stage, SBG::LIB::SBG g)
{
  SBG::LIB::Matching match = SBG::LIB::MATCH_FACT.createMatchAlgorithm();
  SBG::LIB::MatchData match_result = match.calculate(g);

  if (stage == 0) {
    OG::OrdinaryGraphBuilder ordinary_graph_builder(g);
    OG::Graph graph = ordinary_graph_builder.build();
    computeMaxCardinalityMatching(graph);
  }

  SBG::LIB::DSBG scc_dsbg = MISC::buildSCCFromMatching(match_result); 
  SBG::LIB::SCC scc = SBG::LIB::SCC_FACT.createSCCAlgorithm();
  SBG::LIB::SCCData scc_res = scc.calculate(scc_dsbg);
 
  if (stage == 1) {
    OG::OrdinaryDGraphBuilder ordinary_dgraph_builder(scc_dsbg);
    OG::DGraph dgraph = ordinary_dgraph_builder.build();
    computeSCC(dgraph);
  }

  SBG::LIB::DSBG ts_dsbg = MISC::buildSortFromSCC(scc_res);
  SBG::LIB::TopoSort ts = SBG::LIB::TS_FACT.createTSAlgorithm();

  if (stage == 2) {
    OG::OrdinaryDGraphBuilder ordinary_dgraph_builder(ts_dsbg);
    OG::DGraph dgraph = ordinary_dgraph_builder.build();
    computeTS(dgraph);
  }

  return;
}

} // namespace Internal

////////////////////////////////////////////////////////////////////////////////
// Evaluation Executor ---------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

BoostExecutor::BoostExecutor() : stage_(0)
{
  config_.add_options()
    ("stage,s", SBG::Util::prog_opts::value(&stage_),
      "Desired algorithm execution:"
      "\n  - 0: up to matching"
      "\n  - 1: matching + scc"
      "\n  - 2: matching + scc + topological sorting of scc");

  cmd_line_opts_.add(generic_).add(config_).add(hidden_);
  cfg_file_opts_.add(config_).add(hidden_);
  visible_.add(generic_).add(config_);
}

void BoostExecutor::execute(int arg_count, char* args[])
{
  // Command line options handling ---------------------------------------------

  SBG::Util::prog_opts::variables_map vm;
  store(SBG::Util::prog_opts::command_line_parser(arg_count, args)
    .options(cmd_line_opts_).positional(positional_).run(), vm);
  notify(vm);

  // Help handling -------------------------------------------------------------
 
  if (vm.count("help")) {
    std::cout << "Usage: filename [options]\n";
    std::cout << "Command line options are prioritized over configuration file"
      " options.";
    std::cout << visible_ << "\n";
    return;
  }

  // Version handling ----------------------------------------------------------

  if (vm.count("version")) {
    SBG::Util::version();
    return;
  }

  // Optional configuration file handling --------------------------------------
 
  if (config_file_) { 
    std::ifstream config_fs((*config_file_).c_str());
    if (config_fs) {
      store(parse_config_file(config_fs, cfg_file_opts_), vm);
      notify(vm);
    }
  }

  // Input SBG program file handling -------------------------------------------

  if (input_file_) {
    SBG::Eval::ProgramIO eval_result = SBG::Eval::parseEvalFile(*input_file_); 
    if (stage_) {
      for (const SBG::Eval::ExprResult &ev : eval_result.exprs()) {
        SBG::Eval::ExprBaseType e = std::get<1>(ev);
        if (std::holds_alternative<SBG::LIB::SBG>(e))
          Internal::algorithmEvaluator(*stage_, std::get<SBG::LIB::SBG>(e));
      }
    }
  }
  else {
    std::cout << "Usage: filename [options]\n";
    SBG::Util::ERROR("BoostExecutor: must provide a SBG program filename\n");
  }
}

} // namespace Boost
