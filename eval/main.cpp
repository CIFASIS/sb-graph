/** @file main.hpp

 @brief <b>SBG program evaluator</b>

 This modules allows the user to test the SBG modules. To do so the user should
 provide a SBG program file. The file will be parser, and next the visitors
 will be used to return a result.

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

#include <fstream>
#include <getopt.h>

#include "parser/sbg_program.hpp"
#include "eval/visitors/program_visitor.hpp"

void parseEvalProgramFromFile(std::string fname, bool debug)
{
  std::ifstream in(fname.c_str());
  if (in.fail()) 
    SBG::Util::ERROR("Unable to open file ", fname, "\n");
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

  std::cout << "-------------------------\n";
  if (r && iter == end) {
    std::cout << "Parsing succeeded\n";
    std::cout << "-------------------------\n";
    std::cout << ">>>>>> Eval result <<<<<<\n";
    std::cout << "-------------------------\n\n";

    SBG::Eval::ProgramVisitor program_visit(debug); 
    SBG::Eval::ProgramIO visit_result = boost::apply_visitor(
      program_visit, parser_result
    );
    std::cout << visit_result;
  }
  else {
    std::string rest(iter, end);
    std::cout << "Parsing failed\n";
    std::cout << "-------------------------\n";
    std::cout << "\nstopped at: \n" << rest << "\n";
  }

  return;
}

void usage()
{
  std::cout << "Usage evaluator: ./bin/sbg-eval -f filename [options]\n";
  std::cout << "Parses and evaluates a SBG program.\n\n";
  std::cout << "-f, --file      SBG program file used as input\n";
  std::cout << "-h, --help      Display this information and exit\n";
  std::cout << "-d, --debug     Activate debug info\n";
  std::cout << "-v, --version   Display version information and exit\n\n";
  std::cout << "SBG library home page: https://github.com/CIFASIS/sb-graph\n\n";

  std::cout << "To start writing a SBG program the following considerations\n";
  std::cout << "should be taken into account:\n";
  std::cout << "  * Multi-block and single line C-like comments are supported.";
  std::cout << "\n";
  std::cout << "  * A SBG program starts with a list (possibly empty) of\n";
  std::cout << "    assignments, and then continues with a list (possibly\n";
  std::cout << "    empty) of expressions.\n";
  std::cout << "  * Each assignment or expression should be ended with a\n";
  std::cout << "    semicolon ;\n";
  std::cout << "  * All expressions defined in a SBG program should have the\n";
  std::cout << "    number of dimensions.\n";
  std::cout << "  * The first assignment of a SBG program should be\n";
  std::cout << "    \"dims = k\", where k is the number of the dimensions of\n";
  std::cout << "    all elements defined in the SBG program. If it is\n";
  std::cout << "    omitted then is considered to be 1.\n";
  std::cout << "  * Numerical (and only numerical) variables can be defined.\n";
  std::cout << "    through an assignment. It is forbidden to use a variable\n";
  std::cout << "    before its definition.\n";
  std::cout << "  * Variable names should start with a letter, and then\n";
  std::cout << "    alfanumeric characters (including \"_\") can be used.\n";
  std::cout << "    The variable name \"x\" is forbidden, to preserve it\n";
  std::cout << "    for linear expressions. As explained above, \"dims\"\n";
  std::cout << "    is also reserved.\n";
  std::cout << "  * Linear expresssions should include a numeric value for\n";
  std::cout << "    its slope. That is, to express a constant expression\n";
  std::cout << "    it should be written as: 0*x+h.\n\n";
  std::cout << "  * Both intervals and linear expressions will be parsed as\n";
  std::cout << "    their multi-dimensional counterparts.\n\n";

  std::cout << "A brief list of the available expressions:\n";
  std::cout << "  * Arithmetic.\n";
  std::cout << "  * Interval.\n";
  std::cout << "  * Multi-dimensional Interval.\n";
  std::cout << "  * SBG Set.\n";
  std::cout << "  * Linear Expression.\n";
  std::cout << "  * Multi-dimensional Expression.\n";
  std::cout << "  * SBG Map.\n";
  std::cout << "  * SBG Piecewise Linear Map.\n";
  std::cout << "  * Undirected SBG.\n";
  std::cout << "  * Directed SBG.\n";
  std::cout << "  * Function Call.\n\n";

  std::cout << "For a more detailed description of the grammar, the /parser\n";
  std::cout << "files can be analyzed. Also /test files can be consulted\n";
  std::cout << "to start writing basic SBG programs.\n\n";

  std::cout << "\nA comprehensive list of the available functions to call on SBG"
    << " elements,\nwith a short description of the permitted arguments is";
  std::cout << " as follows:\n";
  std::cout << "  - isEmpty: check if container is empty\n";
  std::cout << "    --> isEmpty(interval) | isEmpty(set)\n";
  std::cout << "\n  - isMember: check if an element belongs to a container\n";
  std::cout << "    --> isMember(nat, interval)\n";
  std::cout << "\n  - minElem: minimum element of a container\n";
  std::cout << "    --> minElem(interval) | minElem(set)\n";
  std::cout << "\n  - maxElem: maximum element of a container\n";
  std::cout << "    --> maxElem(interval)  | maxElem(set)\n";
  std::cout << "\n  - compose: composition of expressions or maps\n";
  std::cout << "    --> compose(lexp, lexp) | compose(mdlexp, mdlexp)\n";
  std::cout << "    | compose(map, map) | compose(pw, pw)\n";
  std::cout << "\n  - inv: calculate the inverse of an expression.\n";
  std::cout << "    If the expression is constant in some dimension, returns\n";
  std::cout << "    inf-inf there\n";
  std::cout << "    --> inv(lexp) | inv(mdlexp)\n";
  std::cout << "\n  - image: return the image for a set of elements of the domain";
  std::cout << "\n    --> image(set, map) | image(set, pw)\n";
  std::cout << "\n  - preImage: return the pre-image for a set of elements in the\n";
  std::cout << "    image of the map\n";
  std::cout << "    --> preImage(set, map) | preImage(set, pw)\n";
  std::cout << "\n  - dom: return the whole domain of a piecewise-linear map\n";
  std::cout << "    --> dom(pw)\n";
  std::cout << "\n  - combine: return a new map with the union of the two domains\n";
  std::cout << "    mapping to the first argument for elements in its domain\n";
  std::cout << "    and to the second one for the remaining elements\n"; 
  std::cout << "    --> combine(pw, pw)\n";
  std::cout << "\n  - firstInv: calculate an inverse of a piecewise-linear map.\n";
  std::cout << "    If some image y is shared by more than one element return\n";
  std::cout << "    any of its pre-images.\n";
  std::cout << "    --> firstInv(pw)\n";
  std::cout << "\n  - minMap: return a new piecewise-linear map that maps each\n";
  std::cout << "    element in both domains to the minimum image between the\n";
  std::cout << "    two arguments\n";
  std::cout << "    --> minMap(pw1, pw2)\n";
  std::cout << "\n  - reduce: return a piecewise-linear map to which the argument\n";
  std::cout << "    converges to\n";
  std::cout << "    --> reduce(pw)\n";
  std::cout << "\n  - minAdj: given two piecewise-linear maps with the same domain\n";
  std::cout << "    for each image y = pw1(x) map to the minimum y' such that\n";
  std::cout << "    y' = pw2(x)\n";
  std::cout << "    --> minAdj(pw1, pw2)\n";
  std::cout << "\n  - mapInf: compose the argument with itself until it converges\n";
  std::cout << "    --> mapInf(pw)\n";
  std::cout << "\n  - CC: calculate the connected components of a SBG\n";
  std::cout << "    --> CC(sbg)\n";
  std::cout << "\n  - matching: calculate a maximum matching of n copies of a SBG\n";
  std::cout << "    --> matching(sbg, nat)\n";
  std::cout << "\n  - scc: calculate the strongly connected components of a DSBG\n";
  std::cout << "    --> scc(dsbg)\n";
  std::cout << "\n  - sort: calculate a topological sort a DSBG\n";
  std::cout << "    --> sort(dsbg)\n";
  std::cout << "\n  - matchSCC: calculate matching+scc for n copies of a SBG\n";
  std::cout << "    --> matchSCC(sbg, nat)\n";
  std::cout << "\n  - matchSCCTS: calculate matching+scc+sort for n copies of a SBG\n";
  std::cout << "    --> matchSCCTS(sbg, nat)\n";
  std::cout << "\n  - cut: calculate a (possibly minimum) cut set for a DSBG\n";
  std::cout << "    --> cut(dsbg)\n";
}

void version()
{
  std::cout << "SBG library v3.0.0\n";
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
  bool debug = false;

  while (true) {
    static struct option long_options[] = {{"file", required_argument, 0, 'f'}
                                           , {"help", no_argument, 0, 'h'}
                                           , {"debug", no_argument, 0, 'd'}
                                           , {"version", no_argument, 0, 'v'}
                                           , {0, 0, 0, 0}};
    opt = getopt_long(argc, argv, "f:hdv", long_options, nullptr);
    if (opt == EOF) 
      break;
    switch (opt) {
      case 'f':
        filename = optarg;
        break;
      case 'h':
        usage();
        exit(0);
      case 'd':
        debug = true;
        break;
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
    parseEvalProgramFromFile(filename, debug);
  else
    SBG::Util::ERROR("A filename should be provided\n");

  return 0;
}
