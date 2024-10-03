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

void parseProgramFromFile(std::string fname)
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
    SBG::Util::ERROR("A filename should be provided\n");

  return 0;
}
