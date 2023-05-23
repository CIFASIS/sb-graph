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
#include <fstream>
#include <iostream>
#include <string>

#include <parser/comp_converter.hpp>
#include <sbg/pw_map.hpp>
#include <sbg/sbg_algorithms.hpp>
#include <sbg/graph_builders/scc_graph_builder.hpp>

void parse_build() 
{
  Parser::comp_sbg_parser g; // Our grammar

  std::cout << "Type filename: ";
  std::string fname;
  std::cin >> fname;

  if (fname != "") {
    std::ifstream in(fname.c_str());
    if (in.fail()) {
      std::cerr << "Unable to open file " << fname << std::endl;
      exit(-1);
    }
    in.unsetf(std::ios::skipws);

    std::string str((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();

    Parser::CompSetGraph result;
    bool r = boost::spirit::qi::phrase_parse(iter, end, g, boost::spirit::ascii::space, result);

    Parser::CompConverter c(result);
    Parser::Grph g = c.convertGraph();

    MatchingStruct m(boost::get<SBG::SBGraph>(g));
    m.SBGMatching();
    SCCGraphBuilder graph_builder(m);
    graph_builder.build();
    DSBGraph dg = graph_builder.result();
 
    std::cout << "-------------------------\n";

    if (r && iter == end) {
      std::cout << "Parsing succeeded\n";
      std::cout << "SCC Builder result = \n\n";
      graph_builder.pretty_print();
    }

    else {
      std::string rest(iter, end);
      std::cout << "Parsing failed\n";
      std::cout << "stopped at: \": " << rest << "\"\n";
    }

    std::cout << "-------------------------\n";
  } 

  else 
    std::cout << "A filename should be provided\n";

  std::cout << "Bye... :-) \n\n";
}

void parse_scc() 
{
  Parser::comp_sbg_parser g; // Our grammar

  std::cout << "Type filename: ";
  std::string fname;
  std::cin >> fname;

  if (fname != "") {
    std::ifstream in(fname.c_str());
    if (in.fail()) {
      std::cerr << "Unable to open file " << fname << std::endl;
      exit(-1);
    }
    in.unsetf(std::ios::skipws);

    std::string str((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();

    Parser::CompSetGraph result;
    bool r = boost::spirit::qi::phrase_parse(iter, end, g, boost::spirit::ascii::space, result);

    Parser::CompConverter c(result);
    Parser::Grph g = c.convertGraph();

    DSBGraph aux_g = boost::get<DSBGraph>(g);
    SCCStruct s(aux_g);
    PWLMap rmap = s.SBGSCC();

    SBG::IO::DirectedConverter dc(aux_g);
    dc.convert_graph();
    SBG::IO::RMapIO result_io = dc.convert_map(rmap);

    std::cout << "-------------------------\n";

    if (r && iter == end) {
      std::cout << "Parsing succeeded\n";
      std::cout << "SCC result = \n\n" << result_io << "\n";
    }

    else {
      std::string rest(iter, end);
      std::cout << "Parsing failed\n";
      std::cout << "stopped at: \": " << rest << "\"\n";
    }

    std::cout << "-------------------------\n";
  } 

  else 
    std::cout << "A filename should be provided\n";

  std::cout << "Bye... :-) \n\n";
}

void parse_build_scc()
{
  Parser::comp_sbg_parser g; // Our grammar

  std::cout << "Type filename: ";
  std::string fname;
  std::cin >> fname;

  if (fname != "") {
    std::ifstream in(fname.c_str());
    if (in.fail()) {
      std::cerr << "Unable to open file " << fname << std::endl;
      exit(-1);
    }
    in.unsetf(std::ios::skipws);

    std::string str((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();

    Parser::CompSetGraph result;
    bool r = boost::spirit::qi::phrase_parse(iter, end, g, boost::spirit::ascii::space, result);

    std::cout << "-------------------------\n";
    if (r && iter == end)
      std::cout << "Parsing succeeded\n\n";

    else {
      std::string rest(iter, end);
      std::cout << "Parsing failed\n";
      std::cout << "stopped at: \": " << rest << "\"\n";
    }

    Parser::CompConverter c(result);
    Parser::Grph g = c.convertGraph();

    MatchingStruct m(boost::get<SBGraph>(g));
    m.SBGMatching();
    SCCGraphBuilder graph_builder(m);
    graph_builder.build();
    DSBGraph dg = graph_builder.result();

    std::cout << "SCC Builder result = \n\n";
    graph_builder.pretty_print();
  
    SCCStruct scc(dg);
    PWLMap rmap = scc.SBGSCC();

    SBG::IO::DirectedConverter dc(dg);
    dc.convert_graph();
    SBG::IO::RMapIO result_io = dc.convert_map(rmap);

    std::cout << "SCC result = \n\n" << result_io << "\n";

    std::cout << "-------------------------\n";
  } 

  else 
    std::cout << "A filename should be provided\n";

  std::cout << "Bye... :-) \n\n";
}

int main(int argc, char** argv) 
{
  std::cout << "\n/////////////////////////////////////////////////////////\n\n";
  std::cout << "SCC test\n\n";
  std::cout << "/////////////////////////////////////////////////////////\n\n";

  std::cout << "[1] Parse bipartite graph + Matching algorithm + Build directed graph\n";
  std::cout << "[2] Parse directed graph + SCC algorithm\n";
  std::cout << "[3] Parse bipartite graph + Matching algorithm + Build directed graph + SCC algorithm\n";

  int opt;
  std::cout << "Select one option:\n";
  std::cin >> opt;

  switch (opt) {
    case 1:
      parse_build();
      break;

    case 2:
      parse_scc();
      break;

    case 3:
      parse_build_scc();
      break;
  }

  return 0;
}
