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
#include <sbg/graph_builders/order_graph_builder.hpp>

void print_result(MatchingStruct m, SCCStruct scc, OrderStruct o, VertexOrder vo)
{
  SBG::IO::UndirectedConverter uc(m.g());
  SBG::IO::GraphIO g_io = uc.convert_graph();
  BOOST_FOREACH (Set s, vo) {
    BOOST_FOREACH (MultiInterval mi, s.asets()) {
      SBG::Set represented = scc.rmap().preImage(Set(mi));
      
      SBG::IO::MatchingIO s_io = uc.convert_matching(represented);
      std::cout << s_io << "\n";
    }
  }

  return;
}

void parse_order() 
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
    OrderStruct o(aux_g);
    VertexOrder vo = o.order();

    SBG::IO::DirectedConverter dc(aux_g);
    dc.convert_graph();
    SBG::IO::VertexOrderIO result_io = dc.convert_vertex_order(vo);

    std::cout << "-------------------------\n";

    if (r && iter == end) {
      std::cout << "Parsing succeeded\n";
      std::cout << "Resulting graph:\n\n" << aux_g << "\n";
      std::cout << "Order result = \n\n" << result_io << "\n";
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

    std::cout << "-------------------------\n";


    if (r && iter == end) {
      Parser::CompConverter c(result);
      Parser::Grph g = c.convertGraph();

      DSBGraph aux_g = boost::get<DSBGraph>(g);

      SCCStruct scc(aux_g);
      PWLMap rmap = scc.SBGSCC();

      OrderGraphBuilder o(scc);
      o.build();
      DSBGraph dg = o.result();

      OrderStruct order_struct(dg);
      VertexOrder vo = order_struct.order();

      SBG::IO::DirectedConverter dc(dg);
      SBG::IO::GraphIO g_io = dc.convert_graph();
      SBG::IO::VertexOrderIO result_io = dc.convert_vertex_order(vo);

      std::cout << "Parsing succeeded\n";
      std::cout << "Resulting graph:\n\n" << g_io << "\n";
      std::cout << "Resulting order:\n\n" << result_io << "\n"; 
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

void parse_causalize()
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
    if (r && iter == end) {
      std::cout << "Parsing succeeded\n\n";

      Parser::CompConverter c(result);
      Parser::Grph g = c.convertGraph();

      MatchingStruct m(boost::get<SBGraph>(g));
      m.SBGMatching();
      SBG::SCCGraphBuilder graph_builder(m);
      graph_builder.build();
      SBG::DSBGraph scc_dg = graph_builder.result();

      SCCStruct scc(scc_dg);
      scc.SBGSCC();

      SBG::OrderGraphBuilder o(scc);
      o.build();
      SBG::DSBGraph order_dg = o.result();

      OrderStruct order_struct(order_dg);
      SBG::VertexOrder vo = order_struct.order();

      std::cout << "Causalize result = \n\n";
      print_result(m, scc, order_struct, vo);
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


int main(int argc, char** argv) 
{
  std::cout << "\n/////////////////////////////////////////////////////////\n\n";
  std::cout << "Order test\n\n";
  std::cout << "/////////////////////////////////////////////////////////\n\n";

  std::cout << "[1] Parse directed graph + Order algorithm\n";
  std::cout << "[2] Parse directed graph + SCC algorithm + Build graph between representants\n";
  std::cout << "[3] Parse bipartite undirected graph + Causalize\n";

  int opt;
  std::cout << "Select one option:\n";
  std::cin >> opt;

  switch (opt) {
    case 1:
      parse_order();
      break;

    case 2:
      parse_build();
      break;

    case 3:
      parse_causalize();
      break;
  }

  return 0;
}
