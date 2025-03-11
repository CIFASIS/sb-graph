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
#include <getopt.h>

#include "parser/sbg_program.hpp"
#include "util/debug.hpp"

void parseProgramFromFile(std::string fname)
{
  std::ifstream in(fname.c_str());
  if (in.fail()) 
    SBG::Debug::ERROR("Unable to open file ", fname, "\n");
  in.unsetf(std::ios::skipws);

  std::string str(
    (std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>()
  );
  SBG::Parser::StrIt iter = str.begin();
  SBG::Parser::StrIt end = str.end();

  SBG::Parser::SBGProgramRule g(iter); // Grammar
  SBG::AST::SBGProgram result;
  bool r = boost::spirit::qi::phrase_parse(
    iter, end, g, SBG::Parser::Skipper<SBG::Parser::StrIt>(), result
  );

  std::cout << "-------------------------\n";
  if (r && iter == end) {
    std::cout << "Parsing succeeded\n";
    std::cout << "-------------------------\n";
    std::cout << "\n" << result;
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
  std::cout << "    its slope, except if its value is equal to 1. That is,\n";
  std::cout << "    to express a constant expression it should be written\n";
  std::cout << "    as: 0*x+h.\n\n";

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
    parseProgramFromFile(filename);
  else
    SBG::Debug::ERROR("A filename should be provided\n");

  return 0;
}
