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

void parseEvalProgramFromFile(std::string fname)
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

  SBG::Util::SBG_LOG << "-------------------------\n";
  if (r && iter == end) {
    SBG::Util::SBG_LOG << "Parsing succeeded\n";
    SBG::Util::SBG_LOG << "-------------------------\n";
    SBG::Util::SBG_LOG << ">>>>>> Eval result <<<<<<\n";
    SBG::Util::SBG_LOG << "-------------------------\n\n";

    SBG::Eval::ProgramVisitor program_visit(false); 
    SBG::Eval::ProgramIO visit_result = boost::apply_visitor(
      program_visit, parser_result
    );
    SBG::Util::SBG_LOG << visit_result;
  }
  else {
    std::string rest(iter, end);
    SBG::Util::SBG_LOG << "Parsing failed\n";
    SBG::Util::SBG_LOG << "-------------------------\n";
    SBG::Util::SBG_LOG << "\nstopped at: \n" << rest << "\n";
  }

  return;
}

int main(int argc, char** argv) 
{
  std::string fname = "../../../set.test";

  if (fname != "")
    parseEvalProgramFromFile(fname);
  else 
    SBG::Util::SBG_LOG << "A filename should be provided\n";

  return 0;
}
