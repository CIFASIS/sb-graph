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

#include "eval/visitors/program_visitor.hpp"
#include "parser/sbg_program.hpp"
#include "util/logger.hpp"

void parseProgramFromFile(std::string str)
{
  SBG::Parser::StrIt iter = str.begin();
  SBG::Parser::StrIt end = str.end();

  SBG::Parser::SBGProgramRule g(iter); // Grammar
  SBG::AST::SBGProgram parser_result;
  bool r = boost::spirit::qi::phrase_parse(iter, end, g, SBG::Parser::Skipper<SBG::Parser::StrIt>(), parser_result);

  SBG::Util::SBG_LOG << "-------------------------\n";

  if (r && iter == end) {
    SBG::Util::SBG_LOG << "Parsing succeeded\n";

    SBG::Util::SBG_LOG << "-------------------------\n";
    SBG::Util::SBG_LOG << ">>>>>> Eval result <<<<<<\n";
    SBG::Util::SBG_LOG << "-------------------------\n";

    SBG::Eval::ProgramVisitor program_visit; 
    SBG::Eval::ProgramIO visit_result = Apply(program_visit, parser_result);

    SBG::Util::SBG_LOG << "Visit result = \n\n" << visit_result;
  }

  else {
    std::string rest(iter, end);
    SBG::Util::SBG_LOG << "Parsing failed\n";
    SBG::Util::SBG_LOG << "stopped at: " << rest << "\"\n";
  }

  SBG::Util::SBG_LOG << "-------------------------\n";

  return;
}

int main(int argc, char** argv) 
{
  std::string fname = "../../../map.test";

  if (fname != "") {
    std::ifstream in(fname.c_str());
    if (in.fail()) {
      std::cerr << "Unable to open file " << fname << std::endl;
      exit(-1);
    }
    in.unsetf(std::ios::skipws);

    std::string str((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    parseProgramFromFile(str);
  } 

  else 
    SBG::Util::SBG_LOG << "A filename should be provided\n";

  SBG::Util::SBG_LOG << "Bye... :-)\n";

  return 0;
}
