/** @file main.hpp

 @brief <b>SBG program evaluator</b>

 These modules allows the user to test the SBG modules. To do so the user should
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
#include "eval/visitors/autom_impl_visitor.hpp"

void parseEvalProgramFromFile(std::string fname, SBG::Eval::Impl impl
  , bool debug)
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

    SBG::Eval::AutomImplVisitor impl_visit(impl);
    SBG::Eval::Impl autom_impl
      = boost::apply_visitor(impl_visit, parser_result);

    std::shared_ptr<SBG::LIB::SetAF> set_fact 
      = std::make_shared<SBG::LIB::UnordAF>();

    switch (autom_impl.set_impl()) {
      case 0:
        break;

      case 2:
        set_fact = std::make_shared<SBG::LIB::OrdDenseAF>();

      default:
        break;
    }

    SBG::LIB::MapAF map_fact(*set_fact);
    std::shared_ptr<SBG::LIB::PWMapAF> fact
      = std::make_shared<SBG::LIB::UnordPWMapAF>(map_fact);
    switch (autom_impl.pw_impl()) {
      default:
        break;
    }

    SBG::Eval::ProgramVisitor program_visit(*fact, debug); 
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
  std::cout << "-s, --set_impl  Choose set implementation: 0 unordered sets,\n";
  std::cout << "                1 ordered sets, 2 ordered dense sets. If no\n";
  std::cout << "                option is selected, the evaluator decides\n";
  std::cout << "                automatically\n";
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
  std::cout << "    empty) of expressions, each one separated by a\n";
  std::cout << "    semicolon.";
  std::cout << "  * Each assignment or expression should be ended with a\n";
  std::cout << "    semicolon ;.\n";
  std::cout << "  * All expressions defined in a SBG program should have the\n";
  std::cout << "    same number of dimensions.\n";
  std::cout << "  * The first assignment of a SBG program should be\n";
  std::cout << "    \"dims = k\", where k is the number of the dimensions of\n";
  std::cout << "    all elements defined in the SBG program. If it is\n";
  std::cout << "    omitted then is considered to be 1.\n";
  std::cout << "  * It is forbidden to use a variable before its definition.\n";
  std::cout << "  * Variable names should start with a letter, and then\n";
  std::cout << "    alfanumeric characters (including \"_\") can be used.\n";
  std::cout << "    The variable name \"x\" is forbidden, to preserve it\n";
  std::cout << "    for linear expressions. As explained above, \"dims\"\n";
  std::cout << "    is also reserved.\n";
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

  std::cout << "A brief list of available operators:\n";
  std::cout << "  * Arithmetic expressions: +, -, *, /.\n";
  std::cout << "  * SBG expressions:\n";
  std::cout << "    - For linear expressions and maps: + and -.\n";
  std::cout << "    - For containers (intervals, MDIs, sets): /\\ \n";
  std::cout << "      (intersection), \\/ (union), \\ (difference),\n";
  std::cout << "      \' (complement).\n";
  std::cout << "  * Relational operators for any expression: <, == (equality).";
  std::cout << "\n\n";

  std::cout << "\nA comprehensive list of the available functions to call on"
    << " SBG elements,\nwith a short description of the correct types for"
    << " arguments is as follows\n(a container is either an interval, a mdi or"
    << " a set):\n\n";
  std::cout << "  - isEmpty: check if container is empty\n";
  std::cout << "    --> isEmpty(container)\n";
  std::cout << "\n  - minElem: minimum element of a container\n";
  std::cout << "    --> minElem(container)\n";
  std::cout << "\n  - maxElem: maximum element of a container\n";
  std::cout << "    --> maxElem(container)\n";
  std::cout << "\n  - compose: composition of expressions or maps\n";
  std::cout << "    --> compose(lexp, lexp) | compose(mdlexp, mdlexp)\n";
  std::cout << "    | compose(map, map) | compose(pw, pw)\n";
  std::cout << "\n  - inv: calculate the inverse of an expression or map.\n";
  std::cout << "    If the expression is constant in some dimension, returns\n";
  std::cout << "    inf-inf there. If the argument is a map, it should be\n";
  std::cout << "    bijective.\n";
  std::cout << "    --> inv(lexp) | inv(mdlexp) | inv(map) | inv(pw)\n";
  std::cout << "\n  - image: return the image for a set of elements of the\n";
  std::cout << "    domain. If no subdomain is specified, the whole domain\n";
  std::cout << "    is used.";
  std::cout << "\n    --> image(map) | image(pw) | image(set, map) ";
  std::cout << "| image(set, pw)\n";
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
  std::cout << "SBG library v4.0.0" << std::endl;
  std::cout << "License GPLv3+: GNU GPL version 3 or later"
    << " <http://gnu.org/licenses/gpl.html>\n";
  std::cout << "This is free software: you are free to change and redistribute" 
    << "it.\n";
  std::cout << "There is NO WARRANTY, to the extent permitted by law.\n";
}


int main(int argc, char**argv)
{
  std::cout << std::boolalpha;

  std::string filename;
  int opt, set_impl = -1, pw_impl = -1;
  extern char* optarg;
  bool debug = false;

  while (true) {
    static struct option long_options[] = {{"file", required_argument, 0, 'f'}
                                           , {"set_impl", required_argument, 0, 's'}
                                           , {"help", no_argument, 0, 'h'}
                                           , {"debug", no_argument, 0, 'd'}
                                           , {"version", no_argument, 0, 'v'}
                                           , {0, 0, 0, 0}};
    opt = getopt_long(argc, argv, "f:s:hdv", long_options, nullptr);
    if (opt == EOF) 
      break;
    switch (opt) {
      case 'f':
        filename = optarg;
        break;
      case 's':
        set_impl = std::stoi(optarg);
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
    parseEvalProgramFromFile(filename, SBG::Eval::Impl(set_impl, pw_impl)
      , debug);
  else
    SBG::Util::ERROR("A filename should be provided\n");

  return 0;
}
