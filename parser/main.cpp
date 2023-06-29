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

#include <parser/sbg_program.hpp> 

void parseExprsFromFile(std::string str)
{
  SBG::Parser::StrIt iter = str.begin();
  SBG::Parser::StrIt end = str.end();

  SBG::Parser::ExprRule g(iter); // Grammar
  SBG::AST::ExprList result;
  bool r = boost::spirit::qi::phrase_parse(iter, end, g, SBG::Parser::Skipper<SBG::Parser::StrIt>(), result);

  std::cout << "-------------------------\n";

  if (r && iter == end) {
    std::cout << "Parsing succeeded\n";
    std::cout << "result = \n\n" << result << "\n\n";
  }

  else {
    std::string rest(iter, end);
    std::cout << "Parsing failed\n";
    std::cout << "stopped at: " << rest << "\"\n";
  }

  std::cout << "-------------------------\n";

  return;
}

void parseStmsFromFile(std::string str)
{
  SBG::Parser::StrIt iter = str.begin();
  SBG::Parser::StrIt end = str.end();

  SBG::Parser::StmRule g(iter); // Grammar
  SBG::AST::StatementList result;
  bool r = boost::spirit::qi::phrase_parse(iter, end, g, SBG::Parser::Skipper<SBG::Parser::StrIt>(), result);

  std::cout << "-------------------------\n";

  if (r && iter == end) {
    std::cout << "Parsing succeeded\n";
    std::cout << "result = \n\n" << result << "\n\n";
  }

  else {
    std::string rest(iter, end);
    std::cout << "Parsing failed\n";
    std::cout << "stopped at: " << rest << "\"\n";
  }

  std::cout << "-------------------------\n";

  return;
}

void parseProgramFromFile(std::string str)
{
  SBG::Parser::StrIt iter = str.begin();
  SBG::Parser::StrIt end = str.end();

  SBG::Parser::SBGProgramRule g(iter); // Grammar
  SBG::AST::SBGProgram result;
  bool r = boost::spirit::qi::phrase_parse(iter, end, g, SBG::Parser::Skipper<SBG::Parser::StrIt>(), result);

  std::cout << "-------------------------\n";

  if (r && iter == end) {
    std::cout << "Parsing succeeded\n";
    std::cout << "result = \n\n" << result << "\n\n";
  }

  else {
    std::string rest(iter, end);
    std::cout << "Parsing failed\n";
    std::cout << "stopped at: " << rest << "\"\n";
  }

  std::cout << "-------------------------\n";

  return;
}

int main(int argc, char** argv) 
{
  std::cout << "\n/////////////////////////////////////////////////////////\n\n";
  std::cout << "SBG parser\n\n";
  std::cout << "/////////////////////////////////////////////////////////\n\n";

  std::cout << "[1] Parse expressions\n";
  std::cout << "[2] Parse statements\n";
  std::cout << "[3] Parse SBG program\n";

  int opt;
  std::cout << "Select one option:\n";
  std::cin >> opt;

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
    switch (opt) {
      case 1:
        parseExprsFromFile(str);
        break;

      case 2:
        parseStmsFromFile(str);
        break;

      case 3:
        parseProgramFromFile(str);
        break;

      default:
        parseProgramFromFile(str);
        break;
    }
  } 

  else 
    std::cout << "A filename should be provided\n";

  std::cout << "Bye... :-) \n\n";

  return 0;
}
