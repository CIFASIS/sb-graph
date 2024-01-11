/** @file main.cpp

 @brief <b>Boost matching profiling</b>

 This file is meant to test the three examples presented in the paper
 "Efficient Matching of Large Systems ...". When executed it will ten times
 the Boost Edmonds matching algorithm and report the average execiton time
 for each of them.

 <hr>

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
#include <fstream>
#include <getopt.h>
#include <iostream>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/max_cardinality_matching.hpp>
#include <gtest/gtest.h>

#include <eval/visitors/program_visitor.hpp>
#include <parser/sbg_program.hpp>
#include <test/performance/boost/ordinary_graph_builder.hpp>

template<typename Set>
void computeMaxCardinalityMatching(SBG::LIB::SBGraph<Set> sb_graph)
{
  OG::OrdinaryGraphBuilder ordinary_graph_builder(sb_graph);

  OG::Graph graph = ordinary_graph_builder.build();

  int n_vertices = num_vertices(graph);

  std::vector<boost::graph_traits<OG::Graph>::vertex_descriptor>
    mate(n_vertices);
  auto begin = std::chrono::high_resolution_clock::now();
  checked_edmonds_maximum_cardinality_matching(graph, &mate[0]);
  auto end = std::chrono::high_resolution_clock::now();
  auto total = std::chrono::duration_cast<std::chrono::microseconds>(
    end - begin
  );

  SBG::Util::SBG_LOG << "Boost Edmonds Maximum cardinality matching time: " 
    << total.count() << " [μs]" << std::endl;
  SBG::Util::SBG_LOG << "Mathcing sz: " << matching_size(graph, &mate[0]) << "\n";
}

template void computeMaxCardinalityMatching<SBG::LIB::UnordSet>(
  SBG::LIB::BaseSBG sbg
);
template void computeMaxCardinalityMatching<SBG::LIB::OrdSet>(
  SBG::LIB::CanonSBG sbg
);

void parseEvalProgramFromFile(std::string fname)
{
  std::ifstream in(fname.c_str());
  if (in.fail()) 
    SBG::Util::ERROR("Unable to open file");
  in.unsetf(std::ios::skipws);

  std::string str(
    (std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>()
  );
  SBG::Parser::StrIt iter = str.begin();
  SBG::Parser::StrIt end = str.end();

  SBG::Parser::SBGProgramRule g(iter); // Grammar
  SBG::AST::SBGProgram parser_result;
  bool r = boost::spirit::qi::phrase_parse(
    iter, end, g, SBG::Parser::Skipper<SBG::Parser::StrIt>(), parser_result
  );

  if (r && iter == end) {
    SBG::Eval::ProgramVisitor program_visit(false); 
    SBG::Eval::ProgramIO visit_result = boost::apply_visitor(
      program_visit, parser_result
    );

    for (const SBG::Eval::ExprEval &ev : visit_result.exprs()) {
      auto e = std::get<1>(ev);
      if (std::holds_alternative<SBG::Eval::SBGBaseType>(e)) {
        auto g_variant = std::get<SBG::Eval::SBGBaseType>(e);
        if (std::holds_alternative<SBG::LIB::CanonSBG>(g_variant)) {
          auto g = std::get<SBG::LIB::CanonSBG>(g_variant);
          computeMaxCardinalityMatching(g);
        }
      }
    }
  }
  else {
    std::string rest(iter, end);
    SBG::Util::SBG_LOG << "Parsing failed\n";
    SBG::Util::SBG_LOG << "-------------------------\n";
    SBG::Util::SBG_LOG << "\nstopped at: \n" << rest << "\n";
  }

  return;
}

void usage()
{
  std::cout << "Usage parser [options] file" << std::endl;
  std::cout << "Parses a SBG program." << std::endl;
  std::cout << std::endl;
  std::cout << "-f, --file      SBG program file used as input " << std::endl;
  std::cout << "-h, --help      Display this information and exit" << std::endl;
  std::cout << "-v, --version   Display version information and exit"
    << std::endl;
  std::cout << std::endl;
  std::cout << "SBG library home page: https://github.com/CIFASIS/sb-graph"
    << std::endl;
}

void version()
{
  std::cout << "SBG library v3.0\n";
  std::cout << "License GPLv3+: GNU GPL version 3 or later"
    << " <http://gnu.org/licenses/gpl.html>\n";
  std::cout << "This is free software: you are free to change and redistribute" 
    << "it.\n";
  std::cout << "There is NO WARRANTY, to the extent permitted by law.\n";
}

int main(int argc, char**argv)
{
  std::string filename;
  int opt;
  extern char* optarg;

  while (true) {
    static struct option long_options[] = {{"file", required_argument, 0, 'f'}
                                           , {"help", no_argument, 0, 'h'}
                                           , {"version", no_argument, 0, 'v'}
                                           , {0, 0, 0, 0}};
    opt = getopt_long(argc, argv, "f:hv", long_options, nullptr);
    if (opt == EOF) 
      break;
    switch (opt) {
      case 'f':
        filename = optarg;
        break;
      case 'h':
        usage();
        exit(0);
      case 'v':
        version();
        exit(0);
      case '?':
        usage();
        exit(-1);
        break;
      default:
        abort();
    }
  }

  if (!filename.empty())
    parseEvalProgramFromFile(filename);
  else
    SBG::Util::ERROR("A filename should be provided");

  return 0;
}
