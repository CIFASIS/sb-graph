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

#include <getopt.h>

#include "parser/parser.hpp"
#include "util/debug.hpp"

void usage()
{
  std::cout << "Usage parser: ./bin/sbg-parser -f filename [options]\n";
  std::cout << "Parses a SBG program.\n\n";
  std::cout << "-f, --file      SBG program file used as input\n";
  std::cout << "-h, --help      Display this information and exit\n";
  std::cout << "-v, --version   Display version information and exit\n\n";
  std::cout << "SBG library home page: https://github.com/CIFASIS/sb-graph\n";

  std::cout << "To start writing a SBG program the following considerations\n";
  std::cout << "should be taken into account:\n";
  std::cout << "  * Multi-block and single line C-like comments are supported.";
  std::cout << "\n";
  std::cout << "  * A SBG program starts with a list (possibly empty) of\n";
  std::cout << "    assignments, and then continues with a list (possibly\n";
  std::cout << "    empty) of expressions, each one separated by a\n";
  std::cout << "    semicolon.\n";
  std::cout << "  * Each assignment or expression should be ended with a\n";
  std::cout << "    semicolon ;\n";
  std::cout << "  * All expressions defined in a SBG program should have the\n";
  std::cout << "    same number of dimensions.\n";
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
  std::cout << "    its slope, except if its value is equal to 1. That is,\n";
  std::cout << "    to express a constant expression it should be written\n";
  std::cout << "    as: 0*x+h.\n\n";

  std::cout << "A brief list of the available expressions:\n";
  std::cout << "  * Arithmetic.\n";
  std::cout << "  * Interval.\n";
  std::cout << "  * Multi-dimensional Interval (MDI).\n";
  std::cout << "  * SBG Set.\n";
  std::cout << "  * Linear Expression.\n";
  std::cout << "  * Multi-dimensional Linear Expression (MDLE).\n";
  std::cout << "  * SBG Map.\n";
  std::cout << "  * SBG Piecewise Linear Map.\n";
  std::cout << "  * Undirected SBG.\n";
  std::cout << "  * Directed SBG.\n";
  std::cout << "  * Function Call.\n\n";

  std::cout << "A brief list of available operators:\n";
  std::cout << "  * Arithmetic expressions: +, -, *, /.\n";
  std::cout << "  * SBG expressions:\n";
  std::cout << "    - For linear expressions and maps: + and -.\n";
  std::cout << "    - For containers (intervals, MDIs, sets): /\\ \n";
  std::cout << "      (intersection), \\/ (union), \\ (difference),\n";
  std::cout << "      \' (complement).\n";
  std::cout << "  * Relational operators for any expression: <, == (equality).";
  std::cout << "\n\n";

  std::cout << "As functions definitions are not supported, there is a fixed\n";
  std::cout << "list of callable functions:\n";
  std::cout << "  * isEmpty(expr)\n";
  std::cout << "  * minElem(expr)\n";
  std::cout << "  * maxElem(expr)\n";
  std::cout << "  * compose(expr, expr)\n";
  std::cout << "  * inv(expr)\n";
  std::cout << "  * image(expr) and image(expr, expr)\n";
  std::cout << "  * preImage(expr) and preImage(expr, expr)\n";
  std::cout << "  * dom(expr)\n";
  std::cout << "  * combine(expr, expr)\n";
  std::cout << "  * firstInv(expr)\n";
  std::cout << "  * minMap(expr, expr)\n";
  std::cout << "  * reduce(expr)\n";
  std::cout << "  * minAdj(expr, expr)\n";
  std::cout << "  * mapInf(expr)\n";
  std::cout << "  * CC(expr)\n";
  std::cout << "  * matching(expr, expr)\n";
  std::cout << "  * scc(expr)\n";
  std::cout << "  * sort(expr)\n";
  std::cout << "  * matchSCC(expr, expr)\n";
  std::cout << "  * matchSCCTS(expr, expr)\n";
  std::cout << "  * cut(expr)\n";
  std::cout << "Then the evaluator will be in charge of analyzing if\n";
  std::cout << "arguments have the correct type to call the function\n\n";

  std::cout << "For a more detailed description of the grammar, the files\n";
  std::cout << "present in /parser may be of use. Examples of such programs\n";
  std::cout << "are present in the /test directory.\n\n";
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

int main(int argc, char** argv) 
{
  std::string filename;
  int opt;
  extern char* optarg;

  while (true) {
    static struct option long_options[] = {{"version", no_argument, 0, 'v'}
                                           , {"help", no_argument, 0, 'h'}
                                           , {"file", required_argument, 0, 'f'}
                                           , {0, 0, 0, 0}};
    opt = getopt_long(argc, argv, "vhf:", long_options, nullptr);
    if (opt == EOF) 
      break;
    switch (opt) {
      case 'v':
        version();
        exit(0);
      case 'h':
        usage();
        exit(0);
      case 'f':
        filename = optarg;
        break;
      case '?':
        usage();
        exit(-1);
        break;
      default:
        abort();
    }
  }

  if (!filename.empty())
    SBG::Parser::parseProgramFromFile(filename);
  else
    SBG::Util::ERROR("A filename should be provided\n");

  return 0;
}
