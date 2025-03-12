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

#include "parser/sbg_program.hpp"
#include "util/logger.hpp"

void parseProgramFromFile(std::string fname)
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
  SBG::AST::SBGProgram result;
  bool r = boost::spirit::qi::phrase_parse(
    iter, end, g, SBG::Parser::Skipper<SBG::Parser::StrIt>(), result
  );

  SBG::Util::SBG_LOG << "-------------------------\n";
  if (r && iter == end) {
    SBG::Util::SBG_LOG << "Parsing succeeded\n";
    SBG::Util::SBG_LOG << "-------------------------\n";
    SBG::Util::SBG_LOG << "\n" << result;
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
  std::string fname = "../../../interval.test";

  if (fname != "") 
    parseProgramFromFile(fname);
  else 
    SBG::Util::SBG_LOG << "A filename should be provided\n";

  return 0;
}
