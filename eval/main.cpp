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
  std::cout << "Usage evaluator [options] file" << std::endl;
  std::cout << "Parses a SBG program." << std::endl;
  std::cout << std::endl;
  std::cout << "-f, --file      SBG program file used as input " << std::endl;
  std::cout << "-h, --help      Display this information and exit" << std::endl;
  std::cout << "-d, --debug     Activate debug info" << std::endl;
  std::cout << "-v, --version   Display version information and exit"
    << std::endl;
  std::cout << std::endl;
  std::cout << "SBG library home page: https://github.com/CIFASIS/sb-graph"
    << std::endl;

  std::cout << "\nThe grammar for a SBG program can be derived from the .cpp\n"
    << " files present in /parser.\n";
  std::cout << "The following rules are used to avoid ambiguities:\n";
  std::cout << "  * Naturals are evaluated into md_nat (with arity equal 1).\n";
  std::cout << "  * Since a natural can be parsed both into an\n";
  std::cout << "    an interval or a mdi interval with arity equal to 1, it\n";
  std::cout << "    will be interpreted as an interval.\n";
  std::cout << "    To test operations of mdis, single elements sets can be\n";
  std::cout << "    used, i.e.: {[1:1:10] x [1:1:10]} /\\ {[5:1:10] x [5:1:10]}.";
  std::cout << "\n  * The same situation arises for le and mdle, for which the\n";
  std::cout << "    same criteria is used.\n";
  std::cout << "  * In a file all elements should be of the same arity (i.e., \n";
  std::cout << "    have the same number of dimensions).\n";

  std::cout << "\n\nA comprehensive list of the available functions to call on SBG"
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
  std::cout << "SBG library v3.0.0" << std::endl;
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
