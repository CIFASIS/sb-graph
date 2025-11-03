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

#include "parser/file_parser.hpp"

namespace SBG {

namespace Parser {

AST::SBGProgram parseFile(std::string fname)
{
  std::ifstream in(fname.c_str());
  if (in.fail()) 
    SBG::Util::ERROR("Unable to open file ", fname, "\n");
  in.unsetf(std::ios::skipws);

  std::string str((std::istreambuf_iterator<char>(in))
    , std::istreambuf_iterator<char>());
  SBG::Parser::StrIt iter = str.begin();
  SBG::Parser::StrIt end = str.end();

  SBG::Parser::SBGProgramRule g(iter);
  SBG::AST::SBGProgram result;
  bool r = boost::spirit::qi::phrase_parse(iter, end, g
    , SBG::Parser::Skipper<SBG::Parser::StrIt>(), result);

  std::cout << "-----------------------------------\n";
  if (r && iter == end) {
    std::cout << ">>>>>>>>> Parser result <<<<<<<<<<<\n";
    std::cout << "-----------------------------------\n";
    std::cout << "\n" << result << "\n";
  }
  else {
    std::string rest(iter, end);
    std::cout << "Parsing failed\n";
    std::cout << "-----------------------------------\n";
    std::cout << "\nstopped at: \n" << rest << "\n";
  }

  return result;
}

} // namespace Parser

} // namespace SBG
