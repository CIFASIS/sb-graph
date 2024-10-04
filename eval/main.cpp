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
  std::cout << "-d, --debug     Activate debug" << std::endl;
  std::cout << "-v, --version   Display version information and exit"
    << std::endl;
  std::cout << std::endl;
  std::cout << "SBG library home page: https://github.com/CIFASIS/sb-graph"
    << std::endl;
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
    SBG::Util::ERROR("A filename should be provided");

  return 0;
}
